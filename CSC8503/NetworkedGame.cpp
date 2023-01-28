#include "NetworkedGame.h"
#include "NetworkPlayer.h"
#include "NetworkObject.h"
#include "GameServer.h"
#include "GameClient.h"
#include "GameObject.h"
#include "RenderObject.h"
#include "PhysicsObject.h"

#define COLLISION_MSG 30

struct MessagePacket : public GamePacket {
	short playerID;
	short messageID;

	MessagePacket() {
		type = Message;
		size = sizeof(short) * 2;
	}
};

NetworkedGame::NetworkedGame()	{
	thisServer = nullptr;
	thisClient = nullptr;

	NetworkBase::Initialise();
	timeToNextPacket  = 0.0f;
	packetsToSnapshot = 0;
}

NetworkedGame::~NetworkedGame()	{
	delete thisServer;
	delete thisClient;
}

void NetworkedGame::StartAsServer() {
	name = "Server";

	thisServer = new GameServer(NetworkBase::GetDefaultPort(), 4);

	thisServer->RegisterPacketHandler(Received_State, this);
	thisServer->RegisterPacketHandler(Hello, this);
	thisServer->RegisterPacketHandler(Player_Connected_Ack, this);
	thisServer->RegisterPacketHandler(Client, this);
	std::cout << "Server: I am now a server" << std::endl;

	StartLevel();
}

void NetworkedGame::StartAsClient(char a, char b, char c, char d) {
	name = "Client (?)";

	thisClient = new GameClient();

	thisClient->RegisterPacketHandler(Delta_State, this);
	thisClient->RegisterPacketHandler(Full_State, this);
	thisClient->RegisterPacketHandler(Player_Connected, this);
	thisClient->RegisterPacketHandler(Player_Disconnected, this);

	thisClient->Connect(a, b, c, d, NetworkBase::GetDefaultPort());

	std::cout << "Client: I am now a Client" << std::endl;

	StartLevel();
}

void NetworkedGame::UpdateGame(float dt) {
	timeToNextPacket -= dt;
	if (timeToNextPacket < 0) {
		if (thisServer) {
			UpdateAsServer(dt);
		}
		else if (thisClient) {
			UpdateAsClient(dt);
		}
		timeToNextPacket += 1.0f / 20.0f; //20hz server/client update
	}

	if (!thisServer && Window::GetKeyboard()->KeyPressed(KeyboardKeys::F9)) {
		StartAsServer();
	}
	if (!thisClient && Window::GetKeyboard()->KeyPressed(KeyboardKeys::F10)) {
		StartAsClient(127,0,0,1);
	}

	if (thisServer) {
		Debug::Print("Server", Vector2(85, 95), Vector4(0.5f, 0, 1, 1));
	}
	else if (thisClient) {
		Debug::Print("Client " + std::to_string(thisClient->GetID()), Vector2(80, 95), Vector4(1, 0.5f, 0, 1));
	}

	TutorialGame::UpdateGame(dt);
}

void NetworkedGame::UpdateAsServer(float dt) {
	//thisServer->UpdateServer();

	packetsToSnapshot--;
	if (packetsToSnapshot < 0) {
		BroadcastSnapshot(false);
		packetsToSnapshot = 5;
	}
	else {
		BroadcastSnapshot(true);
	}
}

void NetworkedGame::UpdateAsClient(float dt) {
	//thisClient->UpdateClient();

	ClientPacket newPacket;

	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::SPACE)) {
		//fire button pressed!
		newPacket.buttonstates[0] = 1;
		newPacket.lastID = 0; // This is the network state id of the client
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::W)) {
		//fire button pressed!
		newPacket.buttonstates[1] = 1;
		newPacket.lastID = 0;
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::A)) {
		//fire button pressed!
		newPacket.buttonstates[2] = 1;
		newPacket.lastID = 0; 
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::S)) {
		//fire button pressed!
		newPacket.buttonstates[3] = 1;
		newPacket.lastID = 0; 
	}
	if (Window::GetKeyboard()->KeyPressed(KeyboardKeys::D)) {
		//fire button pressed!
		newPacket.buttonstates[4] = 1;
		newPacket.lastID = 0; 
	}
	thisClient->SendPacket(newPacket);
}

void NetworkedGame::BroadcastSnapshot(bool deltaFrame) {
	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;

	world->GetObjectIterators(first, last);

	for (auto i = first; i != last; ++i) {
		NetworkObject* o = (*i)->GetNetworkObject();
		if (!o) {
			continue;
		}
		//TODO - you'll need some way of determining
		//when a player has sent the server an acknowledgement
		//and store the lastID somewhere. A map between player
		//and an int could work, or it could be part of a 
		//NetworkPlayer struct. 
		int playerState = 0;
		GamePacket* newPacket = nullptr;
		if (o->WritePacket(&newPacket, deltaFrame, playerState)) {
			thisServer->SendGlobalPacket(*newPacket);
			delete newPacket;
		}
	}
}

void NetworkedGame::UpdateMinimumState() {
	//Periodically remove old data from the server
	int minID = INT_MAX;
	int maxID = 0; //we could use this to see if a player is lagging behind?

	for (auto i : stateIDs) {
		minID = min(minID, i.second);
		maxID = max(maxID, i.second);
	}
	//every client has acknowledged reaching at least state minID
	//so we can get rid of any old states!
	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;
	world->GetObjectIterators(first, last);

	for (auto i = first; i != last; ++i) {
		NetworkObject* o = (*i)->GetNetworkObject();
		if (!o) {
			continue;
		}
		o->UpdateStateHistory(minID); //clear out old states so they arent taking up memory...
	}
}

void NetworkedGame::SpawnPlayer(int id) {
		NetworkPlayer* player = AddNetworkPlayerToWorld(Vector3(180, 0, 180 - id * 5), id);
		if (thisClient) { localPlayer = player; }
		GameObject obj = (GameObject)*player;
		NetworkObject* networkObject = new NetworkObject(obj, -id);
		localPlayer->SetNetworkObject(networkObject);
		networkObjects.push_back(networkObject);
		if (thisServer) { serverPlayers.emplace(id, (GameObject*)player); }
}

void NetworkedGame::StartLevel() {
	InitNetworkGame();

	std::vector<GameObject*>::const_iterator first;
	std::vector<GameObject*>::const_iterator last;
	world->GetObjectIterators(first, last);

	int n = 0;
	for (auto i = first; i != last; ++i) {
		GameObject* obj = (*i);
		if (obj->GetCollisionLayer() == ENVIRONMENT) { continue; }
		NetworkObject* networkObject = new NetworkObject(*obj, n);
		obj->SetNetworkObject(networkObject);
		networkObjects.push_back(networkObject);
		++n;
	}
	
}

void NetworkedGame::ReceivePacket(int type, GamePacket* payload, int source) {
	HelloPacket* helloPacket;
	ClientAcknowledgePacket* packet;

	switch (type) {
	case String_Message:
		std::cout << name << " recieved message: " << ((StringPacket*)payload)->GetStringFromData() << std::endl;
		break;
	case Hello:
		std::cout << "Client says hello for the first time" << std::endl;
		break;
	case Client:
		std::cout << "Client sent updated button states" << std::endl;
		break;
	case Player_Connected:
		if (thisClient->GetID() >= 0) { break; }
		thisClient->SetID(((ClientConnectPacket*)payload)->id);
		UpdateName();

		SpawnPlayer(thisClient->GetID());

		packet = new ClientAcknowledgePacket(thisClient->GetID());
		thisClient->SendPacket((GamePacket&)packet);

		std::cout << name << " has an ID now!" << std::endl;
		break;
	case Player_Connected_Ack:
		packet = (ClientAcknowledgePacket*)payload;

		SpawnPlayer(packet->id);

		std::cout << "Spawned player for Client " << packet->id << std::endl;

		break;
	}
}

void NetworkedGame::OnPlayerCollision(NetworkPlayer* a, NetworkPlayer* b) {
	if (thisServer) { //detected a collision between players!
		MessagePacket newPacket;
		newPacket.messageID = COLLISION_MSG;
		newPacket.playerID  = a->GetPlayerNum();

		thisServer->SendGlobalPacket(newPacket);

		newPacket.playerID = b->GetPlayerNum();
		thisServer->SendGlobalPacket(newPacket);
	}
}

void NetworkedGame::UpdateName() {
	if (thisClient->GetID() < 0) { return; }

	name = "Client (" + std::to_string(GetClient()->GetID()) + ")";
}

NetworkPlayer* NetworkedGame::AddNetworkPlayerToWorld(const Vector3& position, const int num) {
	float meshSize = 1.0f;
	float inverseMass = 1.0f;

	NetworkPlayer* character = new NetworkPlayer(this, num, "player");
	SphereVolume* volume = new SphereVolume(1.0f);

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

void NetworkedGame::InitNetworkGame() {
	world->ClearAndErase();
	physics->Clear();

	navGrid = new NavigationGrid("TestGrid2.txt", Vector3(-200, 0, -200));

	BuildWorld();

	//player = AddPlayerToWorld(Vector3(180, 0, 180));

	AddDestructibleSphereToWorld(Vector3(20, 2, 100), 1.0f);
	AddDestructibleSphereToWorld(Vector3(12, 2, 80), 1.0f);
	AddDestructibleSphereToWorld(Vector3(16, 2, 80), 1.0f);

	//AddHumanToWorld(Vector3(100, 0, 100));
	//AddHumanToWorld(Vector3(120, 0, 80));
	//AddHumanToWorld(Vector3(80, 0, 80));
}