#pragma once
#include "PhysicsSystem.h"
#include "GameWorld.h"
#include "Camera.h"

#include <string>

namespace NCL{
	namespace CSC8503 {
		class GameObject;

		class Player : public GameObject {
		public:
			Player(GameWorld* world, PhysicsSystem* physics, std::string name = "") : GameObject(name) {
				speed = 8.0f;
				rotSpeed = 0.85f;
				jumpHeight = 7.0f;
				isGrounded = false;
				score = 0;

				this->world = world;
				this->physics = physics;
				camera = world->GetMainCamera();
				cameraOffsetV = 1.5f;
				cameraOffsetH = 4.5f;

				Vector3 verticalOffset = Vector3(0, 1, 0) * cameraOffsetV;
				Vector3 horizontalOffset = transform.GetOrientation() * (Vector3(0, 0, 1) * cameraOffsetH);

				camera->SetPosition(transform.GetPosition() + verticalOffset + horizontalOffset);
				camera->SetYaw(transform.GetOrientation().ToEuler().y);
			}
			~Player() {}

			void UpdatePlayer(const float dt);

			void OnCollisionBegin(GameObject* otherObject);

			int destroyCount;
			int score;
		protected:
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