#pragma once
#include "GameObject.h"

#include <string>

namespace NCL::CSC8503 {
	class GameObject;

	enum BehaviourState {
		Initialise,
		Failure,
		Success,
		Ongoing
	};

	class BehaviourNode {
	public:
		BehaviourNode(const std::string& nodeName) {
			name = nodeName;
			currentState = Initialise;
		}

		virtual ~BehaviourNode() {}
		virtual BehaviourState Execute(float dt, GameObject* obj) = 0;

		//BehaviourState	GetState() const {return currentState; }
		virtual void	Reset() {
			currentState = Initialise;
		}

		BehaviourState	currentState;

		std::string		name;
	};
}