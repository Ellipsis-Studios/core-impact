#include <cugl/net/CUNetworkConnection.h>

#include <cugl/cugl.h>

#include <utility>


#ifdef _WIN32
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <WS2tcpip.h>
#include <WinSock2.h>
#include <fcntl.h>
#pragma comment(lib, "ws2_32")
#include <io.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#ifndef _SSIZE_T_DEFINED
typedef int ssize_t;
#define _SSIZE_T_DEFINED
#endif
#ifndef _SOCKET_T_DEFINED
typedef SOCKET socket_t;
#define _SOCKET_T_DEFINED
#endif
#endif

#include <slikenet/peerinterface.h>



using namespace cugl;

template <class... Fs>
struct overload;

template <class F0, class... Frest>
struct overload<F0, Frest...> : F0, overload<Frest...>
{
	overload(F0 f0, Frest... rest) : F0(f0), overload<Frest...>(rest...) {}

	using F0::operator();
	using overload<Frest...>::operator();
};

template <class F0>
struct overload<F0> : F0
{
	overload(F0 f0) : F0(f0) {}

	using F0::operator();
};

template <class... Fs>
auto make_visitor(Fs... fs)
{
	return overload<Fs...>(fs...);
}

/** How long to block on shutdown */
constexpr unsigned int SHUTDOWN_BLOCK = 10;

/** Length of room IDs */
constexpr uint8_t ROOM_LENGTH = 5;

CUNetworkConnection::CUNetworkConnection(const ConnectionConfig& config)
	: apiVer(config.apiVersion), numPlayers(1), maxPlayers(1), playerID(0) {
	startupConn(config);
	remotePeer = HostPeers(config.maxNumPlayers);
}

CUNetworkConnection::CUNetworkConnection(const ConnectionConfig& config, std::string roomID)
	: apiVer(config.apiVersion), numPlayers(1), maxPlayers(0) {
	startupConn(config);
	remotePeer = ClientPeer(std::move(roomID));
	peer->SetMaximumIncomingConnections(1);
}

CUNetworkConnection::~CUNetworkConnection() {
	peer->Shutdown(SHUTDOWN_BLOCK);
	SLNet::RakPeerInterface::DestroyInstance(peer.release());
}

void CUNetworkConnection::startupConn(const ConnectionConfig& config) {
	peer = std::unique_ptr<SLNet::RakPeerInterface>(SLNet::RakPeerInterface::GetInstance());

	peer->AttachPlugin(&(natPunchthroughClient));
	natPunchServerAddress = std::make_unique<SLNet::SystemAddress>(
		SLNet::SystemAddress(config.punchthroughServerAddr, config.punchthroughServerPort));

	// Use the default socket descriptor
	// This will make the OS assign us a random port.
	SLNet::SocketDescriptor socketDescriptor;
	// Allow connections for each player and one for the NAT server.
	peer->Startup(config.maxNumPlayers, &socketDescriptor, 1);

	CULog("Your GUID is: %s",
		peer->GetGuidFromSystemAddress(SLNet::UNASSIGNED_SYSTEM_ADDRESS).ToString());

	// Connect to the NAT Punchthrough server
	CULog("Connecting to punchthrough server");
	peer->Connect(this->natPunchServerAddress->ToString(false),
		this->natPunchServerAddress->GetPort(), nullptr, 0);
}

void CUNetworkConnection::broadcast(const std::vector<uint8_t>& msg, SLNet::SystemAddress& ignore,
	CustomDataPackets packetType) {
	SLNet::BitStream bs;
	bs.Write(static_cast<uint8_t>(ID_USER_PACKET_ENUM + packetType));
	bs.Write(static_cast<uint8_t>(msg.size()));
	bs.WriteAlignedBytes(msg.data(), static_cast<unsigned int>(msg.size()));
	peer->Send(&bs, MEDIUM_PRIORITY, RELIABLE, 1, ignore, true);
}

void CUNetworkConnection::send(const std::vector<uint8_t>& msg) { send(msg, Standard); }

void CUNetworkConnection::send(const std::vector<uint8_t>& msg, CustomDataPackets packetType) {
	SLNet::BitStream bs;
	bs.Write(static_cast<uint8_t>(ID_USER_PACKET_ENUM + packetType));
	bs.Write(static_cast<uint8_t>(msg.size()));
	bs.WriteAlignedBytes(msg.data(), static_cast<unsigned int>(msg.size()));

	std::visit(make_visitor(
		[&](HostPeers& /*h*/) {
			peer->Send(&bs, MEDIUM_PRIORITY, RELIABLE, 1, *natPunchServerAddress, true);
		},
		[&](ClientPeer& c) {
			if (c.addr == nullptr) {
				return;
			}
			peer->Send(&bs, MEDIUM_PRIORITY, RELIABLE, 1, *c.addr, false);
		}), remotePeer);
}

void CUNetworkConnection::receive(
	const std::function<void(const std::vector<uint8_t>&)>& dispatcher) {
	SLNet::Packet* packet = nullptr;
	for (packet = peer->Receive(); packet != nullptr;
		peer->DeallocatePacket(packet), packet = peer->Receive()) {
		SLNet::BitStream bts(packet->data, packet->length, false);

		// NOLINTNEXTLINE Dealing with an old 2000s era C++ library here
		switch (packet->data[0]) {
		case ID_CONNECTION_REQUEST_ACCEPTED: // Connected to some remote server
			if (packet->systemAddress == *(this->natPunchServerAddress)) {
				CULog("Connected to punchthrough server");

				std::visit(make_visitor(
					[&](HostPeers& h) {
						CULog("Accepting connections now");
						peer->SetMaximumIncomingConnections(h.maxPlayers);
					},
					[&](ClientPeer& c) {
						CULog("Trying to connect to %s", c.room.c_str());
						SLNet::RakNetGUID remote;
						remote.FromString(c.room.c_str());
						this->natPunchthroughClient.OpenNAT(remote,
							*(this->natPunchServerAddress));
					}), remotePeer);
			}
			else {
				std::visit(make_visitor(
					[&](HostPeers& h) {
						for (uint8_t i = 0; i < h.peers.size(); i++) {
							if (*h.peers.at(i) == packet->systemAddress) {
								uint8_t pID = i + 1;
								CULog("Player %d accepted connection request", pID);

								connectedPlayers.set(pID);
								std::vector<uint8_t> joinMsg = { pID };
								broadcast(joinMsg, packet->systemAddress, PlayerJoined);
								numPlayers++;

								if (h.started) {
									// Reconnection attempt
									SLNet::BitStream bs;
									std::vector<uint8_t> connMsg = { numPlayers, pID, apiVer };
									bs.Write(
										static_cast<uint8_t>(ID_USER_PACKET_ENUM + Reconnect));
									bs.Write(static_cast<uint8_t>(connMsg.size()));
									bs.WriteAlignedBytes(
										connMsg.data(),
										static_cast<unsigned int>(connMsg.size()));
									peer->Send(&bs, MEDIUM_PRIORITY, RELIABLE, 1,
										packet->systemAddress, false);
								}
								else {
									SLNet::BitStream bs;
									std::vector<uint8_t> connMsg = { numPlayers, pID, apiVer };
									bs.Write(
										static_cast<uint8_t>(ID_USER_PACKET_ENUM + JoinRoom));
									bs.Write(static_cast<uint8_t>(connMsg.size()));
									bs.WriteAlignedBytes(
										connMsg.data(),
										static_cast<unsigned int>(connMsg.size()));
									peer->Send(&bs, MEDIUM_PRIORITY, RELIABLE, 1,
										packet->systemAddress, false);
									maxPlayers++;
								}
								break;
							}
						}
					},
					[&](ClientPeer& /*c*/) {
						CULogError(
							"A connection request you sent was accepted despite being client?");
					}), remotePeer);
			}
			break;
		case ID_NEW_INCOMING_CONNECTION: // Someone connected to you
			CULog("A peer connected");
			std::visit(make_visitor(
				[&](HostPeers& /*h*/) { CULogError("How did that happen? You're the host"); },
				[&](ClientPeer& c) {
					if (packet->systemAddress == *c.addr) {
						CULog("Connected to host :D");
					}
				}), remotePeer);
			break;
		case ID_NAT_PUNCHTHROUGH_SUCCEEDED: // Punchthrough succeeded
			CULog("Punchthrough success");

			std::visit(make_visitor(
				[&](HostPeers& h) {
					auto p = packet->systemAddress;

					bool hasRoom = false;
					for (uint8_t i = 0; i < h.peers.size(); i++) {
						if (h.peers.at(i) == nullptr) {
							hasRoom = true;
							h.peers.at(i) = std::make_unique<SLNet::SystemAddress>(p);
							// numPlayers++;
							break;
						}
					}

					if (hasRoom) {
						CULog("Connecting to client now");
						peer->Connect(p.ToString(false), p.GetPort(), nullptr, 0);
					}
					else {
						CULogError(
							"Client attempted to join but room was full - if you're seeing "
							"this error, that means somehow there are ghost clients not "
							"actually connected even though mib thinks they are");
					}
				},
				[&](ClientPeer& c) {
					c.addr = std::make_unique<SLNet::SystemAddress>(packet->systemAddress);
				}), remotePeer);
			break;
		case ID_NAT_TARGET_NOT_CONNECTED:
			// TODO report error
			// dispatcher({NetworkDataType::JoinRoom, 1});
			break;
		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
		case ID_REMOTE_CONNECTION_LOST:
		case ID_DISCONNECTION_NOTIFICATION:
		case ID_CONNECTION_LOST:
			CULog("A disconnect occured");
			std::visit(make_visitor(
				[&](HostPeers& h) {
					for (uint8_t i = 0; i < h.peers.size(); i++) {
						if (h.peers.at(i) == nullptr) {
							continue;
						}
						if (*h.peers.at(i) == packet->systemAddress) {
							uint8_t pID = i + 1;
							CULog("Lost connection to player %d", pID);
							std::vector<uint8_t> disconnMsg{ pID };
							h.peers.at(i) = nullptr;
							numPlayers--;
							connectedPlayers.reset(pID);
							// dispatcher(disconnMsg);
							send(disconnMsg, PlayerLeft);
							return;
						}
					}
				},
				[&](ClientPeer& c) {
					if (packet->systemAddress == *c.addr) {
						CULog("Lost connection to host");
						connectedPlayers.reset(0);
						// TODO more robust disconnect communication
					}
				}), remotePeer);

			break;
		case ID_NAT_PUNCHTHROUGH_FAILED:
		case ID_CONNECTION_ATTEMPT_FAILED:
		case ID_NAT_TARGET_UNRESPONSIVE: {
			CULogError("Punchthrough failure %d", packet->data[0]); // NOLINT
			// dispatcher({NetworkDataType::GenericError});

			// TODO report error
			bts.IgnoreBytes(sizeof(SLNet::MessageID));
			SLNet::RakNetGUID recipientGuid;
			bts.Read(recipientGuid);

			CULogError("Attempted punchthrough to GUID %s failed", recipientGuid.ToString());
			break;
		}
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			CULog("Room full");
			// TODO report error
			break;

			// Begin Non-SLikeNet Reported Codes
		case ID_USER_PACKET_ENUM + Standard: {
			// More Old C++ Library Shenanigans
			uint8_t ignored; // NOLINT
			bts.Read(ignored);
			uint8_t length; // NOLINT
			bts.Read(length);
			uint8_t* message = new uint8_t[length]; // NOLINT
			bts.ReadAlignedBytes(message, length);
			// NOLINTNEXTLINE
			std::vector<uint8_t> msgConverted(&message[0], &message[length]);

			dispatcher(msgConverted);

			std::visit(make_visitor(
				[&](HostPeers& /*h*/) { broadcast(msgConverted, packet->systemAddress); },
				[&](ClientPeer& c) {}), remotePeer);

			delete[] message; // NOLINT
			break;
		}
		case ID_USER_PACKET_ENUM + AssignedRoom: {
			if (playerID != 0) {
				CULog("Assigned room ID but ignoring");
				break;
			}
			uint8_t ignored;
			bts.Read(ignored);
			uint8_t length;
			bts.Read(length);
			uint8_t* message = new uint8_t[length];
			bts.ReadAlignedBytes(message, length);
			std::vector<uint8_t> msgConverted(&message[0], &message[length]);
			std::stringstream newRoomId;
			for (size_t i = 0; i < ROOM_LENGTH; i++) {
				newRoomId << static_cast<char>(msgConverted[i]);
			}
			connectedPlayers.set(0);
			roomID = newRoomId.str();
			CULog("Got room ID: %s", roomID.c_str());
			break;
		}
		case ID_USER_PACKET_ENUM + JoinRoom: {
			uint8_t ignored;
			bts.Read(ignored);
			uint8_t length;
			bts.Read(length);
			uint8_t* message = new uint8_t[length];
			bts.ReadAlignedBytes(message, length);
			std::vector<uint8_t> msgConverted(&message[0], &message[length]);

			std::visit(make_visitor(
				[&](HostPeers& /*h*/) { CULogError("Received join room message as host"); },
				[&](ClientPeer& c) {
					if (msgConverted[2] != apiVer) {
						CULogError("API version mismatch; currently %d but host was %d", apiVer,
							msgConverted[2]);
						// TODO report API mismatch error
					}
					numPlayers = msgConverted[0];
					maxPlayers = numPlayers;
					playerID = msgConverted[1];
				}), remotePeer);
			break;
		}
		case ID_USER_PACKET_ENUM + JoinRoomFail: {
			// TODO
			break;
		}
		case ID_USER_PACKET_ENUM + Reconnect: {
			// TODO
			break;
		}
		case ID_USER_PACKET_ENUM + PlayerJoined: {
			uint8_t ignored;
			bts.Read(ignored);
			uint8_t length;
			bts.Read(length);
			uint8_t* message = new uint8_t[length];
			bts.ReadAlignedBytes(message, length);
			std::vector<uint8_t> msgConverted(&message[0], &message[length]);

			std::visit(make_visitor(
				[&](HostPeers& /*h*/) { CULogError("Received player joined message as host"); },
				[&](ClientPeer& c) {
					connectedPlayers.set(msgConverted[0]);
					numPlayers++;
					maxPlayers++;
				}), remotePeer);

			break;
		}
		case ID_USER_PACKET_ENUM + PlayerLeft: {
			uint8_t ignored;
			bts.Read(ignored);
			uint8_t length;
			bts.Read(length);
			uint8_t* message = new uint8_t[length];
			bts.ReadAlignedBytes(message, length);
			std::vector<uint8_t> msgConverted(&message[0], &message[length]);

			std::visit(make_visitor(
				[&](HostPeers& /*h*/) { CULogError("Received player left message as host"); },
				[&](ClientPeer& c) {
					connectedPlayers.reset(msgConverted[0]);
					numPlayers--;
				}), remotePeer);
			break;
		}
		default:
			CULog("Received unknown message: %d", packet->data[0]);
			break;
		}
	}
}

void CUNetworkConnection::startGame() {
	std::visit(make_visitor([&](HostPeers& h) { h.started = true; }, [&](ClientPeer& c) {}), remotePeer);
	maxPlayers = numPlayers;
}
