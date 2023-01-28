#pragma once
//#include "./enet/enet.h"
#include <map>

struct _ENetHost;
struct _ENetPeer;
struct _ENetEvent;

namespace NCL {
	namespace CSC8503 {
		enum BasicNetworkMessages {
			None,
			Hello,
			Message,
			String_Message,
			Delta_State,	//1 byte per channel since the last state
			Full_State,		//Full transform etc
			Received_State, //received from a client, informs that its received packet n
			Client,
			Player_Connected,
			Player_Connected_Ack,
			Player_Disconnected,
			Shutdown
		};

		struct GamePacket {
			short size;
			short type;

			GamePacket() {
				type = BasicNetworkMessages::None;
				size = 0;
			}

			GamePacket(short type) : GamePacket() {
				this->type = type;
			}

			int GetTotalSize() {
				return sizeof(GamePacket) + size;
			}
		};

		struct StringPacket : public GamePacket {
			char stringData[256];

			StringPacket(const std::string& message) {
				type = BasicNetworkMessages::String_Message;
				size = (short)message.length();

				memcpy(stringData, message.data(), size);
			};

			std::string GetStringFromData() {
				std::string realString(stringData);
				realString.resize(size);
				return realString;
			}
		};

		struct ClientConnectPacket : public GamePacket {
			int id;

			ClientConnectPacket(int id) {
				type = BasicNetworkMessages::Player_Connected;
				this->id = id;
				size = sizeof(int);
			}
		};

		struct ClientAcknowledgePacket : public GamePacket {
			int id;

			ClientAcknowledgePacket(int id) {
				type = BasicNetworkMessages::Player_Connected_Ack;
				this->id = id;
				size = sizeof(int);
			}
		};

		struct HelloPacket : public GamePacket {
			HelloPacket() {
				type = BasicNetworkMessages::Hello;
				size = 0;
			}
		};

		class PacketReceiver {
		public:
			virtual void ReceivePacket(int type, GamePacket* payload, int source = -1) = 0;
		};

		class NetworkBase {
		public:
			static void Initialise();
			static void Destroy();

			static int GetDefaultPort() {
				return 1234;
			}

			void RegisterPacketHandler(int msgID, PacketReceiver* receiver) {
				packetHandlers.insert(std::make_pair(msgID, receiver));
			}
		protected:
			NetworkBase();
			~NetworkBase();

			bool ProcessPacket(GamePacket* p, int peerID = -1);

			typedef std::multimap<int, PacketReceiver*>::const_iterator PacketHandlerIterator;

			bool GetPacketHandlers(int msgID, PacketHandlerIterator& first, PacketHandlerIterator& last) const {
				auto range = packetHandlers.equal_range(msgID);

				if (range.first == packetHandlers.end()) {
					return false; //no handlers for this message type!
				}
				first = range.first;
				last = range.second;
				return true;
			}

			_ENetHost* netHandle;

			std::multimap<int, PacketReceiver*> packetHandlers;
		};
	}
}