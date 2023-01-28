#include "Goose.h"
#include "GameObject.h"
#include "DestructibleObject.h"
#include "BehaviourSelector.h"
#include "BehaviourSequence.h"
#include "BehaviourAction.h"
#include "StateGameObject.h"
#include "Player.h"

using namespace NCL;
using namespace NCL::CSC8503;

/*Goose::Goose(GameWorld* world, Player* player, Vector3 spawnPoint, NavigationGrid* navGrid, std::string name) : GameObject(name) {
	speed = 12.0f;
	rotSpeed = 0.85f;
	node = -1;

	this->world = world;
	this->player = player;
	this->spawnPoint = spawnPoint;
	this->navGrid = navGrid;

	state = new StateGameObject();

	chasing = false;

	BehaviourAction* verifyRoute = new BehaviourAction("Verify route", 
		[&](float dt, GameObject* obj, BehaviourState state)->BehaviourState {
			if (state == Initialise) {
				return Ongoing;
			}
			else if (state == Ongoing) {
				if (((Goose*)obj)->node < 0 || ((Goose*)obj)->node >= ((Goose*)obj)->route.size() - 1) { // There is no route or the route is complete
					return Failure;
				}
				Vector3 pos = obj->GetTransform().GetPosition();
				Vector3 playerPos = ((Goose*)obj)->player->GetTransform().GetPosition();
				if ((playerPos - pos).Length() <= 30 && !chasing) { // If player near target it
					return Failure;
				}
				Vector3 target = ((Goose*)obj)->route.at(((Goose*)obj)->node + 1);
				Vector3 direction = Vector3(target.x, 0, target.z) - Vector3(pos.x, 0, pos.z);
				Vector3 movement = direction.Normalised() * speed * dt;
				if (direction.Length() - movement.Length() < -0.001f) {
					((Goose*)obj)->node += 1;
				}
				obj->GetTransform().SetPosition(pos + movement);
				return Success;
			}
			return state;
		}
	);

	BehaviourAction* targetPlayer = new BehaviourAction("Target player", 
		[&](float dt, GameObject* obj, BehaviourState state)->BehaviourState {
			if (state == Initialise) {
				return Ongoing;
			}
			else if (state == Ongoing) {
				Vector3 pos = obj->GetTransform().GetPosition();
				Vector3 playerPos = ((Goose*)obj)->player->GetTransform().GetPosition();
				if ((playerPos - pos).Length() > 30) { // If player near target it
					chasing = false;
					return Failure;
				}
				chasing = true;
				NavigationPath outPath;
				bool found = ((Goose*)obj)->navGrid->FindPath(pos, playerPos, outPath);
				Vector3 position;
				while (outPath.PopWaypoint(position)) {
					((Goose*)obj)->route.push_back(position);
				}

				((Goose*)obj)->node = 0;
				return Success;
			}
			return state;
		}
	);

	BahviourAction* idle = new BehaviourAction("Idle", 
		[&](float dt, GameObject* obj, BehaviourState state)->BehaviourState {
			if (state == Initialise) {
				return Ongoing;
			}
			else if (state == Ongoing) {

			}
			return state;
		}
	);

	root = new BehaviourSelector("Root");
	root->AddChild(verifyRoute);
	root->AddChild(targetPlayer);
	root->AddChild(idle);
}

void Goose::UpdateObject(const float dt) {
	root->Reset();
	BehaviourState state = Ongoing;
	//std::cout << "New Frame: " << std::endl;
	while (state == Ongoing) {
		state = root->Execute(dt, this);
	}
}

void Goose::OnCollisionBegin(GameObject* otherObject) {
	if (otherObject->GetCollisionLayer() == ITEM) {
		DestructibleObject* obj = (DestructibleObject*)otherObject;
		obj->Destroy(3.0f);
		player->score += (obj->value) * 2;
		++(player->destroyCount);
	}
}*/