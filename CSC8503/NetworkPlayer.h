#pragma once
#include "GameObject.h"
#include "GameClient.h"

namespace NCL {
	class Camera;

	namespace CSC8503 {
		class NetworkedGame;
		class GameWorld;
		class PhysicsSystem;

		class NetworkPlayer : public GameObject {
		public:
			NetworkPlayer(NetworkedGame* game, int num, std::string name = "");
			~NetworkPlayer();

			void UpdatePlayer(const float dt);

			void OnCollisionBegin(GameObject* otherObject) override;

			int GetPlayerNum() const {
				return playerNum;
			}

		protected:
			NetworkedGame* game;
			int playerNum;

			float speed;
			float rotSpeed;
			float jumpHeight;
			bool isGrounded;

			Camera* camera;
			GameWorld* world;
			PhysicsSystem* physics;
			float cameraOffsetV;
			float cameraOffsetH;
		};
	}
}

