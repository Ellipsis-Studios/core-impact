#ifndef CU_NETWORK_CONNECTION_H
#define CU_NETWORK_CONNECTION_H

#include <array>
#include <bitset>
#include <functional>
#include <string>
#include <thread>
#include <vector>
#include <optional>
#include <variant>

#include <slikenet/BitStream.h>
#include <slikenet/MessageIdentifiers.h>
#include <slikenet/NatPunchthroughClient.h>

namespace SLNet {
	class RakPeerInterface;
}

namespace cugl {
	class CUNetworkConnection {
	public:
		/**
		 * Basic data needed to setup a connection
		 */
		struct ConnectionConfig {
			/** Address of the NAT Punchthrough server */
			const char* punchthroughServerAddr;
			/** Port to connect on the NAT Punchthrough server */
			uint16_t punchthroughServerPort;
			/** Maximum number of players allowed per game */
			uint32_t maxNumPlayers;
			/** API version number */
			uint8_t apiVersion;

			ConnectionConfig(const char* punchthroughServerAddr, uint16_t punchthroughServerPort, uint32_t maxPlayers, uint8_t apiVer) {
				this->punchthroughServerAddr = punchthroughServerAddr;
				this->punchthroughServerPort = punchthroughServerPort;
				this->maxNumPlayers = maxPlayers;
				this->apiVersion = apiVer;
			}
		};

		/**
		 * Start a new network connection as host.
		 *
		 * @param setup Connection config
		 */
		explicit CUNetworkConnection(const ConnectionConfig& config);

		/**
		 * Start a new network connection as client.
		 *
		 * @param setup Connection config
		 * @param roomID The RakNet GUID of the host.
		 */
		CUNetworkConnection(const ConnectionConfig& config, std::string roomID);

		/** Delete and cleanup this connection. */
		~CUNetworkConnection();

		/**
		 * Sends a byte array to all other players.
		 *
		 * This requires a connection be established. If not, this is a noop.
		 *
		 * @param msg The byte array to send.
		 */
		void send(const std::vector<uint8_t>& msg);

		/**
		 * Method to call every frame to process incoming network messages.
		 *
		 * @param dispatcher Function that will be called on every byte array sent by other players.
		 */
		void receive(const std::function<void(const std::vector<uint8_t>&)>& dispatcher);

		/**
		 * Mark the game as started and ban incoming connections except for reconnects.
		 * PRECONDITION: Should only be called by host.
		 */
		void startGame();

		/**
		 * Returns the player ID or empty.
		 * 
		 * If this player is the host, this is guaranteed to be 0, even before a connection is established.
		 * 
		 * Otherwise, as client, this will return empty until connected to host and a player ID is assigned.
		 */
		std::optional<uint8_t> getPlayerID() { return playerID; }

		/**
		 * Returns the room ID or empty string.
		 * 
		 * If this player is a client, this will return the room ID this object was constructed with.
		 * 
		 * Otherwise, as host, this will return the empty string until connected to the punchthrough server
		 * and a room ID is assigned.
		 */
		std::string getRoomID() { return roomID; }

		/** Return the number of players currently connected to this game */
		uint8_t getNumPlayers() { return numPlayers; }

		/** Return the number of players present when the game was started
		 *  (including players that may have disconnected) */
		uint8_t getTotalPlayers() { return maxPlayers;  }

	private:
		/** Connection object */
		std::unique_ptr<SLNet::RakPeerInterface> peer;

#pragma region State
		/** API version number */
		const uint8_t apiVer;
		/** Number of players currently connected */
		uint8_t numPlayers;
		/** Number of players connected when the game started */
		uint8_t maxPlayers;
		/** Current player ID */
		std::optional<uint8_t> playerID;
		/** Connected room ID */
		std::string roomID;
		/** Which players are active */
		std::bitset<256> connectedPlayers;
#pragma endregion

#pragma region Punchthrough
		/** Address of punchthrough server */
		std::unique_ptr<SLNet::SystemAddress> natPunchServerAddress;
		/** NAT Punchthrough Client */
		SLNet::NatPunchthroughClient natPunchthroughClient;
#pragma endregion

#pragma region Connection Data Structures
		struct HostPeers {
			bool started;
			uint32_t maxPlayers;
			std::vector<std::unique_ptr<SLNet::SystemAddress>> peers;

			HostPeers() : started(false), maxPlayers(6) {
				for (uint8_t i = 0; i < 5; i++) {
					peers.push_back(nullptr);
				}
			};
			explicit HostPeers(uint32_t max) : started(false), maxPlayers(max) {
				for (uint8_t i = 0; i < max - 1; i++) {
					peers.push_back(nullptr);
				}
			};
		};

		/** Connection to host and room ID for client */
		struct ClientPeer {
			std::unique_ptr<SLNet::SystemAddress> addr;
			std::string room;

			explicit ClientPeer(std::string roomID) { room = std::move(roomID); }
		};

		/**
		 * Collection of peers for the host, or the host for clients
		 */
		std::variant<HostPeers, ClientPeer> remotePeer;
#pragma endregion

		enum CustomDataPackets {
			Standard = 0,
			AssignedRoom,
			// Request to join, or success
			JoinRoom,
			// Couldn't find room
			JoinRoomFail,
			Reconnect,
			PlayerJoined,
			PlayerLeft
		};

		/** Initialize the connection */
		void startupConn(const ConnectionConfig& config);

		/**
		 * Broadcast a message to everyone except the specified connection.
		 *
		 * PRECONDITION: This player MUST be the host
		 *
		 * @param packetType Packet type from RakNet
		 * @param msg The message to send
		 * @param ignore The address to not send to
		 */
		void broadcast(const std::vector<uint8_t>& msg, SLNet::SystemAddress& ignore,
			CustomDataPackets packetType = Standard);

		void send(const std::vector<uint8_t>& msg, CustomDataPackets packetType);

	};
}

#endif // CU_NETWORK_CONNECTION_H