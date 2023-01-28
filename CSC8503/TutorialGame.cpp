#include "TutorialGame.h"
#include "GameWorld.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "TextureLoader.h"

#include "CollisionVolume.h"
#include "PositionConstraint.h"
#include "OrientationConstraint.h"
#include "StateGameObject.h"
#include "NavigationGrid.h"
#include "NavigationPath.h"
#include "Layer.h"

#include "GameObject.h"
#include "Player.h"
#include "Human.h"
#include "Button.h"
#include "DestructibleObject.h"

#include <vector>

using namespace std;
using namespace NCL;
using namespace CSC8503;

TutorialGame::TutorialGame()	{
	world		= new GameWorld();
#ifdef USEVULKAN
	renderer	= new GameTechVulkanRenderer(*world);
#else 
	renderer = new GameTechRenderer(*world);
#endif

	physics		= new PhysicsSystem(*world);

	forceMagnitude	= 10.0f;
	useGravity		= false;
	inSelectionMode = false;

	win = false;
	gameOver = false;
	timer = 360.0f;

	InitialiseAssets();
}

/*

Each of the little demo scenarios used in the game uses the same 2 meshes, 
and the same texture and shader. There's no need to ever load in anything else
for this module, even in the coursework, but you can add it if you like!

*/
void TutorialGame::InitialiseAssets() {
	cubeMesh	= renderer->LoadMesh("cube.msh");
	sphereMesh	= renderer->LoadMesh("sphere.msh");
	coneMesh    = renderer->LoadMesh("cone.msh");
	charMesh	= renderer->LoadMesh("goat.msh");
	enemyMesh	= renderer->LoadMesh("Keeper.msh");
	bonusMesh	= renderer->LoadMesh("apple.msh");
	capsuleMesh = renderer->LoadMesh("capsule.msh");
	gooseMesh   = renderer->LoadMesh("goose.msh");

	basicTex	= renderer->LoadTexture("checkerboard.png");
	basicShader = renderer->LoadShader("scene.vert", "scene.frag");

	useGravity = true;

	InitCamera();
	InitGame();
}

TutorialGame::~TutorialGame()	{
	delete cubeMesh;
	delete sphereMesh;
	delete coneMesh;
	delete charMesh;
	delete enemyMesh;
	delete bonusMesh;
	delete gooseMesh;

	delete basicTex;
	delete basicShader;

	delete physics;
	delete renderer;
	delete world;
	delete navGrid;
}

void TutorialGame::UpdateGame(float dt) {
	if (player->destroyCount >= destroyTarget) {
		gameOver = true;
		win = true;
	}
	else if (timer <= 0) {
		gameOver = true;
		win = false;
	}

	player->UpdatePlayer(dt);

	if (!inSelectionMode) {
		world->GetMainCamera()->UpdateCamera(dt);
	}
	if (lockedObject != nullptr) {
		Vector3 objPos = lockedObject->GetTransform().GetPosition();
		Vector3 camPos = objPos + lockedOffset;

		Matrix4 temp = Matrix4::BuildViewMatrix(camPos, objPos, Vector3(0, 1, 0));

		Matrix4 modelMat = temp.Inverse();

		Quaternion q(modelMat);
		Vector3 angles = q.ToEuler(); //nearly there now!

		world->GetMainCamera()->SetPosition(camPos);
		world->GetMainCamera()->SetPitch(angles.x);
		world->GetMainCamera()->SetYaw(angles.y);
	}

	UpdateKeys();

	if (useGravity) {
		Debug::Print("G", Vector2(5, 95), Debug::RED);
	}
	else {
		//Debug::Print("(G)ravity off", Vector2(5, 95), Debug::RED);
	}

	Debug::Print("Score:" + std::to_string(player->score), Vector2(5, 5), Vector4(0, 1, 0.5f, 1));

	if ((int)timer % 60 < 10) {
		Debug::Print(std::to_string((int)timer / 60) + ":0" + std::to_string((int)timer % 60), Vector2(85, 5), Vector4(0, 1, 0.5f, 1));
	}
	else {
		Debug::Print(std::to_string((int)timer / 60) + ":" + std::to_string((int)timer % 60), Vector2(85, 5), Vector4(0, 1, 0.5f, 1));
	}

	RayCollision closestCollision;
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::K) && selectionObject) {
		Vector3 rayPos;
		Vector3 rayDir;

		rayDir = selectionObject->GetTransform().GetOrientation() * Vector3(0, 0, -1);
		rayPos = selectionObject->GetTransform().GetPosition();

		Ray r = Ray(rayPos, rayDir);

		if (world->Raycast(r, closestCollision, true, selectionObject)) {
			if (objClosest) {
				objClosest->GetRenderObject()->SetColourToDefault();
			}
			objClosest = (GameObject*)closestCollision.node;

			objClosest->GetRenderObject()->SetColour(Vector4(1, 0, 1, 1));
		}
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::J) && selectionObject) {
		Vector3 rayPos;
		Vector3 rayDir;

		rayDir = selectionObject->GetTransform().GetOrientation() * Vector3(0, 0, 1);
		rayPos = selectionObject->GetTransform().GetPosition();

		Ray r = Ray(rayPos, rayDir);

		if (world->Raycast(r, closestCollision, true, selectionObject)) {
			if (objClosest) {
				objClosest->GetRenderObject()->SetColourToDefault();
			}
			objClosest = (GameObject*)closestCollision.node;

			objClosest->GetRenderObject()->SetColour(Vector4(1, 0.5, 0, 1));
		}
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::Y) && selectionObject) {
		Vector3 rayPos;
		Vector3 rayDir;

		rayDir = selectionObject->GetTransform().GetOrientation() * Vector3(1, 0, 0);
		rayPos = selectionObject->GetTransform().GetPosition();

		Ray r = Ray(rayPos, rayDir);

		if (world->Raycast(r, closestCollision, true, selectionObject)) {
			if (objClosest) {
				objClosest->GetRenderObject()->SetColourToDefault();
			}
			objClosest = (GameObject*)closestCollision.node;

			objClosest->GetRenderObject()->SetColour(Vector4(1, 0, 0.25, 1));
		}
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::U) && selectionObject) {
		Vector3 rayPos;
		Vector3 rayDir;

		rayDir = selectionObject->GetTransform().GetOrientation() * Vector3(-1, 0, 0);
		rayPos = selectionObject->GetTransform().GetPosition();

		Ray r = Ray(rayPos, rayDir);

		if (world->Raycast(r, closestCollision, true, selectionObject)) {
			if (objClosest) {
				objClosest->GetRenderObject()->SetColourToDefault();
			}
			objClosest = (GameObject*)closestCollision.node;

			objClosest->GetRenderObject()->SetColour(Vector4(0, 0.5, 1, 1));
		}
	}

	Debug::DrawLine(Vector3(), Vector3(0, 100, 0), Vector4(0, 1, 0, 1)); // Up
	Debug::DrawLine(Vector3(), Vector3(100, 0, 0), Vector4(1, 0, 0, 1)); // Right
	Debug::DrawLine(Vector3(), Vector3(0, 0, -100), Vector4(0, 0, 1, 1)); // Forward

	SelectObject();
	MoveSelectedObject();

	if (selectionObject) {
		if (selectionObject->GetBoundingVolume()) {
			selectionObject->GetBoundingVolume()->Debug(selectionObject->GetTransform(), dt);
		}
	}

	if (axle) {
		axle->Update(dt);
	}

	KillPlane();

	world->UpdateWorld(dt);
	renderer->Update(dt);
	physics->Update(dt);

	renderer->Render();
	Debug::UpdateRenderables(dt);

	timer -= dt;
}

void TutorialGame::UpdateKeys() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F1)) {
		InitWorld(); //We can reset the simulation at any time with F1
		selectionObject = nullptr;
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F2)) {
		InitWorld2(); //We can reset the simulation at any time with F1
		selectionObject = nullptr;
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F3)) {
		InitGame(); //We can reset the simulation at any time with F1
		selectionObject = nullptr;
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F4)) {
		InitCamera(); //F2 will reset the camera to a specific default place
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::G)) {
		useGravity = !useGravity; //Toggle gravity!
		physics->UseGravity(useGravity);
	}
	//Running certain physics updates in a consistent order might cause some
	//bias in the calculations - the same objects might keep 'winning' the constraint
	//allowing the other one to stretch too much etc. Shuffling the order so that it
	//is random every frame can help reduce such bias.
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F9)) {
		world->ShuffleConstraints(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F10)) {
		world->ShuffleConstraints(false);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F7)) {
		world->ShuffleObjects(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::F8)) {
		world->ShuffleObjects(false);
	}

	if (lockedObject) {
		LockedObjectMovement();
	}
	else {
		DebugObjectMovement();
	}
}

void TutorialGame::LockedObjectMovement() {
	Matrix4 view		= world->GetMainCamera()->BuildViewMatrix();
	Matrix4 camWorld	= view.Inverse();

	Vector3 rightAxis = Vector3(camWorld.GetColumn(0)); //view is inverse of model!

	//forward is more tricky -  camera forward is 'into' the screen...
	//so we can take a guess, and use the cross of straight up, and
	//the right axis, to hopefully get a vector that's good enough!

	Vector3 fwdAxis = Vector3::Cross(Vector3(0, 1, 0), rightAxis);
	fwdAxis.y = 0.0f;
	fwdAxis.Normalise();


	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
		selectionObject->GetPhysicsObject()->AddForce(fwdAxis);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
		selectionObject->GetPhysicsObject()->AddForce(-fwdAxis);
	}

	if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NEXT)) {
		selectionObject->GetPhysicsObject()->AddForce(Vector3(0,-10,0));
	}
}

void TutorialGame::DebugObjectMovement() {
//If we've selected an object, we can manipulate it with some key presses
	if (inSelectionMode && selectionObject) {
		//Twist the selected object!
		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::LEFT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(-10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM7)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, 10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM8)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, -10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::UP)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, -10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::DOWN)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, 10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyboardKeys::NUM5)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, -10, 0));
		}
	}
}

void TutorialGame::InitCamera() {
	world->GetMainCamera()->SetNearPlane(0.1f);
	world->GetMainCamera()->SetFarPlane(500.0f);
	world->GetMainCamera()->SetPitch(-15.0f);
	world->GetMainCamera()->SetYaw(315.0f);
	world->GetMainCamera()->SetPosition(Vector3(-60, 40, 60));
	lockedObject = nullptr;
}

void TutorialGame::InitWorld() {
	world->ClearAndErase();
	physics->Clear();

	InitMixedGridWorld(15, 15, 3.5f, 3.5f);
	InitCapsuleGridWorld(15, 2, 3.5f, -3.5f);

	BridgeConstraintTest();

	InitGameExamples();
	InitDefaultFloor();

	//QuaternionMath();
}

void TutorialGame::InitWorld2() {
	world->ClearAndErase();
	physics->Clear();

	BridgeConstraintTest();

	InitDefaultFloor();

	TestPathfinding();
	DisplayPath();

	//QuaternionMath();
}

void TutorialGame::InitGame() {
	world->ClearAndErase();
	physics->Clear();

	navGrid = new NavigationGrid("TestGrid2.txt", Vector3(-200, 0, -200));

	BuildWorld();

	BuildObstacle();

	player = AddPlayerToWorld(Vector3(180, 0, 180));

	AddDestructibleSphereToWorld(Vector3(80, 2, 30), 1.0f, 10);
	//AddDestructibleCubeToWorld(Vector3(50, 15, -120), Vector3(1.0f, 1.0f, 1.0f), 10); // There is a chance this cube gets knocked into the void by the environment preventing the player from winning

	AddDestructibleSphereToWorld(Vector3(20, 2, 100), 1.0f, 1);
	AddDestructibleCubeToWorld(Vector3(12, 2, 80), Vector3(1.0f, 1.0f, 1.0f), 1);
	AddDestructibleSphereToWorld(Vector3(16, 2, 80), 1.0f, 1);
	AddDestructibleCubeToWorld(Vector3(34, 2, 100), Vector3(1.0f, 1.0f, 1.0f), 1);
	AddDestructibleSphereToWorld(Vector3(82, 2, 80), 1.0f, 1);
	AddDestructibleCubeToWorld(Vector3(86, 2, 90), Vector3(1.0f, 1.0f, 1.0f), 1);
	AddDestructibleSphereToWorld(Vector3(68, 2, 90), 1.0f, 1);
	AddDestructibleCubeToWorld(Vector3(86, 2, 80), Vector3(1.0f, 1.0f, 1.0f), 1);
	AddDestructibleSphereToWorld(Vector3(-68, 2, 160), 1.0f, 1);
	AddDestructibleCubeToWorld(Vector3(-72, 2, 160), Vector3(1.0f, 1.0f, 1.0f), 1);
	AddDestructibleSphereToWorld(Vector3(-64, 2, 148), 1.0f, 1);
	AddDestructibleCubeToWorld(Vector3(-74, 2, 152), Vector3(1.0f, 1.0f, 1.0f), 1);
	AddDestructibleSphereToWorld(Vector3(-60, 2, 154), 1.0f, 1);
	AddDestructibleCubeToWorld(Vector3(-80, 2, 146), Vector3(1.0f, 1.0f, 1.0f), 1);
	AddDestructibleCapsuleToWorld(Vector3(-36, 2, -86), 1.0f, 1.0f, 2);
	AddDestructibleCapsuleToWorld(Vector3(-34, 2, -92), 1.0f, 1.0f, 2);
	AddDestructibleCapsuleToWorld(Vector3(-44, 2, -78), 1.0f, 1.0f, 2);
	AddDestructibleCapsuleToWorld(Vector3(-42, 2, -82), 1.0f, 1.0f, 2);
	AddDestructibleCapsuleToWorld(Vector3(-24, 2, -108), 1.0f, 1.0f, 2);
	AddDestructibleCapsuleToWorld(Vector3(-22, 2, -112), 1.0f, 1.0f, 2);
	AddDestructibleCapsuleToWorld(Vector3(-26, 2, -116), 1.0f, 1.0f, 2);
	AddDestructibleCapsuleToWorld(Vector3(-32, 2, -104), 1.0f, 1.0f, 2);

	AddHumanToWorld(Vector3(100, 0, 100));
	AddHumanToWorld(Vector3(120, 0, 80));
	AddHumanToWorld(Vector3(80, 0, 80));
	AddHumanToWorld(Vector3(-60, 0, 160));
	AddHumanToWorld(Vector3(-40, 0, -100));
	AddHumanToWorld(Vector3(-170, 0, -40));
}

/*

A single function to add a large immoveable cube to the bottom of our world

*/
GameObject* TutorialGame::AddFloorToWorld(const Vector3& position, const Vector3& dimensions) {
	GameObject* floor = new GameObject();

	AABBVolume* volume = new AABBVolume(dimensions);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform().SetScale(dimensions * 2).SetPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, nullptr, basicShader, Vector4(0.1f, 0.1f, 0.1f, 1)));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia(false);

	world->AddGameObject(floor);

	return floor;
}

void TutorialGame::BuildWorld() {
	GridNode* nodes = navGrid->GetNodes();

	GameObject* block = nullptr;
	GameObject* previous = nullptr;
	char last = '$';
	for (int y = 0; y < navGrid->gridHeight; ++y) {
		for (int x = 0; x < navGrid->gridWidth; ++x) {
			char current = nodes[(navGrid->gridWidth * y) + x].type;
			Vector3 pos = nodes[(navGrid->gridWidth * y) + x].position;
			if (current == 'b') {
				pos.y -= 5.0f;
				block = AddCubeToWorld(pos, Vector3(navGrid->nodeSize * 0.35f, navGrid->nodeSize * 0.1f, navGrid->nodeSize * 0.40f), nullptr, AXLE, 2.0f, Vector4(0.9f, 0.9f, 0.9f, 1));
				PositionConstraint* constraint = new PositionConstraint(previous, block, 10);
				OrientationConstraint* constraintOrient = new OrientationConstraint(previous, block);
				world->AddConstraint(constraint);
				world->AddConstraint(constraintOrient);
			}
			else if (current == 'x') {
				pos.y += 2.0f;
				AddCubeToWorld(pos, Vector3(navGrid->nodeSize * 0.5f, navGrid->nodeSize * 0.5f, navGrid->nodeSize * 0.5f), nullptr, ENVIRONMENT, 0, Vector4(0.1f, 0.1f, 0.1f, 1));
			}
			else if (current == 'v') {
				continue;
			}
			else { // Add floor where no wall or bridge
				pos.y -= 4.0f;
				block = AddCubeToWorld(pos, Vector3(navGrid->nodeSize * 0.5f, navGrid->nodeSize * 0.1f, navGrid->nodeSize * 0.5f), nullptr, ENVIRONMENT, 0, Vector4(0.9f, 0.9f, 0.9f, 1));
				if (last == 'b') {
					PositionConstraint* constraint = new PositionConstraint(previous, block, 10);
					OrientationConstraint* constraintOrient = new OrientationConstraint(previous, block);
					world->AddConstraint(constraint);
					world->AddConstraint(constraintOrient);
				}
			}

			if (current == 'o') {
				AddButtonToWorld(pos);
			}

			float doorDelta1 = navGrid->GetDoorStatus() ? 2.0f : -10.0f;
			float doorDelta2 = navGrid->GetDoorStatus() ? -10.0f : 2.0f;
			
			if (current == 'd') {
				pos.y = doorDelta1;
				GameObject* door = AddCubeToWorld(pos, Vector3(navGrid->nodeSize * 0.25f, navGrid->nodeSize * 0.5f, navGrid->nodeSize * 0.5f), nullptr, ENVIRONMENT, 0, Vector4(1, 0, 0.25f, 1));
				door1list.push_back(door);
			}
			else if (current == 'h') {
				pos.y = doorDelta1;
				GameObject* door = AddCubeToWorld(pos, Vector3(navGrid->nodeSize * 0.5f, navGrid->nodeSize * 0.5f, navGrid->nodeSize * 0.25f), nullptr, ENVIRONMENT, 0, Vector4(1, 0, 0.25f, 1));
				door1list.push_back(door);
			}
			else if (current == 'p') {
				pos.y = doorDelta2;
				GameObject* door = AddCubeToWorld(pos, Vector3(navGrid->nodeSize * 0.25f, navGrid->nodeSize * 0.5f, navGrid->nodeSize * 0.5f), nullptr, ENVIRONMENT, 0, Vector4(0.25f, 0, 1, 1));
				door2list.push_back(door);
			}
			else if (current == 'q') {
				pos.y = doorDelta2;
				GameObject* door = AddCubeToWorld(pos, Vector3(navGrid->nodeSize * 0.5f, navGrid->nodeSize * 0.5f, navGrid->nodeSize * 0.25f), nullptr, ENVIRONMENT, 0, Vector4(0.25f, 0, 1, 1));
				door2list.push_back(door);
			}

			previous = block;
			last = current;
		}
	}
}

void TutorialGame::SwitchDoors() {
	vector<GameObject*> currentActiveDoors = navGrid->GetDoorStatus() ? door2list : door1list;
	vector<GameObject*> currentInactiveDoors = navGrid->GetDoorStatus() ? door1list : door2list;

	for (int i = 0; i < currentActiveDoors.size(); ++i) {
		Vector3 pos = currentActiveDoors[i]->GetTransform().GetPosition();
		pos.y  = -10.0f;
		currentActiveDoors[i]->GetTransform().SetPosition(pos);
	}

	for (int i = 0; i < currentInactiveDoors.size(); ++i) {
		Vector3 pos = currentInactiveDoors[i]->GetTransform().GetPosition();
		pos.y = 2.0f;
		currentInactiveDoors[i]->GetTransform().SetPosition(pos);
	}
}

/*

Builds a game object that uses a sphere mesh for its graphics, and a bounding sphere for its
rigid body representation. This and the cube function will let you build a lot of 'simple' 
physics worlds. You'll probably need another function for the creation of OBB cubes too.

*/
GameObject* TutorialGame::AddSphereToWorld(const Vector3& position, float radius, float inverseMass, bool isHollow) {
	GameObject* sphere = new GameObject();

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform().SetScale(sphereSize).SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia(isHollow);

	world->AddGameObject(sphere);

	return sphere;
}

DestructibleObject* TutorialGame::AddDestructibleSphereToWorld(const Vector3& position, float radius, int value, float inverseMass, bool isHollow) {
	DestructibleObject* sphere = new DestructibleObject(value);

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform().SetScale(sphereSize).SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia(isHollow);

	sphere->SetCollisionLayer(ITEM);

	world->AddGameObject(sphere);

	++destroyTarget;

	return sphere;
}

GameObject* TutorialGame::AddCubeToWorld(const Vector3& position, Vector3 dimensions, TextureBase* texture, Layer layer, float inverseMass, Vector4 defaultColour, bool isHollow) {
	GameObject* cube = new GameObject();

	AABBVolume* volume = new AABBVolume(dimensions);
	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform().SetPosition(position).SetScale(dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, texture, basicShader, defaultColour));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia(isHollow);

	cube->SetCollisionLayer(layer);

	world->AddGameObject(cube);

	return cube;
}

DestructibleObject* TutorialGame::AddDestructibleCubeToWorld(const Vector3& position, Vector3 dimensions, int value, float inverseMass, bool isHollow) {
	DestructibleObject* cube = new DestructibleObject(value);

	AABBVolume* volume = new AABBVolume(dimensions);
	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform().SetPosition(position).SetScale(dimensions * 2);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia(isHollow);

	cube->SetCollisionLayer(ITEM);

	world->AddGameObject(cube);

	++destroyTarget;

	return cube;
}

GameObject* TutorialGame::AddCapsuleToWorld(const Vector3& position, float halfHeight, float radius, float inverseMass, bool isHollow) {
	GameObject* capsule = new GameObject();

	CapsuleVolume* volume = new CapsuleVolume(halfHeight, radius);
	capsule->SetBoundingVolume((CollisionVolume*)volume);

	capsule->GetTransform().SetScale(Vector3(radius, halfHeight, radius)).SetPosition(position);

	capsule->SetRenderObject(new RenderObject(&capsule->GetTransform(), capsuleMesh, basicTex, basicShader));
	capsule->SetPhysicsObject(new PhysicsObject(&capsule->GetTransform(), capsule->GetBoundingVolume()));

	capsule->GetPhysicsObject()->SetInverseMass(inverseMass);
	capsule->GetPhysicsObject()->InitCapsuleInertia(isHollow);

	world->AddGameObject(capsule);

	return capsule;
}

DestructibleObject* TutorialGame::AddDestructibleCapsuleToWorld(const Vector3& position, float halfHeight, float radius, int value, float inverseMass, bool isHollow) {
	DestructibleObject* capsule = new DestructibleObject(value);

	CapsuleVolume* volume = new CapsuleVolume(halfHeight, radius);
	capsule->SetBoundingVolume((CollisionVolume*)volume);

	capsule->GetTransform().SetScale(Vector3(radius, halfHeight, radius)).SetPosition(position).SetOrientation(Quaternion::AxisAngleToQuaterion(Vector3(0, 0, 1), 90));

	capsule->SetRenderObject(new RenderObject(&capsule->GetTransform(), capsuleMesh, basicTex, basicShader));
	capsule->SetPhysicsObject(new PhysicsObject(&capsule->GetTransform(), capsule->GetBoundingVolume()));

	capsule->GetPhysicsObject()->SetInverseMass(inverseMass);
	capsule->GetPhysicsObject()->InitCapsuleInertia(isHollow);

	capsule->SetCollisionLayer(ITEM);

	world->AddGameObject(capsule);

	++destroyTarget;

	return capsule;
}

Player* TutorialGame::AddPlayerToWorld(const Vector3& position) {
	float meshSize		= 1.0f;
	float inverseMass	= 1.0f;

	Player* character = new Player(world, physics, "player");
	SphereVolume* volume  = new SphereVolume(1.0f);

	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform().SetScale(Vector3(meshSize, meshSize, meshSize)).SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), charMesh, nullptr, basicShader, Vector4(0.5f, 0.4f, 0.1f, 1.0f)));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia(false);

	character->SetCollisionLayer(PLAYER);

	world->AddGameObject(character);

	return character;
}

Human* TutorialGame::AddHumanToWorld(const Vector3& position) {
	float inverseMass = 1.0f;

	Human* character = new Human(world, player, position, navGrid, "human");
	CapsuleVolume* volume = new CapsuleVolume(2.0f, 1.2f);

	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform().SetScale(Vector3(2.0f, 2.0f, 2.0f)).SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform() , enemyMesh, nullptr, basicShader, Vector4(1, 0.9f, 0.6f, 1.0f)));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitCapsuleInertia(false);

	character->SetCollisionLayer(HUMAN);

	world->AddGameObject(character);

	humans.push_back(character);

	return character;
}

Button* TutorialGame::AddButtonToWorld(const Vector3& position) {
	Button* button = new Button(this, navGrid, "button");
	SphereVolume* volume = new SphereVolume(2.25f, true);

	button->SetBoundingVolume((CollisionVolume*)volume);

	button->GetTransform().SetScale(Vector3(2.0f, 2.0f, 2.0f)).SetPosition(position);

	button->SetRenderObject(new RenderObject(&button->GetTransform(), sphereMesh, basicTex, basicShader, Vector4(1, 0, 0.25f, 1)));
	button->SetPhysicsObject(new PhysicsObject(&button->GetTransform(), button->GetBoundingVolume()));

	button->GetPhysicsObject()->SetInverseMass(0);
	button->GetPhysicsObject()->InitSphereInertia(false);

	button->SetCollisionLayer(BUTTON);

	world->AddGameObject(button);

	return button;
}

/*Goose* TutorialGame::AddGooseToWorld(const Vector3& position) {
	Goose* character = new Goose(world, player, position, navGrid, "goose");
	SphereVolume* volume  = new SphereVolume(1.0f);

	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform().SetScale(Vector3(meshSize, meshSize, meshSize)).SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), gooseMesh, nullptr, basicShader, Vector4(0.5f, 0.4f, 0.1f, 1.0f)));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia(false);

	character->SetCollisionLayer(GOOSE);

	world->AddGameObject(character);
*/

GameObject* TutorialGame::AddEnemyToWorld(const Vector3& position) {
	float meshSize		= 3.0f;
	float inverseMass	= 0.5f;

	GameObject* character = new GameObject();

	AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.9f, 0.3f) * meshSize);
	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), enemyMesh, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia(false);

	world->AddGameObject(character);

	return character;
}

GameObject* TutorialGame::AddBonusToWorld(const Vector3& position) {
	GameObject* apple = new GameObject();

	SphereVolume* volume = new SphereVolume(0.5f);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform()
		.SetScale(Vector3(2, 2, 2))
		.SetPosition(position);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), bonusMesh, nullptr, basicShader));
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetPhysicsObject()->SetInverseMass(1.0f);
	apple->GetPhysicsObject()->InitSphereInertia(false);

	world->AddGameObject(apple);

	return apple;
}

StateGameObject* TutorialGame::AddStateObjectToWorld(const Vector3& position) {
	StateGameObject* apple = new StateGameObject();

	SphereVolume* volume = new SphereVolume(0.5f);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform().SetPosition(position).SetScale(Vector3(2, 2, 2));

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), sphereMesh, basicTex, basicShader));
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetPhysicsObject()->SetInverseMass(1.0f);
	apple->GetPhysicsObject()->InitSphereInertia(false);

	world->AddGameObject(apple);

	return apple;
}

void TutorialGame::InitDefaultFloor() {
	AddFloorToWorld(Vector3(0, -20, 0), Vector3(200, 2, 200));
}

void TutorialGame::InitGameExamples() {
	AddPlayerToWorld(Vector3(0, 5, 0));
	AddEnemyToWorld(Vector3(5, 5, 0));
	AddBonusToWorld(Vector3(10, 5, 0));
}

void TutorialGame::InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius) {
	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddSphereToWorld(position, radius, 1.0f);
		}
	}
	AddFloorToWorld(Vector3(0, -2, 0), Vector3(200, 2, 200));
}

void TutorialGame::InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing) {
	float sphereRadius = 1.0f;
	Vector3 cubeDims = Vector3(1, 1, 1);

	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);

			if (rand() % 2) {
				AddCubeToWorld(position, cubeDims, basicTex, DEFAULT);
			}
			else {
				AddSphereToWorld(position, sphereRadius);
			}
		}
	}
}

void TutorialGame::InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims) {
	for (int x = 1; x < numCols+1; ++x) {
		for (int z = 1; z < numRows+1; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddCubeToWorld(position, cubeDims, basicTex, DEFAULT, 1.0f);
		}
	}
}

void TutorialGame::InitCapsuleGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing) {
	float radius = 1.0f;
	float halfHeight = 1.0f;

	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3((x + 1) * colSpacing, 10.0f, z * rowSpacing);
			AddCapsuleToWorld(position, halfHeight, radius);
			//Debug::DrawLine(position, position + (Vector3(0, halfHeight, 0)), Vector4(1, 0, 1, 1), 50.0f);
		}
	}
}

/*
Every frame, this code will let you perform a raycast, to see if there's an object
underneath the cursor, and if so 'select it' into a pointer, so that it can be 
manipulated later. Pressing Q will let you toggle between this behaviour and instead
letting you move the camera around. 

*/
bool TutorialGame::SelectObject() {
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::Q)) {
		inSelectionMode = !inSelectionMode;
		if (inSelectionMode) {
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);
		}
		else {
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
		}
	}
	if (inSelectionMode) {
		//Debug::Print("Press Q to change to camera mode!", Vector2(5, 85));

		if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::LEFT)) {
			if (selectionObject) {	//set colour to deselected;
				selectionObject->GetRenderObject()->SetColourToDefault();
				selectionObject = nullptr;
			}

			Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

			RayCollision closestCollision;
			if (world->Raycast(ray, closestCollision, true)) {
				selectionObject = (GameObject*)closestCollision.node;

				selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
				//Debug::DrawLine(ray.GetPosition(), ray.GetPosition() + (ray.GetDirection() * closestCollision.rayDistance), Vector4(0, 0.5, 1, 1), 10.0f);
				return true;
			}
			else {
				//Debug::DrawLine(ray.GetPosition(), ray.GetPosition() + (ray.GetDirection() * closestCollision.rayDistance), Vector4(0, 0, 1, 1), 10.0f);
				return false;
			}
		}
		if (Window::GetKeyboard()->KeyPressed(NCL::KeyboardKeys::L)) {
			if (selectionObject) {
				if (lockedObject == selectionObject) {
					lockedObject = nullptr;
				}
				else {
					lockedObject = selectionObject;
				}
			}
		}
	}
	else {
		//Debug::Print("Press Q to change to select mode!", Vector2(5, 85));
	}
	return false;
}

/*
If an object has been clicked, it can be pushed with the right mouse button, by an amount
determined by the scroll wheel. In the first tutorial this won't do anything, as we haven't
added linear motion into our physics system. After the second tutorial, objects will move in a straight
line - after the third, they'll be able to twist under torque aswell.
*/

void TutorialGame::MoveSelectedObject() {
	//Debug::Print("Click Force:" + std::to_string(forceMagnitude), Vector2(5, 90));
	forceMagnitude += Window::GetMouse()->GetWheelMovement() * 100.0f;

	if (!selectionObject) {
		return;//we haven't selected anything!
	}
	//Push the selected object!
	if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::RIGHT)) {
		Ray ray = CollisionDetection::BuildRayFromMouse(*world->GetMainCamera());

		RayCollision closestCollision;
		if (world->Raycast(ray, closestCollision, true)) {
			if (closestCollision.node == selectionObject) {
				selectionObject->GetPhysicsObject()->AddForceAtPosition(ray.GetDirection() * forceMagnitude, closestCollision.collidedAt);
			}
		}
	}
}

void TutorialGame::BridgeConstraintTest() {
	Vector3 cubeSize = Vector3(8, 8, 8);

	float invCubeMass = 1;
	int numLinks = 10;
	float maxDistance = 30;
	float cubeDistance = 20;

	Vector3 startPos = Vector3(100, 100, 100);

	GameObject* start = AddCubeToWorld(startPos + Vector3(0, 0, 0), cubeSize, basicTex, ENVIRONMENT, 0, Vector4(1, 0, 0.25, 1));
	GameObject* end = AddCubeToWorld(startPos + Vector3((numLinks + 2) * cubeDistance, 0, 0), cubeSize, basicTex, ENVIRONMENT, 0, Vector4(0, 1, 0.25, 1));

	GameObject* previous = start;

	for (int i = 0; i < numLinks; ++i) {
		GameObject* block = AddCubeToWorld(startPos + Vector3((i + 1) * cubeDistance, 0, 0), cubeSize, basicTex, ENVIRONMENT, invCubeMass);
		//GameObject* block = AddCapsuleToWorld(startPos + Vector3((i + 1) * cubeDistance, 0, 0), 8, 8, invCubeMass);
		PositionConstraint* constraint = new PositionConstraint(previous, block, maxDistance);
		OrientationConstraint* constraintOrient = new OrientationConstraint(previous, block);
		world->AddConstraint(constraint);
		world->AddConstraint(constraintOrient);
		previous = block;
	}

	PositionConstraint* constraint = new PositionConstraint(previous, end, maxDistance);
	world->AddConstraint(constraint);
}

void TutorialGame::QuaternionMath() {
	Quaternion identity = Quaternion();
	Quaternion test = Quaternion::AxisAngleToQuaterion(Vector3(0, 1, 0), 45);
	Quaternion test2 = Quaternion::AxisAngleToQuaterion(Vector3(0, 1, 0), 90);

	std::cout << "q2 - q1           = " + (test - identity).To_String() + "\n";
	std::cout << "q2 * q1^-1        = " + (test * identity.Conjugate()).To_String() + "\n";
	std::cout << "q2 - q2           = " + (test - test).To_String() + "\n";
	std::cout << "q2 * q2^-1        = " + (test * test.Conjugate()).To_String() + "\n";
	std::cout << "q3 * 0.5          = " + (test2 * 0.5f).To_String() + "\n";
	std::cout << "q2                = " + (test).To_String() + "\n";
	std::cout << "lerp(q1, q3, 0.5) = " + (Quaternion::Lerp(identity, test2, 0.5f)).To_String() + "\n";
	std::cout << "q2 + q2           = " + (test + test).To_String() + "\n";
	std::cout << "q2 * q2           = " + (test * test).To_String() + "\n";
	std::cout << "q3                = " + (test2).To_String() + "\n";
	std::cout << "-q2               = " + (-test).To_String() + "\n";
	std::cout << "q2 * q3^-1        = " + (test * test2.Conjugate()).To_String() + "\n";
	std::cout << "q3 - q2           = " + (test2 - test).To_String() + "\n";
}

void TutorialGame::TestPathfinding() {
	NavigationGrid grid("TestGrid1.txt", Vector3(0, 0, 0));

	NavigationPath outPath;

	Vector3 startPos(80, -10, 20);
	Vector3 endPos(40, -10, 120);
	bool found = grid.FindPath(startPos, endPos, outPath);

	Vector3 pos;
	while (outPath.PopWaypoint(pos)) {
		testNodes.push_back(pos);
	}

	GridNode* nodes = grid.GetNodes();
	for (int y = 0; y < grid.gridHeight; ++y) {
		for (int x = 0; x < grid.gridWidth; ++x) {
			if (nodes[(grid.gridWidth * y) + x].type == 'x') {
				AddCubeToWorld(Vector3(x * grid.nodeSize, -10, y * grid.nodeSize), Vector3(10, 10, 10), basicTex, ENVIRONMENT, 0);
			}
		}
	}
}

void TutorialGame::CalculatePath(const Vector3 from, const Vector3 to) {
	testNodes.clear();

	NavigationPath outPath;

	bool found = navGrid->FindPath(from, to, outPath);
	Vector3 pos;
	while (outPath.PopWaypoint(pos)) {
		testNodes.push_back(pos);
	}
}

void TutorialGame::DisplayPath() {
	for (int i = 1; i < testNodes.size(); ++i) {
		Vector3 a = testNodes[i - 1];
		Vector3 b = testNodes[i];

		Debug::DrawLine(a, b, Vector4(1, 0, 1, 1), 100.0f);
	}
}

void TutorialGame::KillPlane() {
	if (player->GetTransform().GetPosition().y <= -40.0f) {
		player->score = 0;
		player->GetTransform().SetPosition(Vector3(180, 0, 180));
	}

	for (auto i : humans) {
		if (i->GetTransform().GetPosition().y <= -40.0f) {
			i->GetTransform().SetPosition(i->spawnPoint);
		}
	}
}

void TutorialGame::BuildObstacle() {
	AddCubeToWorld(Vector3(50, -15, -120), Vector3(2, 2, 2), nullptr, ENVIRONMENT, 0, Vector4(0.1f, 0.1f, 0.1f, 1));

	axle = new StateGameObject();

	Vector3 pos = Vector3(50, -5, -120);
	Vector3 dimensions = Vector3(1, 10, 1);

	AABBVolume* volume = new AABBVolume(dimensions);
	axle->SetBoundingVolume((CollisionVolume*)volume);

	axle->GetTransform().SetPosition(pos).SetScale(dimensions * 2);

	axle->SetRenderObject(new RenderObject(&axle->GetTransform(), cubeMesh, nullptr, basicShader, Vector4(0.1f, 0.1f, 0.1f, 1)));
	axle->SetPhysicsObject(new PhysicsObject(&axle->GetTransform(), axle->GetBoundingVolume()));

	axle->GetPhysicsObject()->SetInverseMass(0.00001f);
	axle->GetPhysicsObject()->InitCubeInertia(false);

	axle->SetCollisionLayer(AXLE);

	world->AddGameObject(axle);

	vector<GameObject*> platforms;

	for (int i = 0; i < 9; ++i) {
		if (i == 4) { 
			continue; 
		}
		float x = -navGrid->nodeSize + navGrid->nodeSize * (int)(i / 3);
		float y = i & 1 ? 0 : 2.0f;
		float z = -navGrid->nodeSize + navGrid->nodeSize * (i % 3);
		Vector3 newPos = pos + Vector3(x, y, z);
		GameObject* block = AddCubeToWorld(newPos, Vector3(navGrid->nodeSize * 0.4f, navGrid->nodeSize * 0.1f, navGrid->nodeSize * 0.4f), nullptr, AXLE, 1.0f);
		platforms.push_back(block);
	}

	float maxDistance = 40;
	float maxDistanceCentre = 20;
	AddConstraints(axle, platforms[0], maxDistanceCentre);
	AddConstraints(axle, platforms[1], maxDistanceCentre);
	AddConstraints(axle, platforms[2], maxDistanceCentre);
	AddConstraints(axle, platforms[4], maxDistanceCentre);
	AddConstraints(axle, platforms[7], maxDistanceCentre);
	AddConstraints(axle, platforms[6], maxDistanceCentre);
	AddConstraints(axle, platforms[5], maxDistanceCentre);
	AddConstraints(axle, platforms[3], maxDistanceCentre);
	AddConstraints(platforms[0], platforms[1], maxDistance);
	AddConstraints(platforms[1], platforms[2], maxDistance);
	AddConstraints(platforms[2], platforms[4], maxDistance);
	AddConstraints(platforms[4], platforms[7], maxDistance);
	AddConstraints(platforms[7], platforms[5], maxDistance);
	AddConstraints(platforms[5], platforms[3], maxDistance);
	AddConstraints(platforms[3], platforms[0], maxDistance);
}

void TutorialGame::AddConstraints(GameObject* a, GameObject* b, float maxDistance) {
	PositionConstraint* constraint = new PositionConstraint(a, b, maxDistance);
	OrientationConstraint* constraintOrient = new OrientationConstraint(a, b);
	world->AddConstraint(constraint);
	world->AddConstraint(constraintOrient);
}
