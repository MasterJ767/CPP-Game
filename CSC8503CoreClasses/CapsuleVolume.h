#pragma once
#include "CollisionVolume.h"
#include "Debug.h"

namespace NCL {
    class CapsuleVolume : public CollisionVolume {
    public:
        CapsuleVolume(float halfHeight, float radius, bool isTrigger = false) : CollisionVolume(isTrigger) {
            this->halfHeight    = halfHeight;
            this->radius        = radius;
            this->type          = VolumeType::Capsule;
        };
        ~CapsuleVolume() {

        }
        float GetRadius() const {
            return radius;
        }

        float GetHalfHeight() const {
            return halfHeight;
        }

        /* void Debug(Transform& transform, float dt) const {
            Matrix3 matrix = Matrix3(transform.GetOrientation());

            Debug::DrawLine(transform.GetPosition() - matrix * (Vector3(0, 1, 0) * halfHeight), transform.GetPosition() + matrix * (Vector3(0, 1, 0) * halfHeight), Vector4(0, 1, 0, 1), dt);
            Debug::DrawLine(transform.GetPosition() - matrix * (Vector3(0, 1, 0) * halfHeight), transform.GetPosition() - matrix * (Vector3(0, 1, 0) * halfHeight - Vector3(1, 0, 0) * radius), Vector4(0, 1, 0, 1), dt);
            Debug::DrawLine(transform.GetPosition() + matrix * (Vector3(0, 1, 0) * halfHeight), transform.GetPosition() + matrix * (Vector3(0, 1, 0) * halfHeight + Vector3(1, 0, 0) * radius), Vector4(0, 1, 0, 1), dt);
        }*/

    protected:
        float radius;
        float halfHeight;
    };
}

