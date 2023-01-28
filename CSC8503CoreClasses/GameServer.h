#pragma once
#include "NetworkBase.h"

#include <map>

//struct ENetPeer;

namespace NCL {
	namespace CSC8503 {
		class NetworkedGame;
		class GameWorld;
		class GameServer : public NetworkBase {
		public:
			GameServer(int onPort, int maxClients);
			~GameServer();

			bool Initialise();
			void Shutdown();

			void SetGameWorld(GameWorld &g);

			bool SendGlobalPacket(int msgID);
			bool SendGlobalPacket(GamePacket& packet);
			void SendPacket(GamePacket& packet, _ENetPeer* peer);

			int GetMostRecentID() { return clientMaxID - 1; }

			virtual void UpdateServer();
		protected:
			int			port;
			int			clientMax;
			int			clientCount;
			
			GameWorld*	gameWorld;

			std::map<int, _ENetPeer*> clients;
			int         clientMaxID;

			int incomingDataRate;
			int outgoingDataRate;
		};
	}
}
