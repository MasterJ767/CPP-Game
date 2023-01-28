#pragma once
#include "NavigationGrid.h"
#include "BehaviourSelector.h"
#include "BehaviourSequence.h"
#include "BehaviourAction.h"
#include "Player.h"

#include "Quaternion.h"

#include <string>

namespace NCL{
	namespace CSC8503 {
		class GameObject;

		class Human : public GameObject {
		public:
			Human(GameWorld* world, Player* player, Vector3 spawnPoint, NavigationGrid* navGrid, std::string name = "");
			~Human() {}

			void UpdateObject(const float dt);

			void OnCollisionBegin(GameObject* otherObject);

			Vector3 spawnPoint;
		protected:
			float speed;
			float rotSpeed;

			GameWorld* world;
			Player* player;
			NavigationGrid* navGrid;
			BehaviourSelector* root;
			int node;
			vector<Vector3> route;
		};
	}
}