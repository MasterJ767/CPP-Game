#include "GameObject.h"
#include "CollisionVolume.h"
#include "AABBVolume.h"
#include "OBBVolume.h"
#include "SphereVolume.h"
#include "CapsuleVolume.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "NetworkObject.h"
#include "Debug.h"

using namespace NCL::CSC8503;

GameObject::GameObject(string objectName)	{
	name			= objectName;
	worldID			= -1;
	collisionLayer  = DEFAULT;
	isActive		= true;
	boundingVolume	= nullptr;
	physicsObject	= nullptr;
	renderObject	= nullptr;
	networkObject	= nullptr;
}

GameObject::~GameObject()	{
	delete boundingVolume;
	delete physicsObject;
	delete renderObject;
	delete networkObject;
}

bool GameObject::GetBroadphaseAABB(Vector3&outSize) const {
	if (!boundingVolume) {
		return false;
	}
	outSize = broadphaseAABB;
	return true;
}

void GameObject::UpdateBroadphaseAABB() {
	if (!boundingVolume) {
		return;
	}
	if (boundingVolume->type == VolumeType::AABB) {
		broadphaseAABB = ((AABBVolume&)*boundingVolume).GetHalfDimensions();
	}
	else if (boundingVolume->type == VolumeType::Sphere) {
		float r = ((SphereVolume&)*boundingVolume).GetRadius();
		//Debug::DrawLine(transform.GetPosition() + Vector3(-r, 0, 0), transform.GetPosition() + Vector3(r, 0, 0), Vector4(0, 1, 1, 1), 0.05f);
		//Debug::DrawLine(transform.GetPosition() + Vector3(0, 0, -r), transform.GetPosition() + Vector3(0, 0, r), Vector4(0, 1, 1, 1), 0.05f);
		//Debug::DrawLine(transform.GetPosition() + Vector3(0, -r, 0), transform.GetPosition() + Vector3(0, r, 0), Vector4(0, 1, 1, 1), 0.05f);
		broadphaseAABB = Vector3(r, r, r);
	}
	else if (boundingVolume->type == VolumeType::OBB) {
		Matrix3 mat = Matrix3(transform.GetOrientation());
		mat = mat.Absolute();
		Vector3 halfSizes = ((OBBVolume&)*boundingVolume).GetHalfDimensions();
		broadphaseAABB = mat * halfSizes;
	}
	else if (boundingVolume->type == VolumeType::Capsule) {
		Matrix3 mat = Matrix3(transform.GetOrientation());
		mat = mat.Absolute();
		float r = ((SphereVolume&)*boundingVolume).GetRadius();
		Vector3 tip = transform.GetPosition() + (mat * Vector3(0, ((CapsuleVolume&)*boundingVolume).GetHalfHeight(), 0));
		Vector3 base = transform.GetPosition() - (mat * Vector3(0, ((CapsuleVolume&)*boundingVolume).GetHalfHeight(), 0));
		Vector3 tipC = transform.GetPosition() + (mat * Vector3(0, ((CapsuleVolume&)*boundingVolume).GetHalfHeight() - r / 2, 0));
		Vector3 baseC = transform.GetPosition() - (mat * Vector3(0, ((CapsuleVolume&)*boundingVolume).GetHalfHeight() - r / 2, 0));
		float x = abs(tipC.x - baseC.x) / 2 + r / 2;
		float y = abs(tipC.y - baseC.y) / 2 + r / 2;
		float z = abs(tipC.z - baseC.z) / 2 + r / 2;	
		//Debug::DrawLine(transform.GetPosition() + Vector3(x, y, z), transform.GetPosition() + Vector3(-x, y, z), Vector4(0, 1, 0, 1), 0.05f);
		//Debug::DrawLine(transform.GetPosition() + Vector3(x, y, z), transform.GetPosition() + Vector3(x, -y, z), Vector4(0, 1, 0, 1), 0.05f);
		//Debug::DrawLine(transform.GetPosition() + Vector3(x, y, z), transform.GetPosition() + Vector3(x, y, -z), Vector4(0, 1, 0, 1), 0.05f);
		//Debug::DrawLine(transform.GetPosition() + Vector3(-x, -y, -z), transform.GetPosition() + Vector3(-x, -y, z), Vector4(0, 1, 0, 1), 0.05f);
		//Debug::DrawLine(transform.GetPosition() + Vector3(-x, -y, -z), transform.GetPosition() + Vector3(x, -y, -z), Vector4(0, 1, 0, 1), 0.05f);
		//Debug::DrawLine(transform.GetPosition() + Vector3(-x, -y, -z), transform.GetPosition() + Vector3(-x, y, -z), Vector4(0, 1, 0, 1), 0.05f);
		//Debug::DrawLine(transform.GetPosition() + Vector3(-x, 0, 0), transform.GetPosition() + Vector3(x, 0, 0), Vector4(0, 1, 1, 1), 0.05f);
		//Debug::DrawLine(transform.GetPosition() + Vector3(0, 0, -z), transform.GetPosition() + Vector3(0, 0, z), Vector4(0, 1, 1, 1), 0.05f);
		broadphaseAABB = Vector3(x, y, z);
	}
}