#pragma once
#include "TutorialGame.h"
#include "NetworkBase.h"

namespace NCL {
	namespace CSC8503 {
		class GameServer;
		class GameClient;
		class NetworkPlayer;

		class NetworkedGame : public TutorialGame, public PacketReceiver {
		public:
			NetworkedGame();
			~NetworkedGame();

			void StartAsServer();
			void StartAsClient(char a, char b, char c, char d);

			void UpdateGame(float dt) override;

			void SpawnPlayer(int id);

			void StartLevel();

			void ReceivePacket(int type, GamePacket* payload, int source) override;

			NetworkPlayer* AddNetworkPlayerToWorld(const Vector3& position, const int num);

			void InitNetworkGame();

			void OnPlayerCollision(NetworkPlayer* a, NetworkPlayer* b);

			void UpdateName();

			GameServer* GetServer() { return thisServer; }
			GameClient* GetClient() { return thisClient; }

		protected:
			void UpdateAsServer(float dt);
			void UpdateAsClient(float dt);

			void BroadcastSnapshot(bool deltaFrame);
			void UpdateMinimumState();
			std::map<int, int> stateIDs;

			std::string name;

			GameServer* thisServer;
			GameClient* thisClient;
			float timeToNextPacket;
			int packetsToSnapshot;

			std::vector<NetworkObject*> networkObjects;

			std::map<int, GameObject*> serverPlayers;
			NetworkPlayer* localPlayer;
		};
	}
}

