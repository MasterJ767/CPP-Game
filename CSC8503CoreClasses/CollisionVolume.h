#pragma once
#include "Transform.h"

using namespace NCL::CSC8503;

namespace NCL {
	enum class VolumeType {
		AABB     = 1,
		OBB      = 2,
		Sphere   = 4,
		Mesh     = 8,
		Capsule  = 16,
		Compound = 32,
		Invalid  = 256
	};

	class CollisionVolume
	{
	public:
		CollisionVolume(bool isTrigger = false) {
			this->isTrigger = isTrigger;
			type = VolumeType::Invalid;
		}
		~CollisionVolume() {}

		void virtual Debug(Transform& transform, float dt) const {}

		VolumeType type;
		bool isTrigger;
	};
}