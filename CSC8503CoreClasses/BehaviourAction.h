#pragma once
#include "BehaviourNode.h"
#include "GameObject.h"

namespace NCL::CSC8503 {
	class GameObject;

	typedef std::function<BehaviourState(float, GameObject*, BehaviourState)> BehaviourActionFunc;

	class BehaviourAction : public BehaviourNode {
	public:
		BehaviourAction(const std::string& nodeName, BehaviourActionFunc f) : BehaviourNode(nodeName) {
			function = f;
		}
		BehaviourState Execute(float dt, GameObject* obj) override {
			//std::cout << "Executing action " << name << "\n";
			currentState = function(dt, obj, currentState);
			return currentState;
		}
	protected:
		BehaviourActionFunc function;
	};
}