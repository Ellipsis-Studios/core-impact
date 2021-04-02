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
	: status(NetStatus::Pending), apiVer(config.apiVersion), numPlayers(1), maxPlayers(1), playerID(0) {
	c0StartupConn(config);
	remotePeer = HostPeers(config.maxNumPlayers);
}

CUNetworkConnection::CUNetworkConnection(const ConnectionConfig& config, std::string roomID)
	: status(NetStatus::Pending), apiVer(config.apiVersion), numPlayers(1), maxPlayers(0) {
	c0StartupConn(config);
	remotePeer = ClientPeer(std::move(roomID));
	peer->SetMaximumIncomingConnections(1);
}

CUNetworkConnection::~CUNetworkConnection() {
	peer->Shutdown(SHUTDOWN_BLOCK);
	SLNet::RakPeerInterface::DestroyInstance(peer.release());
}

/**
 * Read the message from a bitstream into a byte vector.
 *
 * Only works if the BitStream was encoded in the standard format used by this clas.
 */
std::vector<uint8_t> readBs(SLNet::BitStream& bts) {
	uint8_t ignored;
	bts.Read(ignored);
	uint8_t length;
	bts.Read(length);

	std::vector<uint8_t> msgConverted;
	msgConverted.resize(length, 0);

	bts.ReadAlignedBytes(msgConverted.data(), length);
	return msgConverted;
}

#pragma region Connection Handshake

void CUNetworkConnection::c0StartupConn(const ConnectionConfig& config) {
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

void cugl::CUNetworkConnection::ch1HostConnServer(HostPeers& h) {
	CULog("Connected to punchthrough server; awaiting room ID");
}

void cugl::CUNetworkConnection::ch2HostGetRoomID(HostPeers& h, SLNet::BitStream& bts) {
	auto msgConverted = readBs(bts);
	std::stringstream newRoomId;
	for (size_t i = 0; i < ROOM_LENGTH; i++) {
		newRoomId << static_cast<char>(msgConverted[i]);
	}
	connectedPlayers.set(0);
	roomID = newRoomId.str();
	CULog("Got room ID: %s; Accepting Connections Now", roomID.c_str());
	status = NetStatus::Connected;
}

void cugl::CUNetworkConnection::cc1ClientConnServer(ClientPeer& c) {
	CULog("Connected to punchthrough server");
	CULog("Trying to connect to %s", c.room.c_str());
	SLNet::RakNetGUID remote;
	remote.FromString(c.room.c_str());
	this->natPunchthroughClient.OpenNAT(remote,
		*(this->natPunchServerAddress));
}

void cugl::CUNetworkConnection::cc2ClientPunchSuccess(ClientPeer& c, SLNet::Packet* packet) {
	c.addr = std::make_unique<SLNet::SystemAddress>(packet->systemAddress);
}

void cugl::CUNetworkConnection::cc3HostReceivedPunch(HostPeers& h, SLNet::Packet* packet) {
	auto p = packet->systemAddress;
	CULog("Host received punchthrough; curr num players %d", peer->NumberOfConnections());



	bool hasRoom = false;
	if(!h.started || numPlayers < maxPlayers) {
		for (uint8_t i = 0; i < h.peers.size(); i++) {
			if (h.peers.at(i) == nullptr) {
				hasRoom = true;
				h.peers.at(i) = std::make_unique<SLNet::SystemAddress>(p);
				break;
			}
		}
	}

	if (!hasRoom) {
		// Client is still waiting for a response at this stage,
		// so we need to connect to them first before telling them no.
		// Store address to reject so we know this connection is invalid.
		h.toReject.insert(p.ToString());
		CULog("Client attempted to join but room was full");
	}

	CULog("Connecting to client now");
	peer->Connect(p.ToString(false), p.GetPort(), nullptr, 0);
}

void cugl::CUNetworkConnection::cc4ClientReceiveHostConnection(ClientPeer& c, SLNet::Packet* packet) {
	if (packet->systemAddress == *c.addr) {
		CULog("Connected to host :D");
	}
}

void cugl::CUNetworkConnection::cc5HostConfirmClient(HostPeers& h, SLNet::Packet* packet) {

	if (h.toReject.count(packet->systemAddress.ToString()) > 0) {
		CULog("Rejecting player connection - bye :(");

		h.toReject.erase(packet->systemAddress.ToString());

		SLNet::BitStream bs;
		bs.Write(
			static_cast<uint8_t>(ID_USER_PACKET_ENUM + JoinRoomFail));
		peer->Send(&bs, MEDIUM_PRIORITY, RELIABLE, 1,
			packet->systemAddress, false);

		peer->CloseConnection(packet->systemAddress, true);
	}

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
				std::vector<uint8_t> connMsg = { numPlayers, maxPlayers, pID, apiVer };
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
				// New player connection
				maxPlayers++;
				SLNet::BitStream bs;
				std::vector<uint8_t> connMsg = { numPlayers, maxPlayers, pID, apiVer };
				bs.Write(
					static_cast<uint8_t>(ID_USER_PACKET_ENUM + JoinRoom));
				bs.Write(static_cast<uint8_t>(connMsg.size()));
				bs.WriteAlignedBytes(
					connMsg.data(),
					static_cast<unsigned int>(connMsg.size()));
				peer->Send(&bs, MEDIUM_PRIORITY, RELIABLE, 1,
					packet->systemAddress, false);
			}
			break;
		}
	}

	CULog("Host confirmed players; curr num players %d", peer->NumberOfConnections());
}

void cugl::CUNetworkConnection::cc6ClientAssignedID(ClientPeer& c, const std::vector<uint8_t>& msgConverted) {
	if (msgConverted[3] != apiVer) {
		CULogError("API version mismatch; currently %d but host was %d", apiVer,
			msgConverted[3]);
		status = NetStatus::ApiMismatch;
	}
	numPlayers = msgConverted[0];
	maxPlayers = msgConverted[1];
	playerID = msgConverted[2];
	peer->CloseConnection(*natPunchServerAddress, true);
	status = NetStatus::Connected;
}

#pragma endregion

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

		switch (packet->data[0]) {
		case ID_CONNECTION_REQUEST_ACCEPTED:
			// Connected to some remote server
			if (packet->systemAddress == *(this->natPunchServerAddress)) {
				// Punchthrough server
				std::visit(make_visitor(
					[&](HostPeers& h) { ch1HostConnServer(h); },
					[&](ClientPeer& c) { cc1ClientConnServer(c); }), remotePeer);
			}
			else {
				std::visit(make_visitor(
					[&](HostPeers& h) { cc5HostConfirmClient(h, packet); },
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
				[&](ClientPeer& c) { cc4ClientReceiveHostConnection(c, packet); }), remotePeer);
			break;
		case ID_NAT_PUNCHTHROUGH_SUCCEEDED: // Punchthrough succeeded
			CULog("Punchthrough success");

			std::visit(make_visitor(
				[&](HostPeers& h) { cc3HostReceivedPunch(h, packet); },
				[&](ClientPeer& c) { cc2ClientPunchSuccess(c, packet); }), remotePeer);
			break;
		case ID_NAT_TARGET_NOT_CONNECTED:
			status = NetStatus::GenericError;
			break;
		case ID_REMOTE_DISCONNECTION_NOTIFICATION:
		case ID_REMOTE_CONNECTION_LOST:
		case ID_DISCONNECTION_NOTIFICATION:
		case ID_CONNECTION_LOST:
			CULog("Received disconnect notification");
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
							send(disconnMsg, PlayerLeft);
							return;
						}
					}
				},
				[&](ClientPeer& c) {
					if (packet->systemAddress == *natPunchServerAddress) {
						CULog("Successfully disconnected from Punchthrough server");
					}
					if (packet->systemAddress == *c.addr) {
						CULog("Lost connection to host");
						connectedPlayers.reset(0);
						switch (status) {
						case NetStatus::Pending:
							status = NetStatus::GenericError;
							break;
						case NetStatus::Connected:
							status = NetStatus::Reconnecting;
							break;
						case NetStatus::Reconnecting:
						case NetStatus::Disconnected:
						case NetStatus::RoomNotFound:
						case NetStatus::ApiMismatch:
						case NetStatus::GenericError:
							return;
						}
					}
				}), remotePeer);

			break;
		case ID_NAT_PUNCHTHROUGH_FAILED:
		case ID_CONNECTION_ATTEMPT_FAILED:
		case ID_NAT_TARGET_UNRESPONSIVE: {
			CULogError("Punchthrough failure %d", packet->data[0]);

			status = NetStatus::GenericError;
			bts.IgnoreBytes(sizeof(SLNet::MessageID));
			SLNet::RakNetGUID recipientGuid;
			bts.Read(recipientGuid);

			CULogError("Attempted punchthrough to GUID %s failed", recipientGuid.ToString());
			break;
		}
		case ID_NO_FREE_INCOMING_CONNECTIONS:
			status = NetStatus::RoomNotFound;
			break;

		// Begin Non-SLikeNet Reported Codes
		case ID_USER_PACKET_ENUM + Standard: {
			auto msgConverted = readBs(bts);
			dispatcher(msgConverted);

			std::visit(make_visitor(
				[&](HostPeers& /*h*/) { broadcast(msgConverted, packet->systemAddress); },
				[&](ClientPeer& c) {}), remotePeer);

			break;
		}
		case ID_USER_PACKET_ENUM + AssignedRoom: {

			std::visit(make_visitor(
				[&](HostPeers& h) { ch2HostGetRoomID(h, bts); },
				[&](ClientPeer& c) {CULog("Assigned room ID but ignoring"); }), remotePeer);
			
			break;
		}
		case ID_USER_PACKET_ENUM + JoinRoom: {
			auto msgConverted = readBs(bts);

			std::visit(make_visitor(
				[&](HostPeers& /*h*/) { CULogError("Received join room message as host"); },
				[&](ClientPeer& c) {
					cc6ClientAssignedID(c, msgConverted);
				}), remotePeer);
			break;
		}
		case ID_USER_PACKET_ENUM + JoinRoomFail: {
			CULog("Failed to join room");
			status = NetStatus::RoomNotFound;
			break;
		}
		case ID_USER_PACKET_ENUM + Reconnect: {
			// TODO
			break;
		}
		case ID_USER_PACKET_ENUM + PlayerJoined: {
			auto msgConverted = readBs(bts);

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
			auto msgConverted = readBs(bts);

			std::visit(make_visitor(
				[&](HostPeers& /*h*/) { CULogError("Received player left message as host"); },
				[&](ClientPeer& c) {
					connectedPlayers.reset(msgConverted[0]);
					numPlayers--;
				}), remotePeer);
			break;
		}
		case ID_USER_PACKET_ENUM + StartGame: {
			startGame();
			break;
		}
		default:
			CULog("Received unknown message: %d", packet->data[0]);
			break;
		}
	}
}

void CUNetworkConnection::startGame() {
	CULog("Starting Game");
	std::visit(make_visitor([&](HostPeers& h) { 
		h.started = true;
		broadcast({}, const_cast<SLNet::SystemAddress&>(SLNet::UNASSIGNED_SYSTEM_ADDRESS), StartGame);
		}, [&](ClientPeer& c) {}), remotePeer);
	maxPlayers = numPlayers;
}

cugl::CUNetworkConnection::NetStatus cugl::CUNetworkConnection::getStatus() {
	return status;
}
