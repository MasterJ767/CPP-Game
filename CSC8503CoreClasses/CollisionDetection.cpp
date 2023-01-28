#include "CollisionDetection.h"
#include "CollisionVolume.h"
#include "AABBVolume.h"
#include "OBBVolume.h"
#include "SphereVolume.h"
#include "Window.h"
#include "Maths.h"
#include "Transform.h"
#include "Debug.h"

using namespace NCL;

bool CollisionDetection::RayPlaneIntersection(const Ray&r, const Plane&p, RayCollision& collisions) {
	float ln = Vector3::Dot(p.GetNormal(), r.GetDirection());

	if (ln == 0.0f) {
		return false; //direction vectors are perpendicular!
	}
	
	Vector3 planePoint = p.GetPointOnPlane();

	Vector3 pointDir = planePoint - r.GetPosition();

	float d = Vector3::Dot(pointDir, p.GetNormal()) / ln;

	collisions.collidedAt = r.GetPosition() + (r.GetDirection() * d);

	return true;
}

bool CollisionDetection::RayIntersection(const Ray& r,GameObject& object, RayCollision& collision) {
	bool hasCollided = false;

	const Transform& worldTransform = object.GetTransform();
	const CollisionVolume* volume	= object.GetBoundingVolume();

	if (!volume) {
		return false;
	}

	switch (volume->type) {
		case VolumeType::AABB:		
			hasCollided = RayAABBIntersection(r, worldTransform, (const AABBVolume&)*volume	, collision); break;
		case VolumeType::OBB:		
			hasCollided = RayOBBIntersection(r, worldTransform, (const OBBVolume&)*volume	, collision); break;
		case VolumeType::Sphere:	
			hasCollided = RaySphereIntersection(r, worldTransform, (const SphereVolume&)*volume	, collision); break;
		case VolumeType::Capsule:	
			hasCollided = RayCapsuleIntersection(r, worldTransform, (const CapsuleVolume&)*volume, collision); break;
	}

	return hasCollided;
}

bool CollisionDetection::RayBoxIntersection(const Ray&r, const Vector3& boxPos, const Vector3& boxSize, RayCollision& collision) {
	Vector3 boxMin = boxPos - boxSize;
	Vector3 boxMax = boxPos + boxSize;

	Vector3 rayPos = r.GetPosition();
	Vector3 rayDir = r.GetDirection();

	Vector3 tVals(-1, -1, -1);

	for (int i = 0; i < 3; ++i) {
		if (rayDir[i] > 0) { tVals[i] = (boxMin[i] - rayPos[i]) / rayDir[i]; }
		else if (rayDir[i] < 0) { tVals[i] = (boxMax[i] - rayPos[i]) / rayDir[i]; }
	}
	float bestT = tVals.GetMaxElement();
	if (bestT < 0.0f) { return false; }
	
	Vector3 intersection = rayPos + (rayDir * bestT);
	const float epsilon = 0.0001f;
	for (int i = 0; i < 3; ++i) {
		if (intersection[i] + epsilon < boxMin[i] || intersection[i] - epsilon > boxMax[i]) { return false; }
	}

	collision.collidedAt = intersection;
	collision.rayDistance = bestT;
	return true;
}

bool CollisionDetection::RayAABBIntersection(const Ray&r, const Transform& worldTransform, const AABBVolume& volume, RayCollision& collision) {
	Vector3 boxPos = worldTransform.GetPosition();
	Vector3 boxSize = volume.GetHalfDimensions();
	return RayBoxIntersection(r, boxPos, boxSize, collision);
}

bool CollisionDetection::RayOBBIntersection(const Ray&r, const Transform& worldTransform, const OBBVolume& volume, RayCollision& collision) {
	Quaternion orientation = worldTransform.GetOrientation();
	Vector3 position = worldTransform.GetPosition();

	Matrix3 transform = Matrix3(orientation);
	Matrix3 invTransform = Matrix3(orientation.Conjugate());

	Vector3 localRayPos = r.GetPosition() - position;

	Ray tempRay(invTransform * localRayPos, invTransform * r.GetDirection());

	bool collided = RayBoxIntersection(tempRay, Vector3(), volume.GetHalfDimensions(), collision);

	if (collided) { collision.collidedAt = transform * collision.collidedAt + position; }

	return collided;
}

bool CollisionDetection::RaySphereIntersection(const Ray&r, const Transform& worldTransform, const SphereVolume& volume, RayCollision& collision) {
	Vector3 spherePos = worldTransform.GetPosition();
	float sphereRadius = volume.GetRadius();
	Vector3 dir = (spherePos - r.GetPosition());
	float sphereProj = Vector3::Dot(dir, r.GetDirection());

	if (sphereProj < 0.0f) { return false; }

	Vector3 point = r.GetPosition() + (r.GetDirection() * sphereProj);
	float sphereDist = (point - spherePos).Length();

	if (sphereDist > sphereRadius) { return false; }

	float offset = sqrt((sphereRadius * sphereRadius) - (sphereDist * sphereDist));
	collision.rayDistance = sphereProj - (offset);
	collision.collidedAt = r.GetPosition() + (r.GetDirection() * collision.rayDistance);
	return true;
}

bool CollisionDetection::RayCapsuleIntersection(const Ray& r, const Transform& worldTransform, const CapsuleVolume& volume, RayCollision& collision) {
	Quaternion orientation = worldTransform.GetOrientation();
	Vector3 capsulePos = worldTransform.GetPosition();

	Matrix3 transform = Matrix3(orientation);

	float ra = volume.GetRadius() / 2;

	Vector3 pb = capsulePos + (transform * Vector3(0, volume.GetHalfHeight() - ra, 0));
	Vector3 pa = capsulePos - (transform * Vector3(0, volume.GetHalfHeight() - ra, 0));
	Vector3 ro = r.GetPosition();
	Vector3 rd = r.GetDirection();
	
	Vector3 ba = pb - pa;
	Vector3 oa = ro - pa;
	float baba = Vector3::Dot(ba, ba);
	float bard = Vector3::Dot(ba, rd);
	float baoa = Vector3::Dot(ba, oa);
	float rdoa = Vector3::Dot(rd, oa);
	float oaoa = Vector3::Dot(oa, oa);
	float a = baba - bard * bard;
	float b = baba * rdoa - baoa * bard;
	float c = baba * oaoa - baoa * baoa - ra * ra * baba;
	float h = b * b - a * c;
	if (h >= 0.0) {
		float t = (-b - sqrt(h)) / a;
		float y = baoa + t * bard;
		if (y > 0.0 && y < baba) {
			collision.rayDistance = t;
			collision.collidedAt = r.GetPosition() + (r.GetDirection() * collision.rayDistance);
			return true;
		}
		Vector3 oc = (y <= 0.0) ? oa : ro - pb;
		b = Vector3::Dot(rd, oc);
		c = Vector3::Dot(oc, oc) - ra * ra;
		h = b * b - c;
		if (h > 0.0) {
			collision.rayDistance = -b - sqrt(h);
			collision.collidedAt = r.GetPosition() + (r.GetDirection() * collision.rayDistance);
			return true;
		}
	}
	return false;
}

bool CollisionDetection::ObjectIntersection(GameObject* a, GameObject* b, CollisionInfo& collisionInfo) {
	if (a->GetCollisionLayer() == DEFAULT || b->GetCollisionLayer() == DEFAULT) { return false; }

	if (a->GetCollisionLayer() == ENVIRONMENT && b->GetCollisionLayer() == ENVIRONMENT) { return false; }
	if (a->GetCollisionLayer() == ENVIRONMENT && b->GetCollisionLayer() == BUTTON) { return false; }
	if (a->GetCollisionLayer() == BUTTON && b->GetCollisionLayer() == ENVIRONMENT) { return false; }

	const CollisionVolume* volA = a->GetBoundingVolume();
	const CollisionVolume* volB = b->GetBoundingVolume();

	if (!volA || !volB) {
		return false;
	}

	collisionInfo.a = a;
	collisionInfo.b = b;

	Transform& transformA = a->GetTransform();
	Transform& transformB = b->GetTransform();

	VolumeType pairType = (VolumeType)((int)volA->type | (int)volB->type);

	//Two AABBs
	if (pairType == VolumeType::AABB) {
		return AABBIntersection((AABBVolume&)*volA, transformA, (AABBVolume&)*volB, transformB, collisionInfo);
	}
	//Two Spheres
	if (pairType == VolumeType::Sphere) {
		return SphereIntersection((SphereVolume&)*volA, transformA, (SphereVolume&)*volB, transformB, collisionInfo);
	}
	//Two OBBs
	if (pairType == VolumeType::OBB) {
		return OBBIntersection((OBBVolume&)*volA, transformA, (OBBVolume&)*volB, transformB, collisionInfo);
	}
	//Two Capsules

	//AABB vs Sphere pairs
	if (volA->type == VolumeType::AABB && volB->type == VolumeType::Sphere) {
		return AABBSphereIntersection((AABBVolume&)*volA, transformA, (SphereVolume&)*volB, transformB, collisionInfo);
	}
	if (volA->type == VolumeType::Sphere && volB->type == VolumeType::AABB) {
		collisionInfo.a = b;
		collisionInfo.b = a;
		return AABBSphereIntersection((AABBVolume&)*volB, transformB, (SphereVolume&)*volA, transformA, collisionInfo);
	}

	//OBB vs sphere pairs
	if (volA->type == VolumeType::OBB && volB->type == VolumeType::Sphere) {
		return OBBSphereIntersection((OBBVolume&)*volA, transformA, (SphereVolume&)*volB, transformB, collisionInfo);
	}
	if (volA->type == VolumeType::Sphere && volB->type == VolumeType::OBB) {
		collisionInfo.a = b;
		collisionInfo.b = a;
		return OBBSphereIntersection((OBBVolume&)*volB, transformB, (SphereVolume&)*volA, transformA, collisionInfo);
	}

	//Capsule vs other interactions
	if (volA->type == VolumeType::Capsule && volB->type == VolumeType::Capsule) {
		return CapsuleIntersection((CapsuleVolume&)*volA, transformA, (CapsuleVolume&)*volB, transformB, collisionInfo);
	}

	if (volA->type == VolumeType::Capsule && volB->type == VolumeType::Sphere) {
		return SphereCapsuleIntersection((CapsuleVolume&)*volA, transformA, (SphereVolume&)*volB, transformB, collisionInfo);
	}
	if (volA->type == VolumeType::Sphere && volB->type == VolumeType::Capsule) {
		collisionInfo.a = b;
		collisionInfo.b = a;
		return SphereCapsuleIntersection((CapsuleVolume&)*volB, transformB, (SphereVolume&)*volA, transformA, collisionInfo);
	}

	if (volB->type == VolumeType::Capsule && volA->type == VolumeType::AABB) {
		return AABBCapsuleIntersection((AABBVolume&)*volA, transformA, (CapsuleVolume&)*volB, transformB, collisionInfo);
	}
	if (volA->type == VolumeType::Capsule && volB->type == VolumeType::AABB) {
		collisionInfo.a = b;
		collisionInfo.b = a;
		return AABBCapsuleIntersection((AABBVolume&)*volB, transformB, (CapsuleVolume&)*volA, transformA, collisionInfo);
	}

	return false;
}

bool CollisionDetection::AABBTest(const Vector3& posA, const Vector3& posB, const Vector3& halfSizeA, const Vector3& halfSizeB) {
	Vector3 delta = posB - posA;
	Vector3 totalSize = halfSizeA + halfSizeB;

	if (abs(delta.x) < totalSize.x &&
		abs(delta.y) < totalSize.y &&
		abs(delta.z) < totalSize.z) {
		return true;
	}
	return false;
}

//AABB/AABB Collisions
bool CollisionDetection::AABBIntersection(const AABBVolume& volumeA, const Transform& worldTransformA, const AABBVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {
	Vector3 boxAPos = worldTransformA.GetPosition();
	Vector3 boxBPos = worldTransformB.GetPosition();

	Vector3 boxASize = volumeA.GetHalfDimensions();
	Vector3 boxBSize = volumeB.GetHalfDimensions();

	bool overlap = AABBTest(boxAPos, boxBPos, boxASize, boxBSize);
	if (overlap) {
		static const Vector3 faces[6] = {
			Vector3(-1, 0, 0), Vector3(1, 0, 0),
			Vector3(0, -1, 0), Vector3(0, 1, 0),
			Vector3(0, 0, -1), Vector3(0, 0, 1)
		};

		Vector3 maxA = boxAPos + boxASize;
		Vector3 minA = boxAPos - boxASize;

		Vector3 maxB = boxBPos + boxBSize;
		Vector3 minB = boxBPos - boxBSize;

		float distances[6] = {
			(maxB.x - minA.x),
			(maxA.x - minB.x),
			(maxB.y - minA.y),
			(maxA.y - minB.y),
			(maxB.z - minA.z),
			(maxA.z - minB.z)
		};

		float penetration = FLT_MAX;
		Vector3 bestAxis;
		for (int i = 0; i < 6; ++i) {
			if (distances[i] < penetration) {
				penetration = distances[i];
				bestAxis = faces[i];
			}
		}
		collisionInfo.AddContactPoint(Vector3(), Vector3(), bestAxis, penetration);
		return true;
	}
	return false;
}

//Sphere / Sphere Collision
bool CollisionDetection::SphereIntersection(const SphereVolume& volumeA, const Transform& worldTransformA, const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {
	float radii = volumeA.GetRadius() + volumeB.GetRadius();
	Vector3 delta = worldTransformB.GetPosition() - worldTransformA.GetPosition();

	float deltaLength = delta.Length();

	if (deltaLength < radii) {
		float penetration = (radii - deltaLength);
		Vector3 normal = delta.Normalised();
		Vector3 localA = normal * volumeA.GetRadius();
		Vector3 localB = -normal * volumeB.GetRadius();

		collisionInfo.AddContactPoint(localA, localB, normal, penetration);
		return true;
	}
	return false;
}

//AABB - Sphere Collision
bool CollisionDetection::AABBSphereIntersection(const AABBVolume& volumeA, const Transform& worldTransformA, const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {
	Vector3 boxSize = volumeA.GetHalfDimensions();
	Vector3 delta = worldTransformB.GetPosition() - worldTransformA.GetPosition();
	Vector3 closestPointOnBox = Maths::Clamp(delta, -boxSize, boxSize);

	Vector3 localPoint = delta - closestPointOnBox;
	float distance = localPoint.Length();

	if (distance < volumeB.GetRadius()) {
		Vector3 collisionNormal = localPoint.Normalised();
		float penetration = (volumeB.GetRadius() - distance);

		Vector3 localA = Vector3();
		Vector3 localB = -collisionNormal * volumeB.GetRadius();

		collisionInfo.AddContactPoint(localA, localB, collisionNormal, penetration);
		return true;
	}
	return false;
}

bool  CollisionDetection::OBBSphereIntersection(const OBBVolume& volumeA, const Transform& worldTransformA, const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {
	Quaternion orientation = worldTransformA.GetOrientation();

	Matrix3 transform = Matrix3(orientation);
	Matrix3 invTransform = Matrix3(orientation.Conjugate());

	return false;
}

bool CollisionDetection::CapsuleIntersection(const CapsuleVolume& volumeA, const Transform& worldTransformA, const CapsuleVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {
	Quaternion orientationA = worldTransformA.GetOrientation();
	Vector3 capsulePosA = worldTransformA.GetPosition();
	Matrix3 transformA = Matrix3(orientationA);
	Vector3 q1 = capsulePosA + (transformA * Vector3(0, volumeA.GetHalfHeight() - volumeA.GetRadius() / 2, 0));
	Vector3 p1 = capsulePosA - (transformA * Vector3(0, volumeA.GetHalfHeight() - volumeA.GetRadius() / 2, 0));

	Quaternion orientationB = worldTransformB.GetOrientation();
	Vector3 capsulePosB = worldTransformB.GetPosition();
	Matrix3 transformB = Matrix3(orientationB);
	Vector3 q2 = capsulePosB + (transformB * Vector3(0, volumeB.GetHalfHeight() - volumeB.GetRadius() / 2, 0));
	Vector3 p2 = capsulePosB - (transformB * Vector3(0, volumeB.GetHalfHeight() - volumeB.GetRadius() / 2, 0));

	Vector3 d1 = q1 - p1;
	Vector3 d2 = q2 - p2;
	Vector3 r = p1 - p2;
	float a = Vector3::Dot(d1, d1);
	float e = Vector3::Dot(d2, d2);
	float f = Vector3::Dot(d2, r);
	const float epsilon = 0.0001f;

	float s;
	float t;
	Vector3 c1;
	Vector3 c2;
	if (a <= epsilon && e <= epsilon) {
		s = 0.0f;
		t = 0.0f;
		c1 = p1;
		c2 = p2;
	}
	else {
		if (a <= epsilon) {
			s = 0.0f;
			t = f / e;
			t = std::clamp(t, 0.0f, 1.0f);
		}
		else {
			float c = Vector3::Dot(d1, r);
			if (e <= epsilon) {
				t = 0.0f;
				s = std::clamp(-c / a, 0.0f, 1.0f);
			}
			else {
				float b = Vector3::Dot(d1, d2);
				float denom = a * e - b * b;
				if (denom != 0.0f) {
					s = std::clamp((b * f - c * e) / denom, 0.0f, 1.0f);
				}
				else { s = 0.0f; }
				t = (b * s + f) / e;
				if (t < 0.0f) {
					t = 0.0f;
					s = std::clamp(-c / a, 0.0f, 1.0f);
				}
				else if (t > 1.0f) {
					t = 1.0f;
					s = std::clamp((b - c) / a, 0.0f, 1.0f);
				}
			}
		}
		c1 = p1 + d1 * s;
		c2 = p2 + d2 * t;
	}
	float dist2 = Vector3::Dot(c1 - c2, c1 - c2);
	float radii = volumeA.GetRadius() / 2 + volumeB.GetRadius() / 2;
	if (dist2 <= radii * radii) {
		Vector3 delta = c2 - c1;

		float penetration = (radii - delta.Length());
		Vector3 normal = delta.Normalised();
		Vector3 localA = normal * volumeA.GetRadius() / 2;
		Vector3 localB = -normal * volumeB.GetRadius() / 2;

		collisionInfo.AddContactPoint(localA, localB, normal, penetration);
		return true;
	}
	return false;
}

bool CollisionDetection::AABBCapsuleIntersection(const AABBVolume& volumeA, const Transform& worldTransformA, const CapsuleVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {
	Quaternion orientationB = worldTransformB.GetOrientation();
	Vector3 capsulePosB = worldTransformB.GetPosition();
	Matrix3 transformB = Matrix3(orientationB);
	Vector3 tip = capsulePosB + (transformB * Vector3(0, volumeB.GetHalfHeight() - volumeB.GetRadius() / 2, 0));
	Vector3 base = capsulePosB - (transformB * Vector3(0, volumeB.GetHalfHeight() - volumeB.GetRadius() / 2, 0));
	Vector3 capsuleDir = tip - base;
	float radius = volumeB.GetRadius() / 2;

	Vector3 boxMin = worldTransformA.GetPosition() - volumeA.GetHalfDimensions() - Vector3(radius, radius, radius);
	Vector3 boxMax = worldTransformA.GetPosition() + volumeA.GetHalfDimensions() + Vector3(radius, radius, radius);

	Vector3 tVals(-1, -1, -1);
	Vector3 baseLocal = base - worldTransformA.GetPosition();

	for (int i = 0; i < 3; ++i) {
		if (capsuleDir[i] > 0) { tVals[i] = (boxMin[i] - baseLocal[i]) / capsuleDir[i]; }
		else if (capsuleDir[i] < 0) { tVals[i] = (boxMax[i] - baseLocal[i]) / capsuleDir[i]; }
	}

	float bestT = tVals.GetMaxElement();

	Vector3 d;
	if (bestT <= 0.0f) { bestT = 0.0f;  d = base; }
	else if (bestT >= 1.0f) { bestT = 1.0f; d = tip; }
	else { d = base + capsuleDir * bestT; }

	Vector3 boxSize = volumeA.GetHalfDimensions();
	Vector3 delta = d - worldTransformA.GetPosition();
	Vector3 closestPointOnBox = Maths::Clamp(delta, -boxSize, boxSize);

	Vector3 localPoint = delta - closestPointOnBox;
	float distance = localPoint.Length();

	if (distance < radius) {
		Vector3 collisionNormal = localPoint.Normalised();
		float penetration = (radius - distance);

		Vector3 localA = Vector3();
		Vector3 localB = -collisionNormal * radius;

		collisionInfo.AddContactPoint(localA, localB, collisionNormal, penetration);
		return true;
	}
	
	return false;
}

bool CollisionDetection::SphereCapsuleIntersection(const CapsuleVolume& volumeA, const Transform& worldTransformA, const SphereVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {
	Quaternion orientation = worldTransformA.GetOrientation();
	Vector3 capsulePos = worldTransformA.GetPosition();

	Matrix3 transform = Matrix3(orientation);

	Vector3 b = capsulePos + (transform * Vector3(0, volumeA.GetHalfHeight() - volumeA.GetRadius() / 2, 0));
	Vector3 a = capsulePos - (transform * Vector3(0, volumeA.GetHalfHeight() - volumeA.GetRadius() / 2, 0));

	Vector3 c = worldTransformB.GetPosition();

	Vector3 ab = b - a;
	Vector3 ac = c - a;
	Vector3 bc = c - b;
	float e = Vector3::Dot(ac, ab);
	float f = Vector3::Dot(ab, ab);

	float dist2 = e <= 0.0f ? Vector3::Dot(ac, ac) : e >= f ? Vector3::Dot(bc, bc) : Vector3::Dot(ac, ac) - e * e / f;
	float radii = volumeA.GetRadius() / 2 + volumeB.GetRadius();
	if (dist2 <= radii * radii) {
		float t = Vector3::Dot(c - a, ab);
		Vector3 d;
		if (t <= 0.0f) { t = 0.0f; d = a; }
		else if (t >= f) { t = 1.0f; d = b; }
		else { t = t / f; d = a + ab * t; }

		Vector3 delta = worldTransformB.GetPosition() - d;

		float penetration = (radii - delta.Length());
		Vector3 normal = delta.Normalised();
		Vector3 localA = normal * volumeA.GetRadius() / 2;
		Vector3 localB = -normal * volumeB.GetRadius();

		collisionInfo.AddContactPoint(localA, localB, normal, penetration);
		return true;
	}

	return false;
}

bool CollisionDetection::OBBIntersection(const OBBVolume& volumeA, const Transform& worldTransformA, const OBBVolume& volumeB, const Transform& worldTransformB, CollisionInfo& collisionInfo) {
	return false;
}

Matrix4 GenerateInverseView(const Camera &c) {
	float pitch = c.GetPitch();
	float yaw	= c.GetYaw();
	Vector3 position = c.GetPosition();

	Matrix4 iview =
		Matrix4::Translation(position) *
		Matrix4::Rotation(-yaw, Vector3(0, -1, 0)) *
		Matrix4::Rotation(-pitch, Vector3(-1, 0, 0));

	return iview;
}

Matrix4 GenerateInverseProjection(float aspect, float nearPlane, float farPlane, float fov) {
	float negDepth = nearPlane - farPlane;

	float invNegDepth = negDepth / (2 * (farPlane * nearPlane));

	Matrix4 m;

	float h = 1.0f / tan(fov*PI_OVER_360);

	m.array[0][0] = aspect / h;
	m.array[1][1] = tan(fov * PI_OVER_360);
	m.array[2][2] = 0.0f;

	m.array[2][3] = invNegDepth;//// +PI_OVER_360;
	m.array[3][2] = -1.0f;
	m.array[3][3] = (0.5f / nearPlane) + (0.5f / farPlane);

	return m;
}

Vector3 CollisionDetection::Unproject(const Vector3& screenPos, const Camera& cam) {
	Vector2 screenSize = Window::GetWindow()->GetScreenSize();

	float aspect	= screenSize.x / screenSize.y;
	float fov		= cam.GetFieldOfVision();
	float nearPlane = cam.GetNearPlane();
	float farPlane  = cam.GetFarPlane();

	//Create our inverted matrix! Note how that to get a correct inverse matrix,
	//the order of matrices used to form it are inverted, too.
	Matrix4 invVP = GenerateInverseView(cam) * GenerateInverseProjection(aspect, fov, nearPlane, farPlane);

	Matrix4 proj  = cam.BuildProjectionMatrix(aspect);

	//Our mouse position x and y values are in 0 to screen dimensions range,
	//so we need to turn them into the -1 to 1 axis range of clip space.
	//We can do that by dividing the mouse values by the width and height of the
	//screen (giving us a range of 0.0 to 1.0), multiplying by 2 (0.0 to 2.0)
	//and then subtracting 1 (-1.0 to 1.0).
	Vector4 clipSpace = Vector4(
		(screenPos.x / (float)screenSize.x) * 2.0f - 1.0f,
		(screenPos.y / (float)screenSize.y) * 2.0f - 1.0f,
		(screenPos.z),
		1.0f
	);

	//Then, we multiply our clipspace coordinate by our inverted matrix
	Vector4 transformed = invVP * clipSpace;

	//our transformed w coordinate is now the 'inverse' perspective divide, so
	//we can reconstruct the final world space by dividing x,y,and z by w.
	return Vector3(transformed.x / transformed.w, transformed.y / transformed.w, transformed.z / transformed.w);
}

Ray CollisionDetection::BuildRayFromMouse(const Camera& cam) {
	Vector2 screenMouse = Window::GetMouse()->GetAbsolutePosition();
	Vector2 screenSize	= Window::GetWindow()->GetScreenSize();

	//We remove the y axis mouse position from height as OpenGL is 'upside down',
	//and thinks the bottom left is the origin, instead of the top left!
	Vector3 nearPos = Vector3(screenMouse.x,
		screenSize.y - screenMouse.y,
		-0.99999f
	);

	//We also don't use exactly 1.0 (the normalised 'end' of the far plane) as this
	//causes the unproject function to go a bit weird. 
	Vector3 farPos = Vector3(screenMouse.x,
		screenSize.y - screenMouse.y,
		0.99999f
	);

	Vector3 a = Unproject(nearPos, cam);
	Vector3 b = Unproject(farPos, cam);
	Vector3 c = b - a;

	c.Normalise();

	return Ray(cam.GetPosition(), c);
}

//http://bookofhook.com/mousepick.pdf
Matrix4 CollisionDetection::GenerateInverseProjection(float aspect, float fov, float nearPlane, float farPlane) {
	Matrix4 m;

	float t = tan(fov*PI_OVER_360);

	float neg_depth = nearPlane - farPlane;

	const float h = 1.0f / t;

	float c = (farPlane + nearPlane) / neg_depth;
	float e = -1.0f;
	float d = 2.0f*(nearPlane*farPlane) / neg_depth;

	m.array[0][0] = aspect / h;
	m.array[1][1] = tan(fov * PI_OVER_360);
	m.array[2][2] = 0.0f;

	m.array[2][3] = 1.0f / d;

	m.array[3][2] = 1.0f / e;
	m.array[3][3] = -c / (d * e);

	return m;
}

/*
And here's how we generate an inverse view matrix. It's pretty much
an exact inversion of the BuildViewMatrix function of the Camera class!
*/
Matrix4 CollisionDetection::GenerateInverseView(const Camera &c) {
	float pitch = c.GetPitch();
	float yaw	= c.GetYaw();
	Vector3 position = c.GetPosition();

	Matrix4 iview =
		Matrix4::Translation(position) *
		Matrix4::Rotation(yaw, Vector3(0, 1, 0)) *
		Matrix4::Rotation(pitch, Vector3(1, 0, 0));

	return iview;
}


/*
If you've read through the Deferred Rendering tutorial you should have a pretty
good idea what this function does. It takes a 2D position, such as the mouse
position, and 'unprojects' it, to generate a 3D world space position for it.

Just as we turn a world space position into a clip space position by multiplying
it by the model, view, and projection matrices, we can turn a clip space
position back to a 3D position by multiply it by the INVERSE of the
view projection matrix (the model matrix has already been assumed to have
'transformed' the 2D point). As has been mentioned a few times, inverting a
matrix is not a nice operation, either to understand or code. But! We can cheat
the inversion process again, just like we do when we create a view matrix using
the camera.

So, to form the inverted matrix, we need the aspect and fov used to create the
projection matrix of our scene, and the camera used to form the view matrix.

*/
Vector3	CollisionDetection::UnprojectScreenPosition(Vector3 position, float aspect, float fov, const Camera &c) {
	//Create our inverted matrix! Note how that to get a correct inverse matrix,
	//the order of matrices used to form it are inverted, too.
	Matrix4 invVP = GenerateInverseView(c) * GenerateInverseProjection(aspect, fov, c.GetNearPlane(), c.GetFarPlane());

	Vector2 screenSize = Window::GetWindow()->GetScreenSize();

	//Our mouse position x and y values are in 0 to screen dimensions range,
	//so we need to turn them into the -1 to 1 axis range of clip space.
	//We can do that by dividing the mouse values by the width and height of the
	//screen (giving us a range of 0.0 to 1.0), multiplying by 2 (0.0 to 2.0)
	//and then subtracting 1 (-1.0 to 1.0).
	Vector4 clipSpace = Vector4(
		(position.x / (float)screenSize.x) * 2.0f - 1.0f,
		(position.y / (float)screenSize.y) * 2.0f - 1.0f,
		(position.z) - 1.0f,
		1.0f
	);

	//Then, we multiply our clipspace coordinate by our inverted matrix
	Vector4 transformed = invVP * clipSpace;

	//our transformed w coordinate is now the 'inverse' perspective divide, so
	//we can reconstruct the final world space by dividing x,y,and z by w.
	return Vector3(transformed.x / transformed.w, transformed.y / transformed.w, transformed.z / transformed.w);
}

