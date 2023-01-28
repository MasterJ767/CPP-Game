#include "PhysicsObject.h"
#include "PhysicsSystem.h"
#include "Transform.h"
#include "Vector3.h"
#include <math.h>

using namespace NCL;
using namespace CSC8503;

PhysicsObject::PhysicsObject(Transform* parentTransform, const CollisionVolume* parentVolume)	{
	transform	= parentTransform;
	volume		= parentVolume;

	inverseMass = 1.0f;
	elasticity	= 0.8f;
	friction	= 0.8f;
}

PhysicsObject::~PhysicsObject()	{

}

void PhysicsObject::ApplyAngularImpulse(const Vector3& force) {
	angularVelocity += inverseInteriaTensor * force;
}

void PhysicsObject::ApplyLinearImpulse(const Vector3& force) {
	linearVelocity += force * inverseMass;
}

void PhysicsObject::AddForce(const Vector3& addedForce) {
	force += addedForce;
}

void PhysicsObject::AddForceAtPosition(const Vector3& addedForce, const Vector3& position) {
	Vector3 localPos = position - transform->GetPosition();

	force  += addedForce;
	torque += Vector3::Cross(localPos, addedForce);
}

void PhysicsObject::AddTorque(const Vector3& addedTorque) {
	torque += addedTorque;
}

void PhysicsObject::ClearForces() {
	force				= Vector3();
	torque				= Vector3();
}

void PhysicsObject::InitCubeInertia(bool isHollow) {
	Vector3 dimensions	= transform->GetScale();

	Vector3 fullWidth   = dimensions * 2;

	Vector3 dimsSqr	    = fullWidth * fullWidth;

	inverseInertia.x = isHollow ? (5.0f * inverseMass) / (3.0f * (dimsSqr.y + dimsSqr.z)) : (12.0f * inverseMass) / (dimsSqr.y + dimsSqr.z);
	inverseInertia.y = isHollow ? (5.0f * inverseMass) / (3.0f * (dimsSqr.x + dimsSqr.z)) : (12.0f * inverseMass) / (dimsSqr.x + dimsSqr.z);
	inverseInertia.z = isHollow ? (5.0f * inverseMass) / (3.0f * (dimsSqr.x + dimsSqr.y)) : (12.0f * inverseMass) / (dimsSqr.x + dimsSqr.y);
}

void PhysicsObject::InitSphereInertia(bool isHollow) {
	float radius	= transform->GetScale().GetMaxElement();
	float i			= isHollow ? 1.5f * inverseMass / (radius*radius) : 2.5f * inverseMass / (radius*radius);

	inverseInertia	= Vector3(i, i, i);
}

void PhysicsObject::InitCapsuleInertia(bool isHollow) {
	float radius = transform->GetScale().x;
	float rr = radius * radius;
	float height = transform->GetScale().y;
	float pi = 3.14159265358979323846;

	float m_cy = height * rr * pi;
	float m_hs = (2.0f * (rr * radius) * pi) / 3.0f;

	float cy_co_y = 2.0f / rr;
	float hs_co_y = 5.0f / (2.0f * rr);
	float cy_co_xz = (12.0f / (height * height)) + (4.0f / rr);
	float hs_co_xz = hs_co_y + (2.0f / (height * height)) + (8.0f / (3.0f * height * radius));

	inverseInertia.x = ((cy_co_xz * inverseMass) / m_cy) + ((hs_co_xz * inverseMass) / (2.0f * m_hs));
	inverseInertia.y = ((cy_co_y * inverseMass) / m_cy) + ((hs_co_y * inverseMass) / (2.0f * m_hs));
	inverseInertia.z = ((cy_co_xz * inverseMass) / m_cy) + ((hs_co_xz * inverseMass) / (2.0f * m_hs));
}

void PhysicsObject::InitConeInertia() {
	float radius = transform->GetScale().x;
	float height = transform->GetScale().y;

	inverseInertia.x = (20.0f * inverseMass) / (3.0f*radius*radius) + (10.0f * inverseMass) / (height*height);
	inverseInertia.y = (10.0f * inverseMass) / (3.0f*radius*radius);
	inverseInertia.z = (20.0f * inverseMass) / (3.0f*radius*radius) + (10.0f * inverseMass) / (height*height);
}

void PhysicsObject::UpdateInertiaTensor() {
	Quaternion q = transform->GetOrientation();
	
	Matrix3 invOrientation	= Matrix3(q.Conjugate());
	Matrix3 orientation		= Matrix3(q);

	inverseInteriaTensor = orientation * Matrix3::Scale(inverseInertia) *invOrientation;
}