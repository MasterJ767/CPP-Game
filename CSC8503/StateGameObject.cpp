#include "StateGameObject.h"
#include "StateTransition.h"
#include "StateMachine.h"
#include "State.h"
#include "PhysicsObject.h"

using namespace NCL;
using namespace CSC8503;

StateGameObject::StateGameObject() {
	counter = 0.0f;
	stateMachine = new StateMachine();

	State* stateA = new State([&](float dt)->void
		{
			this->RotateClockwise(dt);
		}
	);

	State* stateB = new State([&](float dt)->void
		{
			this->RotateAnticlockwise(dt);
		}
	);

	stateMachine->AddState(stateA);
	stateMachine->AddState(stateB);

	stateMachine->AddTransition(new StateTransition(stateA, stateB, [&]()-> bool
		{
			return this->counter > 12.0f;
		}
	));

	stateMachine->AddTransition(new StateTransition(stateB, stateA, [&]()-> bool
		{
			return this->counter < 0.0f;
		}
	));
}

StateGameObject::~StateGameObject() {
	delete stateMachine;
}

void StateGameObject::Update(float dt) {
	stateMachine->Update(dt);
}

void StateGameObject::MoveLeft(float dt) {
	GetPhysicsObject()->AddForce({ -100, 0, 0 });
	counter += dt;
}

void StateGameObject::MoveRight(float dt) {
	GetPhysicsObject()->AddForce({ 100, 0, 0 });
	counter -= dt;
}

void StateGameObject::MoveForward(float dt) {
	GetPhysicsObject()->AddForce({ 0, 0, -100 });
	counter += dt;
}

void StateGameObject::MoveBackward(float dt) {
	GetPhysicsObject()->AddForce({ 0, 0, 100 });
	counter -= dt;
}

void StateGameObject::RotateClockwise(float dt) {
	GetPhysicsObject()->AddTorque({0, -1000, 0});
	counter += dt;
}

void StateGameObject::RotateAnticlockwise(float dt) {
	GetPhysicsObject()->AddTorque({0, 1000, 0});
	counter -= dt;
}