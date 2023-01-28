#pragma once
#include "BehaviourNode.h"

enum DecoratorType {
	Inverter
};

class BehaviourDecorator : public BehaviourNode {
public:
	BehaviourDecorator(const std::string& nodeName, DecoratorType type) : BehaviourNode(nodeName) { this->type = type; }
	~BehaviourDecorator() { delete child;  }

	void AddChild(BehaviourNode* n) { child = n; }

	BehaviourState Execute(float dt) override {
		switch (type) {
		case Inverter:
			return RunInvert();
		}
	}
protected:
	BehaviourState RunInvert() {
		switch (child->currentState) {
		case Success:
			return Failure;
		case Failure:
			return Success;
		}
	}

	DecoratorType type;
	BehaviourNode* child;
};