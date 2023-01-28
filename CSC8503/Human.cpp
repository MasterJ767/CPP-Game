#include "Human.h"
#include "GameObject.h"
#include "DestructibleObject.h"

Human::Human(GameWorld* world, Player* player, Vector3 spawnPoint, NavigationGrid* navGrid, std::string name) : GameObject(name) {
	speed = 12.0f;
	rotSpeed = 0.85f;
	node = -1;

	this->world = world;
	this->player = player;
	this->spawnPoint = spawnPoint;
	this->navGrid = navGrid;

	BehaviourAction* verifyRoute = new BehaviourAction("Verify route", 
		[&](float dt, GameObject* obj, BehaviourState state)->BehaviourState {
			if (state == Initialise) {
				return Ongoing;
			}
			else if (state == Ongoing) {
				if (((Human*)obj)->node < 0 || ((Human*)obj)->node >= ((Human*) obj)->route.size() - 1) { // There is no route or the route is complete
					return Failure;
				}
				Vector3 pos = obj->GetTransform().GetPosition();
				Vector3 target = ((Human*)obj)->route.at(((Human*)obj)->node + 1);
				Vector3 direction = Vector3(target.x, 0, target.z) - Vector3(pos.x, 0, pos.z);
				Vector3 movement = direction.Normalised() * speed * dt;
				if (direction.Length() - movement.Length() < -0.001f) {
					((Human*)obj)->node += 1;
				}
				obj->GetTransform().SetPosition(pos + movement);
				return Success;
			}
			return state;
		}
	);

	BehaviourAction* scanForPlayer = new BehaviourAction("Scan for player",
		[&](float dt, GameObject* obj, BehaviourState state)->BehaviourState {
			if (state == Initialise) {
				return Ongoing;
			}
			else if (state == Ongoing) {
				Vector3 pos = obj->GetTransform().GetPosition();
				Vector3 playerPos = ((Human*)obj)->player->GetTransform().GetPosition();
				Vector3 direction = Vector3(playerPos.x, 0, playerPos.z) - Vector3(pos.x, 0, pos.z);
				if (direction.Length() > 20.0f) { // The enemy does not worry if the player is more than 30 units away
					return Failure;
				}
				Vector3 forward = Matrix3(obj->GetTransform().GetOrientation()) * Vector3(0, 0, -1);
				float dot = Vector3::Dot(direction.Normalised(), forward);
				if (direction.Length() > 8.0f && dot < 0.5f) { // The enemy does not worry if the player is outside the 120 degree vision cone but is between 10 and 30 units away
					return Failure;
				}
				if (direction.Length() > 8.0f && dot >= 0.5f) {
					Ray r(playerPos, direction.Normalised());
					RayCollision closestCollision;
					if (((Human*)obj)->world->Raycast(r, closestCollision, true, obj)) { // The player is close and in the vision cone but an object is blocking direct line of sight
						if (((GameObject*)closestCollision.node)->GetCollisionLayer() & 2 != 1) {
							return Failure;
						}
					}
				}
				Quaternion orientation = obj->GetTransform().GetOrientation();
				Quaternion delta = Quaternion::LookAt(pos, playerPos, Vector3(0, 0, -1));
				obj->GetTransform().SetOrientation(orientation);
				return Success;
			}
			return state;
		}
	);

	BehaviourAction* moveAway = new BehaviourAction("Run Away", 
		[&](float dt, GameObject* obj, BehaviourState state)->BehaviourState {
			if (state == Initialise) {
				return Ongoing;
			}
			else if (state == Ongoing) {
				route.clear();

				NavigationPath outPath;

				Vector3 pos = obj->GetTransform().GetPosition();
				Vector3 playerPos = ((Human*)obj)->player->GetTransform().GetPosition();
				Vector3 direction = (Vector3(pos.x, 0, pos.z) - Vector3(playerPos.x, 0, playerPos.z)).Normalised();
				Vector3 dest = pos + direction * 22;
				bool found = ((Human*)obj)->navGrid->FindPath(pos, dest, outPath);
				if (!found) {
					((Human*)obj)->node = -1;
					dest = pos + (Quaternion::AxisAngleToQuaterion(Vector3(0, 1, 0), 90) * direction * 22);
					found = ((Human*)obj)->navGrid->FindPath(pos, dest, outPath);
					if (!found) {
						((Human*)obj)->node = -1;
						dest = pos + (Quaternion::AxisAngleToQuaterion(Vector3(0, 1, 0), -90) * direction * 22);
						found = ((Human*)obj)->navGrid->FindPath(pos, dest, outPath);
						if (!found) {
							((Human*)obj)->node = -1;
							return Failure;
						}
					}
				}
				Vector3 position;
				while (outPath.PopWaypoint(position)) {
					((Human*)obj)->route.push_back(position);
				}

				((Human*)obj)->node = 0;
				return Success;
			}
			return state;
		}
	);

	BehaviourSequence* checkForPlayer = new BehaviourSequence("Check for player");
	checkForPlayer->AddChild(scanForPlayer);
	checkForPlayer->AddChild(moveAway);

	BehaviourAction* idle = new BehaviourAction("Idle", 
		[&](float dt, GameObject* obj, BehaviourState state)->BehaviourState {
			if (state = Initialise) {
				return Ongoing;
			}
			else if (state == Ongoing) {
				if (rand() % 7 < 1) {
					Vector3 pos = obj->GetTransform().GetPosition();
					Vector3 direction = Vector3(0, 0, -1) * Quaternion::AxisAngleToQuaterion(Vector3(0, 1, 0), rand() % 360);
					Vector3 dest = pos + direction * 12;

					NavigationPath outPath;
					bool found = ((Human*)obj)->navGrid->FindPath(pos, dest, outPath);

					if (found) {
						Vector3 position;
						while (outPath.PopWaypoint(position)) {
							((Human*)obj)->route.push_back(position);
						}

						((Human*)obj)->node = 0;
					}
				}
				return Success;
			}
			return state;
		}
	);

	root = new BehaviourSelector("Root");
	root->AddChild(verifyRoute);
	root->AddChild(checkForPlayer);
	root->AddChild(idle);
}

void Human::UpdateObject(const float dt) {
	root->Reset();
	BehaviourState state = Ongoing;
	//std::cout << "New Frame: " << std::endl;
	while (state == Ongoing) {
		state = root->Execute(dt, this);
	}
}

void Human::OnCollisionBegin(GameObject* otherObject) {
	if (otherObject->GetCollisionLayer() == ITEM) {
		DestructibleObject* obj = (DestructibleObject*)otherObject;
		obj->Destroy(3.0f);
		player->score += (obj->value) * 2;
		++(player->destroyCount);
	}
}