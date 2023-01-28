#include "DestructibleObject.h"
#include "RenderObject.h"
#include "PhysicsObject.h"
#include "Layer.h"

using namespace NCL::CSC8503;

void DestructibleObject::UpdateObject(const float dt) {
	if (destroy) {
		time -= dt;
		if (time <= 0) {
			destroy = false;
			Vector3 pos = GetTransform().GetPosition();
			pos.y = -100.0f;
			GetTransform().SetPosition(pos);
			GetPhysicsObject()->SetInverseMass(0);
			SetCollisionLayer(DEFAULT);
			SetActive(false);
		}
		else {
			GetRenderObject()->SetColour(Vector4(0, 0.75f, 1, time / maxTime));
		}
	}
}

void DestructibleObject::Destroy(const float timer) {
	destroy = true;
	
	SetCollisionLayer(ITEMDECAY);

	GetRenderObject()->SetColour(Vector4(0, 0.75f, 1, 1));
	maxTime = timer;
	time = timer;
}