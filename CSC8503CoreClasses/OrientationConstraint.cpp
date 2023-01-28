#include "OrientationConstraint.h"
#include "GameObject.h"
#include "PhysicsObject.h"
using namespace NCL;
using namespace Maths;
using namespace CSC8503;

OrientationConstraint::OrientationConstraint(GameObject* a, GameObject* b)
{
	objectA = a;
	objectB = b;
}

OrientationConstraint::~OrientationConstraint()
{

}

void OrientationConstraint::UpdateConstraint(float dt) {
	Quaternion rotA = objectA->GetTransform().GetOrientation();
	Quaternion rotB = objectB->GetTransform().GetOrientation();

	Quaternion rotAtoB = rotA.Conjugate() * rotB;
	Quaternion rotBtoA = rotA * rotB.Conjugate();

	if (rotAtoB != Quaternion() || rotBtoA != Quaternion()) {
		PhysicsObject* physA = objectA->GetPhysicsObject();
		PhysicsObject* physB = objectB->GetPhysicsObject();

		float constraintMass = physA->GetInverseMass() + physB->GetInverseMass();

		if (constraintMass > 0.0f) {
			Vector3 angularVelocityA = ((rotB * 2) * (rotA * dt).Conjugate()).ToEuler();
			Vector3 angularVelocityB = ((rotA * 2) * (rotB * dt).Conjugate()).ToEuler();

			physA->ApplyAngularImpulse((angularVelocityA / dt) / constraintMass);
			physB->ApplyAngularImpulse((angularVelocityB / dt) / constraintMass);
		}
	}
}