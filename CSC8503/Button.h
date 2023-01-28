#pragma once
#include "GameObject.h"
#include "NavigationGrid.h"
#include "TutorialGame.h"

#include <string>

namespace NCL {
	namespace CSC8503 {
		class GameObject;

		class Button : public GameObject {
		public:
			Button(TutorialGame* game, NavigationGrid* navGrid, std::string name = "") : GameObject(name) {
				this->game = game;
				this->navGrid = navGrid;
			}
			~Button() {}

			void SwitchColour();

			void OnCollisionBegin(GameObject* otherObject);
		protected:
			TutorialGame* game;
			NavigationGrid* navGrid;
		};
	}
}