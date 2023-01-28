#pragma once
#include "GameObject.h"

#include <string>

namespace NCL{
	namespace CSC8503 {
		class GameObject;

		class DestructibleObject : public GameObject {
		public:
			DestructibleObject(int value = 1, std::string name = "") : GameObject(name) {
				this->value = value;
				destroy = false;
				maxTime = 0;
				time = 0;
			}
			~DestructibleObject() {}

			void UpdateObject(const float dt);
			void Destroy(const float timer);

			int value;
		protected:
			bool destroy;
			float maxTime;
			float time;
		};
	}
}