#pragma once
#include "Transform.h"
#include "Layer.h"
#include "Vector3.h"

#include <string>
#include <vector>

using std::vector;
using namespace NCL::Maths;

namespace NCL {
	class CollisionVolume;
}

namespace NCL::CSC8503 {
	class NetworkObject;
	class RenderObject;
	class PhysicsObject;

	class GameObject	{
	public:
		GameObject(std::string name = "");
		~GameObject();

		void SetBoundingVolume(CollisionVolume* vol) {
			boundingVolume = vol;
		}

		const CollisionVolume* GetBoundingVolume() const {
			return boundingVolume;
		}

		bool IsActive() const {
			return isActive;
		}

		Transform& GetTransform() {
			return transform;
		}

		RenderObject* GetRenderObject() const {
			return renderObject;
		}

		PhysicsObject* GetPhysicsObject() const {
			return physicsObject;
		}

		NetworkObject* GetNetworkObject() const {
			return networkObject;
		}

		void SetActive(bool state) {
			isActive = state;
		}

		void SetRenderObject(RenderObject* newObject) {
			renderObject = newObject;
		}

		void SetPhysicsObject(PhysicsObject* newObject) {
			physicsObject = newObject;
		}

		void SetNetworkObject(NetworkObject* newObject) {
			networkObject = newObject;
		}

		void SetCollisionLayer(Layer layer) {
			collisionLayer = layer;
		}

		Layer GetCollisionLayer() {
			return collisionLayer;
		}

		const std::string& GetName() const {
			return name;
		}

		virtual void UpdateObject(const float dt) {}

		virtual void OnCollisionBegin(GameObject* otherObject) {
			//std::cout << "OnCollisionBegin event occured!\n";
		}

		virtual void OnCollisionEnd(GameObject* otherObject) {
			//std::cout << "OnCollisionEnd event occured!\n";
		}

		bool GetBroadphaseAABB(Vector3&outsize) const;

		void UpdateBroadphaseAABB();

		void SetWorldID(int newID) {
			worldID = newID;
		}

		int		GetWorldID() const {
			return worldID;
		}

	protected:
		Transform			transform;

		CollisionVolume*	boundingVolume;
		PhysicsObject*      physicsObject;
		RenderObject*		renderObject;
		NetworkObject*		networkObject;

		bool		isActive;
		int			worldID;
		Layer         collisionLayer;
		std::string	name;

		Vector3 broadphaseAABB;
	};
}

