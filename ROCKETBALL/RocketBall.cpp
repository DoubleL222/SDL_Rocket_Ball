#include "RocketBall.h"
#include "Box2D/Dynamics/Contacts/b2Contact.h"
#include "GameObject.hpp"
#include "sre/RenderPass.hpp"
#include "PhysicsComponent.hpp"
#include "SpriteComponent.hpp"
#include "BallComponent.h"
#include "AbilityComponent.h"
#include "PlayerController.h"
#include "sre/profiler.hpp"
#include <iostream>

using namespace std;
using namespace sre;

const glm::vec2 RocketBall::windowSize(1600, 900); // some size for the window

RocketBall* RocketBall::gameInstance = nullptr;

RocketBall::RocketBall()
	:debugDraw(physicsScale)
{
	gameInstance = this;
	r.setWindowSize(windowSize);

	r.init(SDL_INIT_EVERYTHING, SDL_WINDOW_OPENGL);

	initGame();

	// setup callback functions
	r.joystickEvent = [&](SDL_Event& e)
	{
		onJoyInput(e);
	};
	r.keyEvent = [&](SDL_Event& e) {
		onKey(e);
	};
	r.frameUpdate = [&](float deltaTime) {
		update(deltaTime * timeScale);
	};
	r.frameRender = [&]() {
		render();
	};

	// start game loop
	r.startEventLoop();
}

void RocketBall::initGame() {

	//Joystick init
	//cout << "number of joysticks " << SDL_NumJoysticks();

	int numJoysticks = SDL_NumJoysticks();
	if (numJoysticks >= 1)
	{
		joy1 = SDL_JoystickOpen(0);
	}
	if (numJoysticks == 2)
	{
		joy2 = SDL_JoystickOpen(1);
	}

	if (world) {
		world->SetContactListener(nullptr);
	}

	sceneObjects.clear();
	camera.reset();
	physicsComponentLookup.clear();

	//MAKING THE SPRITE ATLAS
	if (!mySpriteAtlas)
		mySpriteAtlas = SpriteAtlas::create("RocketBallSprites.json", "RocketBallSprites.png");

	//INITIALIZING PHYSICS
	initPhysics();

	//Setup Camera
	auto camGameObj = createGameObject();
	camGameObj->name = "Camera";
	camera = camGameObj->addComponent<GameCamera>();
	camGameObj->setPosition(windowSize*0.5f);

	floorGameObj->name = "Floor";
	ceilingGameObj->name = "Ceiling";
	leftWall->name = "LeftWall";
	rightWall->name = "RightWall";
	//Set background if it has not yet been set
	if (!background_Layer_1.isInit) {
		background_Layer_1.init("skybackdrop.png", -windowSize.x*0.5f, -windowSize.y*0.5f, true);
	}

	//Set size for the goals
	setPlayField.setGoalSizes(goalSizes);
	//Init playing field
	setPlayField.createPlayField(mySpriteAtlas);

	for (int i = 0; i < 5; i++) {
		createAbilityBox("ability_" + i, mySpriteAtlas->get("gray.png"), createGameObject(), { 1 * i,1 * i }, { 0.5,0.5 }, { 0,0 }, physicsScale);
	}

#pragma region Dynamic Elements
	//Spawn Player1

	player1 = createGameObject();
	player1->name = "Player_1";
	//player1->addComponent<PlayerController>();
	auto spriteComp = player1->addComponent<SpriteComponent>();
	auto player1Sprite = mySpriteAtlas->get("ManUntd.png");
	player1Sprite.setScale(glm::vec2(0.2f, 0.2f));
	spriteComp->setSprite(player1Sprite);
	player1->setPosition(glm::vec2(windowSize.x * 0.2, -windowSize.y * 0.2 + (player1Sprite.getSpriteSize().y * 0.5f)));
	auto physComp = player1->addComponent<PhysicsComponent>();
	//physComp->initPolygon(b2BodyType::b2_dynamicBody, vertices);
	physComp->initCarCollider(glm::vec2(0.3f, 0.07f), player1->getPosition() / physicsScale, playerFriction, playerDensity, playerLinearDamping, playerAngularDamping);
	P1Origin = player1->getPosition();

	//Spawn Player2
	player2 = createGameObject();
	player2->name = "Player_2";
	player2->addComponent<PlayerController>();
	spriteComp = player2->addComponent<SpriteComponent>();
	auto player2Sprite = mySpriteAtlas->get("BlackCarCropped.png");
	player2Sprite.setFlip(glm::vec2(-1, 0));
	player2Sprite.setScale(glm::vec2(0.2f, 0.2f));
	spriteComp->setSprite(player2Sprite);
	player2->setPosition(glm::vec2(-windowSize.x * 0.2, -windowSize.y * 0.2 + (player2Sprite.getSpriteSize().y * 0.5f)));
	physComp = player2->addComponent<PhysicsComponent>();
	physComp->initCarCollider(glm::vec2(0.3f, 0.07f), player2->getPosition() / physicsScale, playerFriction, playerDensity, playerLinearDamping, playerAngularDamping);
	//physComp->initCircle(b2BodyType::b2_dynamicBody, 20 / physicsScale, player2->getPosition() / physicsScale, playerDensity, playerFriction, playerRestitution, playerLinearDamping, playerAngularDamping, true);
	P2Origin = player2->getPosition();

	//Spawn (outer) Soccer Ball w/e sprite
	soccerBall = createGameObject();
	soccerBall->name = "OuterBall";
	spriteComp = soccerBall->addComponent<SpriteComponent>();
	auto soccerBallSprite = mySpriteAtlas->get("SoccerBall.png");
	soccerBallSprite.setScale(glm::vec2(0.4f, 0.4f));
	spriteComp->setSprite(soccerBallSprite);
	soccerBall->setPosition(glm::vec2(0, windowSize.y * 0.3f));
	OuterBallPhyiscs = soccerBall->addComponent<PhysicsComponent>();
	OuterBallPhyiscs->initCircle(b2BodyType::b2_dynamicBody, 50 / physicsScale, soccerBall->getPosition() / physicsScale, ballDensity, ballFriction, ballRestitution, ballLinearDamping, ballAngularDamping, false, SOCCERBALL, BOUNDARY | PLAYER | SOCCERBALL);
	b2sbOuterOrigin = OuterBallPhyiscs->body->GetPosition();
	sbOuterOrigin = soccerBall->getPosition();

	//Create an inner circle collider in the soccerball to score goals only 
	//when the ball is roughly 50% inside the goal (i.e. a very smaller cirlce collider inside the big)
	//The position of the inner collider is set in the update().
	soccerBallInner = createGameObject();
	soccerBallInner->name = "InnerBall";
	soccerBallInner->setPosition(soccerBall->getPosition());
	InnerBallPhysics = soccerBallInner->addComponent<PhysicsComponent>();
	InnerBallPhysics->initCircle(b2BodyType::b2_dynamicBody, 1 / physicsScale, soccerBall->getPosition() / physicsScale, ballDensity, false, SOCCERBALL, BOUNDARY | PLAYER | SOCCERBALL);
	InnerBallPhysics->setSensor(true);
	auto soccerBallInnerMechanics = soccerBallInner->addComponent<BallComponent>();
	soccerBallInnerMechanics->setOuterBall(OuterBallPhyiscs);
	b2sbInnerOrigin = InnerBallPhysics->body->GetPosition();
	sbInnerOrigin = soccerBallInner->getPosition();

#pragma endregion

	//SET GAME STATE
	gameState = GameState::Ready;
}

#pragma endregion
void RocketBall::createAbilityBox(std::string name, sre::Sprite sprite, std::shared_ptr<GameObject> obj, glm::vec2 pos, glm::vec2 scale, glm::vec2 colBuffer, const float phyScale) {
	auto abilityBox = sprite;
	abilityBox.setScale({ scale.x, scale.y });
	auto abilityBox_obj = obj;
	abilityBox_obj->name = name;
	auto abilityBoxSpriteComp = abilityBox_obj->addComponent<SpriteComponent>();
	abilityBox_obj->setPosition(pos);
	glm::vec2 scaleCol{ (abilityBox.getSpriteSize().x * abilityBox.getScale().x / 2) + colBuffer.x, (abilityBox.getSpriteSize().y * abilityBox.getScale().y / 2) + colBuffer.y };
	abilityBoxSpriteComp->setSprite(abilityBox);
	auto abilityBox_physics = abilityBox_obj->addComponent<PhysicsComponent>();
	abilityBox_physics->initBox(b2_dynamicBody, scaleCol / phyScale, { abilityBox_obj->getPosition().x / phyScale, abilityBox_obj->getPosition().y / phyScale }, 0, ABILITYBOX, ABILITYBOX | BOUNDARY | PLAYER);
	auto abilityBoxBehaviour = abilityBox_obj->addComponent<AbilityComponent>();
}


void RocketBall::nextRound() {
	player1->setPosition(P1Origin);
	player1->getComponent<PhysicsComponent>()->body->SetTransform(b2P1Origin, 0);
	player1->getComponent<PhysicsComponent>()->setLinearVelocity({ 0,0 });;
	player1->getComponent<PlayerController>()->resetInputs();
	//Needs to reset player inputs, too!
	//player1->getComponent<PlayerController>()->resetJumps();

	player2->setPosition(P2Origin);
	player2->getComponent<PhysicsComponent>()->body->SetTransform(b2P2Origin, 0);
	player2->getComponent<PhysicsComponent>()->setLinearVelocity({ 0,0 });
	player2->getComponent<PlayerController>()->resetInputs();

	soccerBall->setPosition(sbOuterOrigin);
	soccerBall->getComponent<PhysicsComponent>()->body->SetTransform(b2sbOuterOrigin, 0);
	soccerBall->getComponent<PhysicsComponent>()->setLinearVelocity({ 0,0 });
	soccerBall->getComponent<PhysicsComponent>()->body->SetAngularVelocity(0);

	soccerBallInner->setPosition(sbInnerOrigin);
	soccerBallInner->getComponent<PhysicsComponent>()->body->SetTransform(b2sbInnerOrigin, 0);
	soccerBallInner->getComponent<PhysicsComponent>()->setLinearVelocity({ 0,0 });
	soccerBallInner->getComponent<PhysicsComponent>()->body->SetAngularVelocity(0);
	soccerBallInner->getComponent<BallComponent>()->engageSlowmotion = false;
}

void RocketBall::initPhysics()
{
	float gravity = -9.8; // 9.8 m/s2
	delete world;
	world = new b2World(b2Vec2(0, gravity));
	world->SetContactListener(this);

	if (doDebugDraw) {
		world->SetDebugDraw(&debugDraw);
	}
}


const float RocketBall::getPhysicsScale() {
	return physicsScale;
}

/// Core Update
void RocketBall::update(float time) {
	if (gameState == GameState::Running || gameState == GameState::RoundComplete) {
		updatePhysics(time);
	}
	for (int i = 0; i < sceneObjects.size(); i++) {
		sceneObjects[i]->update(time);
	}


	InnerBallPhysics->body->SetTransform(OuterBallPhyiscs->body->GetPosition(), OuterBallPhyiscs->body->GetAngle());
	soccerBallInner->setPosition(soccerBall->getPosition());
}

GameState RocketBall::getGameState() {
	return gameState;
}

/// Region for Render engine
#pragma region Render
void RocketBall::render() {
	auto rp = RenderPass::create()
		.withCamera(camera->getCamera())
		.build();

	auto pos = camera->getGameObject()->getPosition();

	//Render the background Layer_1
	background_Layer_1.renderBackground(rp, +pos.x*0.8f);

	auto spriteBatchBuilder = SpriteBatch::create();
	for (auto & go : sceneObjects) {
		go->renderSprite(spriteBatchBuilder);
	}

	auto sb = spriteBatchBuilder.build();
	rp.draw(sb);

	ImGui::SetNextWindowPos(ImVec2(Renderer::instance->getWindowSize().x / 2 - 50, .0f), ImGuiSetCond_Always);
	ImGui::SetNextWindowSize(ImVec2(150, 75), ImGuiSetCond_Always);
	ImGui::Begin("", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
	ImGui::TextColored(ImVec4(player1Color.x, player1Color.y, player1Color.z, player1Color.w), "Goal P1 %i", player1Goals);
	ImGui::TextColored(ImVec4(player2Color.x, player2Color.y, player2Color.z, player2Color.w), "Goal P2 %i", player2Goals);
	ImGui::SetWindowFontScale(2.0f);
	ImGui::PushFont;
	ImGui::End();

	if (doDebugDraw) {
		static Profiler profiler;
		profiler.update();
		profiler.gui(/*usewindow=*/ true);
		world->DrawDebugData();
		rp.drawLines(debugDraw.getLines());
		debugDraw.clear();
		RenderSliders();
	}
}
#pragma endregion
void RocketBall::handleContact(b2Contact *contact, bool begin) {
	auto fixA = contact->GetFixtureA();
	auto fixB = contact->GetFixtureB();
	auto physAIter = physicsComponentLookup.find(fixA);
	auto physBIter = physicsComponentLookup.find(fixB);
	auto physA = physAIter == physicsComponentLookup.end() ? nullptr : (*physAIter).second;
	auto physB = physBIter == physicsComponentLookup.end() ? nullptr : (*physBIter).second;

	if (physAIter != physicsComponentLookup.end()) {

		auto & aComponents = physA->getGameObject()->getComponents();
		//std::cout << "fixA" << std::endl;
		for (auto & c : aComponents) {
			if (begin) {
				c->onCollisionStart(physB);
			}
			else {
				c->onCollisionEnd(physB);
			}
		}
	}
	if (physBIter != physicsComponentLookup.end()) {
		auto & bComponents = physB->getGameObject()->getComponents();
		//std::cout << "fixB" << std::endl;
		for (auto & c : bComponents) {
			if (begin) {
				c->onCollisionStart(physA);
			}
			else {
				c->onCollisionEnd(physA);
			}
		}
	}
}
//
//void RocketBall::handleContact(b2Contact *contact, bool begin) {
//	auto fixA = contact->GetFixtureA();
//	auto fixB = contact->GetFixtureB();
//	PhysicsComponent* physA = physicsComponentLookup[fixA];
//	PhysicsComponent* physB = physicsComponentLookup[fixB];
//	auto & aComponents = physA->getGameObject()->getComponents();
//	auto & bComponents = physB->getGameObject()->getComponents();
//	for (auto & c : aComponents) {
//		if (begin) {
//			c->onCollisionStart(physB);
//		}
//		else {
//			c->onCollisionEnd(physB);
//		}
//	}
//	for (auto & c : bComponents) {
//		if (begin) {
//			c->onCollisionStart(physA);
//		}
//		else {
//			c->onCollisionEnd(physA);
//		}
//	}
//}
//
void RocketBall::BeginContact(b2Contact *contact) {
	b2ContactListener::BeginContact(contact);
	handleContact(contact, true);
}

void RocketBall::EndContact(b2Contact *contact) {
	b2ContactListener::EndContact(contact);
	handleContact(contact, false);
}

#pragma region Handle_Inputs
void RocketBall::onJoyInput(SDL_Event &event)
{
	if (event.jdevice.which == 0)
	{
		player2->getComponent<PlayerController>()->onJoyInput(event);
	}
	else if (event.jdevice.which == 1)
	{
		player1->getComponent<PlayerController>()->onJoyInput(event);
	}
}
void RocketBall::onKey(SDL_Event &event) {
	for (auto & gameObject : sceneObjects) {
		for (auto & c : gameObject->getComponents()) {
			bool consumed = c->onKey(event);
			if (consumed) {
				return;
			}
		}
	}
	if (event.type == SDL_KEYDOWN) {
		switch (event.key.keysym.sym) {
		case SDLK_d:
			// press 'd' for physics debug
			doDebugDraw = !doDebugDraw;
			if (doDebugDraw) {
				world->SetDebugDraw(&debugDraw);
			}
			else {
				world->SetDebugDraw(nullptr);
			}
			break;
		case SDLK_r:
			initGame();
			break;
		case SDLK_SPACE:
			if (gameState == GameState::GameOver) {
				initGame();
				gameState = GameState::Ready;
			}
			else if (gameState == GameState::Ready) {
				gameState = GameState::Running;
			}
			else if (gameState == GameState::RoundComplete) {
				soccerBallInner->getComponent<BallComponent>()->goalAchieved = false;
				nextRound();
				gameState = GameState::Ready;
			}
			break;
		}
	}
}
#pragma endregion


std::shared_ptr<GameObject> RocketBall::createGameObject() {
	auto obj = shared_ptr<GameObject>(new GameObject());
	sceneObjects.push_back(obj);
	return obj;
}


#pragma region Physics
///Phyiscs
void RocketBall::registerPhysicsComponent(PhysicsComponent *r)
{
	physicsComponentLookup[r->fixture] = r;
}

void RocketBall::deregisterPhysicsComponent(PhysicsComponent *r) {
	auto iter = physicsComponentLookup.find(r->fixture);
	if (iter != physicsComponentLookup.end()) {
		physicsComponentLookup.erase(iter);
	}
	else {
		assert(false); // cannot find physics object
	}
}



void RocketBall::updatePhysics(float deltaTime)
{
	const float32 timeStep = deltaTime;
	const int positionIterations = 2;
	const int velocityIterations = 6;
	world->Step(timeStep, velocityIterations, positionIterations);

	for (auto phys : physicsComponentLookup) {
		if (phys.second->rbType == b2_staticBody) continue;
		auto position = phys.second->body->GetPosition();
		float angle = phys.second->body->GetAngle();
		auto gameObject = phys.second->getGameObject();
		gameObject->setPosition(glm::vec2(position.x*physicsScale, position.y*physicsScale));
		//COMENTED OUT BECAUSE IT WAS GIVING ME TROUBLE //// LUKA /////
		gameObject->setRotation(glm::degrees(angle));
	}
}

void RocketBall::UpdateWithNewPhysics()
{
	auto soccerBallPhysics = soccerBall->getComponent<PhysicsComponent>();
	soccerBallPhysics->fixture->SetFriction(ballFriction);
	soccerBallPhysics->fixture->SetRestitution(ballRestitution);
	soccerBallPhysics->fixture->SetDensity(ballDensity);
	soccerBallPhysics->body->ResetMassData();

	auto player1Physics = player1->getComponent<PhysicsComponent>();
	player1Physics->fixture->SetFriction(playerFriction);
	player1Physics->fixture->SetRestitution(playerRestitution);
	player1Physics->fixture->SetDensity(playerDensity);
	player1Physics->body->ResetMassData();

	auto player2Physics = player2->getComponent<PhysicsComponent>();
	player2Physics->fixture->SetFriction(playerFriction);
	player2Physics->fixture->SetRestitution(playerRestitution);
	player2Physics->fixture->SetDensity(playerDensity);
	player2Physics->body->ResetMassData();
}
#pragma endregion

#pragma region GUI_Section
void RocketBall::RenderSliders()
{
	bool open = true;
	ImGui::Begin("#TestLabel", &open, ImVec2(500, 100), 0, ImGuiWindowFlags_NoTitleBar);

	ImGui::LabelText("Variables", "BALL SETTINGS");
	ImGui::SetNextWindowPos(ImVec2(500, 100));
	ImGui::SliderFloat(": B_Restitution", &ballRestitution, 0.0f, 1.0f);
	ImGui::SliderFloat(": B_Friction", &ballFriction, 0.0f, 1.0f);
	ImGui::SliderFloat(": B_Density: ", &ballDensity, 0.01f, 1.0f);
	ImGui::LabelText("Variables", "PKAYER SETTINGS");
	ImGui::SetNextWindowPos(ImVec2(500, 100));
	ImGui::SliderFloat(": P_Restitution", &playerRestitution, 0.0f, 1.0f);
	ImGui::SliderFloat(": P_Friction", &playerFriction, 0.0f, 1.0f);
	ImGui::SliderFloat(": P_Density: ", &playerDensity, 0.01f, 1.0f);

	ImGui::End();
	UpdateWithNewPhysics();
}

#pragma endregion

void RocketBall::setGameState(GameState newState) {
	this->gameState = newState;
}