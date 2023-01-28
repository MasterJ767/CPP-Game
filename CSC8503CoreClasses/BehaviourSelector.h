#pragma once
#include "BehaviourNodeWithChildren.h"

namespace NCL::CSC8503 {
	class BehaviourSelector : public BehaviourNodeWithChildren {
	public:
		BehaviourSelector(const std::string& nodeName) : BehaviourNodeWithChildren(nodeName) {}
		~BehaviourSelector() {}
		BehaviourState Execute(float dt, GameObject* obj) override {
			//std::cout << "Executing selector " << name << "\n";
			for (auto& i : childNodes) {
				BehaviourState nodeState = i->Execute(dt, obj);
				//std::cout << i->name << ": " << nodeState << std::endl;
				switch (nodeState) {
				case Failure: continue;
				case Success:
				case Ongoing:
				{
					currentState = nodeState;
					return currentState;
				}
				}
			}
			return Failure;
		}
	};
}