#pragma once
#include "GameTechRenderer.h"
#ifdef USEVULKAN
#include "GameTechVulkanRenderer.h"
#endif
#include "PhysicsSystem.h"

#include "StateGameObject.h"
#include "NavigationGrid.h"

namespace NCL {
	namespace CSC8503 {
		class GameObject;
		class Player;
		class Human;
		class Button;
		class DestructibleObject;

		class TutorialGame		{
		public:
			TutorialGame();
			~TutorialGame();

			void SwitchDoors();

			virtual void UpdateGame(float dt);

			PhysicsSystem* physics;
			GameWorld* world;

			NavigationGrid* navGrid;

			bool gameOver;
			bool win;
		protected:
			void InitialiseAssets();

			void InitCamera();
			void UpdateKeys();

			void InitWorld(); // Test world 1
			void InitWorld2(); // Test World 2
			void InitGame(); // GameWorld

			/*
			These are some of the world/object creation functions I created when testing the functionality
			in the module. Feel free to mess around with them to see different objects being created in different
			test scenarios (constraints, collision types, and so on). 
			*/
			void InitGameExamples();

			void InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius);
			void InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing);
			void InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims);
			void InitCapsuleGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing);

			void InitDefaultFloor();

			bool SelectObject();
			void MoveSelectedObject();
			void DebugObjectMovement();
			void LockedObjectMovement();

			void BridgeConstraintTest();
			void QuaternionMath();

			void BuildObstacle();

			GameObject* AddFloorToWorld(const Vector3& position, const Vector3& dimensions);
			void BuildWorld();
			GameObject* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f, bool isHollow = false);
			DestructibleObject* AddDestructibleSphereToWorld(const Vector3& position, float radius, int value = 1, float inverseMass = 10.0f, bool isHollow = false);
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, TextureBase* texture, Layer layer, float inverseMass = 10.0f, Vector4 defaultColour = Vector4(1, 1, 1, 1), bool isHollow = false);
			DestructibleObject* AddDestructibleCubeToWorld(const Vector3& position, Vector3 dimensions, int value = 1, float inverseMass = 10.0f, bool isHollow = false);
			GameObject* AddCapsuleToWorld(const Vector3& position, float halfHeight, float radius, float inverseMass = 10.0f, bool isHollow = false);
			DestructibleObject* AddDestructibleCapsuleToWorld(const Vector3& position, float halfHeight, float radius, int value = 1, float inverseMass = 10.0f, bool isHollow = false);

			Player* AddPlayerToWorld(const Vector3& position);
			Human* AddHumanToWorld(const Vector3& position);
			Button* AddButtonToWorld(const Vector3& position);
			//Goose* AddGooseToWorld(const Vector3& position);
			GameObject* AddEnemyToWorld(const Vector3& position);
			GameObject* AddBonusToWorld(const Vector3& position);

			void AddConstraints(GameObject* a, GameObject* b, float maxDistance);

			StateGameObject* AddStateObjectToWorld(const Vector3& position);

			void TestPathfinding();
			void CalculatePath(const Vector3 from, const Vector3 to);
			void DisplayPath();

			void KillPlane();

#ifdef USEVULKAN
			GameTechVulkanRenderer*	renderer;
#else
			GameTechRenderer* renderer;
#endif

			bool useGravity;
			bool inSelectionMode;

			float		forceMagnitude;

			GameObject* selectionObject = nullptr;
			Player* player;
			//Goose* goose;

			MeshGeometry*	capsuleMesh = nullptr;
			MeshGeometry*	cubeMesh	= nullptr;
			MeshGeometry*	sphereMesh	= nullptr;
			MeshGeometry*   coneMesh = nullptr;
	

			TextureBase*	basicTex	= nullptr;
			ShaderBase*		basicShader = nullptr;

			//Coursework Meshes
			MeshGeometry*	charMesh	= nullptr;
			MeshGeometry*	enemyMesh	= nullptr;
			MeshGeometry*	bonusMesh	= nullptr;
			MeshGeometry*   gooseMesh   = nullptr;

			//Coursework Additional functionality	
			GameObject* lockedObject	= nullptr;
			Vector3 lockedOffset		= Vector3(0, 20, 20);
			void LockCameraToObject(GameObject* o) {
				lockedObject = o;
			}

			GameObject* objClosest = nullptr;

			StateGameObject* axle;
			vector<Vector3> testNodes;

			vector<Human*> humans;
			vector<GameObject*> door1list;
			vector<GameObject*> door2list;

			float timer;

			int destroyTarget;
		};
	}
}

