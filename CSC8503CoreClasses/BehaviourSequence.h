#pragma once
#include "BehaviourNodeWithChildren.h"

namespace NCL::CSC8503 {
	class BehaviourSequence : public BehaviourNodeWithChildren {
	public:
		BehaviourSequence(const std::string& nodeName) : BehaviourNodeWithChildren(nodeName) {}
		~BehaviourSequence() {}
		BehaviourState Execute(float dt, GameObject* obj) override {
			//std::cout << "Executing sequence " << name << "\n";
			for (auto& i : childNodes) {
				BehaviourState nodeState = i->Execute(dt, obj);
				switch (nodeState) {
				case Success: continue;
				case Failure:
				case Ongoing:
				{
					currentState = nodeState;
					return nodeState;
				}
				}
			}
			return Success;
		}
	};
}