#include "Player.h"
#include "Window.h"
#include "PhysicsObject.h"
#include "DestructibleObject.h"
#include "GameWorld.h"
#include "PhysicsSystem.h"
#include "GameObject.h"
#include "Camera.h"

#include "Matrix4.h"
#include "Vector3.h"

using namespace NCL;
using namespace Maths;
using namespace CSC8503;

void Player::UpdatePlayer(const float dt) {
	Ray r(transform.GetPosition(), Vector3(0, -1, 0));
	RayCollision closestCollision;
	if (world->Raycast(r, closestCollision, true, (GameObject*)this)) {
		isGrounded = closestCollision.rayDistance <= ((SphereVolume*)boundingVolume)->GetRadius() + 0.4f;
	}

	if (isGrounded) {
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::SPACE)) {
			physicsObject->AddForce(Vector3(0, jumpHeight * -physics->gravity.y, 0));
		}
	}

	Vector3 verticalOffset = Vector3(0, 1, 0) * cameraOffsetV;
	Vector3 horizontalOffset = transform.GetOrientation() * (Vector3(0, 0, 1) * cameraOffsetH);

	float modifiedSpeed = speed;

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::SHIFT)) {
		modifiedSpeed *= 2;
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::W)) {
		transform.SetPosition(transform.GetPosition() + (transform.GetOrientation() * (Vector3(0, 0, -1) * modifiedSpeed * dt)));
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::S)) {
		transform.SetPosition(transform.GetPosition() + (transform.GetOrientation() * (Vector3(0, 0, 1) * modifiedSpeed * dt)));
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::A)) {
		transform.SetOrientation(transform.GetOrientation() + Quaternion(Vector3(0, 1, 0) * rotSpeed * dt, 0.0f) * transform.GetOrientation());
		camera->SetYaw(transform.GetOrientation().ToEuler().y);
	}
	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::D)) {
		transform.SetOrientation(transform.GetOrientation() + Quaternion(Vector3(0, -1, 0) * rotSpeed * dt, 0.0f) * transform.GetOrientation());
		camera->SetYaw(transform.GetOrientation().ToEuler().y);
	}

	camera->SetPosition(transform.GetPosition() + verticalOffset + horizontalOffset);
}

void Player::OnCollisionBegin(GameObject* otherObject) {
	if (otherObject->GetCollisionLayer() == ITEM) {
		DestructibleObject* obj = (DestructibleObject*)otherObject;
		obj->Destroy(3.0f);
		score += obj->value;
		++destroyCount;
	}
}