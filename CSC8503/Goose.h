#pragma once
#include "NavigationGrid.h"

#include "Quaternion.h"
#include "Vector3.h"

#include <string>
#include <vector>

/*namespace NCL {
	namespace CSC8503 {
		class GameObject;
		class GameWorld;
		class StateGameObject;
		class Player;
		class BehaviourSelector;

		class Goose : public GameObject {
		public:
			Goose(GameWorld* world, Player* player, Vector3 spawnPoint, NavigationGrid* navGrid, std::string name = "");
			~Goose() {}

			void UpdateObject(const float dt);

			void OnCollisionBegin(GameObject* otherObject);

			Vector3 spawnPoint;
		protected:
			float speed;
			float rotSpeed;
			bool chasing;

			StateGameObject* state;
			GameWorld* world;
			Player* player;
			NavigationGrid* navGrid;
			BehaviourSelector* root;
			int node;
			vector<Vector3> route;
		};
	}
}*/