#include "NetworkPlayer.h"
#include "NetworkedGame.h"
#include "GameWorld.h"
#include "Camera.h"
#include "PhysicsSystem.h"

#include <string>

using namespace NCL;
using namespace CSC8503;

NetworkPlayer::NetworkPlayer(NetworkedGame* game, int num, std::string name) : GameObject(name) {
	this->game = game;
	playerNum = num;

	speed = 8.0f;
	rotSpeed = 0.85f;
	jumpHeight = 7.0f;
	isGrounded = false;

	this->world = this->game->world;
	this->physics = this->game->physics;
	this->camera = this->game->world->GetMainCamera();
	cameraOffsetV = 1.5f;
	cameraOffsetH = 4.5f;

	Vector3 verticalOffset = Vector3(0, 1, 0) * cameraOffsetV;
	Vector3 horizontalOffset = transform.GetOrientation() * (Vector3(0, 0, 1) * cameraOffsetH);

	camera->SetPosition(transform.GetPosition() + verticalOffset + horizontalOffset);
	camera->SetYaw(transform.GetOrientation().ToEuler().y);
}

NetworkPlayer::~NetworkPlayer()	{

}

void NetworkPlayer::OnCollisionBegin(GameObject* otherObject) {
	if (game) {
		if (dynamic_cast<NetworkPlayer*>(otherObject))
		{
			game->OnPlayerCollision(this, (NetworkPlayer*)otherObject);
		}
	}
}