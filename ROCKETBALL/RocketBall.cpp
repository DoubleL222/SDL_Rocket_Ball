#include "RocketBall.h"
#include "Box2D/Dynamics/Contacts/b2Contact.h"
#include "GameObject.hpp"
#include "sre/RenderPass.hpp"
#include "PhysicsComponent.hpp"
#include "SpriteComponent.hpp"
#include "BallComponent.h"
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

	//MAKING THE SPRITE ATLAS
	mySpriteAtlas = SpriteAtlas::create("RocketBallSprites.json", "RocketBallSprites.png");

	if (world) {
		world->SetContactListener(nullptr);
	}

	//INITIALIZING PHYSICS
	initPhysics();

	//Setup Camera
	sceneObjects.clear();
	camera.reset();
	auto camGameObj = createGameObject();
	camGameObj->name = "Camera";
	camera = camGameObj->addComponent<GameCamera>();
	camGameObj->setPosition(windowSize*0.5f);

	//Set background if it has not yet been set
	if (!background_Layer_1.isInit) {
		background_Layer_1.init("skybackdrop.png", -windowSize.x*0.5f, -windowSize.y*0.5f, true);
	}

#pragma region Playing Field

	//Floor and Ceiling (issue: stretches the texture, maybe make into tiles instead)
	createStetchedStaticBox("floor", mySpriteAtlas->get("Grass.png"), createGameObject(), { 0,-windowSize.y * 0.5f }, true, false, { 1,1 }, { 0,-30.0 }, physicsScale);
	createStetchedStaticBox("ceiling", mySpriteAtlas->get("gray.png"), createGameObject(), { 0,windowSize.y * 0.5f }, true, false, { 0,0 }, { 0,0 }, physicsScale);
	createStetchedStaticBox("rightBound", mySpriteAtlas->get("gray.png"), createGameObject(), { windowSize.x * 0.5f,0 }, false, true, { 0,0 }, { 0,0 }, physicsScale);
	createStetchedStaticBox("leftBound", mySpriteAtlas->get("gray.png"), createGameObject(), { -windowSize.x * 0.5f,0 }, false, true, { 0,0 }, { 0,0 }, physicsScale);

	//Field Walls
	//createPlayFieldWall("leftwall_Bot", mySpriteAtlas->get("brickWall.png"), createGameObject(), true, false, { -windowSize.x * 0.5f , -windowSize.y * 0.5f }, { 80.0f, 4.0f }, physicsScale);
	//createPlayFieldWall("leftwall_Top", mySpriteAtlas->get("gray.png"), createGameObject(), false, false, { -windowSize.x * 0.5f , windowSize.y * 0.5f }, { 80.0f, 8.0f }, physicsScale);
	//createPlayFieldWall("rightwall_Bot", mySpriteAtlas->get("gray.png"), createGameObject(), true, true, { windowSize.x * 0.5f , -windowSize.y * 0.5f }, { 80.0f, 4.0f }, physicsScale);
	//createPlayFieldWall("rightwall_Top", mySpriteAtlas->get("gray.png"), createGameObject(), false, true, { windowSize.x * 0.5f , windowSize.y * 0.5f }, { 80.0f, 8.0f }, physicsScale);

	//Platforms
	createStaticBox("Platform_center", mySpriteAtlas->get("gray.png"), createGameObject(), { 0,0 }, { 2.0f, 0.2f }, { 0,0 }, physicsScale);
	createStaticBox("Platform_left", mySpriteAtlas->get("gray.png"), createGameObject(), { -windowSize.x * 0.2, -windowSize.y * 0.2 }, { 1.5f, 0.2f }, { 0,0 }, physicsScale);
	createStaticBox("Platform_right", mySpriteAtlas->get("gray.png"), createGameObject(), { windowSize.x * 0.2, -windowSize.y * 0.2 }, { 1.5f, 0.2f }, { 0,0 }, physicsScale);

#pragma endregion

#pragma region Goals
	//Debug center goal
	//createGoal("Goal_1", mySpriteAtlas->get("gray.png"), createGameObject(), { 0,-windowSize.y * 0.25 }, { 1 ,1 }, { 0,0 }, glm::vec4{ 1.0f, 0.3f, 0.3f, 0.8f }, physicsScale);
	
	//Real goals:
	createGoal("Goal_1", mySpriteAtlas->get("gray.png"), createGameObject(), { windowSize.x *0.5 - 70,-windowSize.y * 0.25 }, { 1 ,2 }, { 0,0 }, glm::vec4{ 1.0f, 0.3f, 0.3f, 0.8f }, physicsScale);
	createGoal("Goal_2", mySpriteAtlas->get("gray.png"), createGameObject(), { -windowSize.x *0.5 + 70,-windowSize.y * 0.25}, { 1 ,2 }, { 0,0 }, glm::vec4{ 0.3f, 0.3f, 1.0f, 0.8f }, physicsScale);
#pragma endregion


#pragma region Dynamic Elements

	//Spawn Soccer Ball
	soccerBall = createGameObject();
	soccerBall->name = "Ball";
	auto spriteComp = soccerBall->addComponent<SpriteComponent>();
	auto soccerBallSprite = mySpriteAtlas->get("SoccerBall.png");
	soccerBallSprite.setScale(glm::vec2(0.4f, 0.4f));
	spriteComp->setSprite(soccerBallSprite);
	soccerBall->setPosition(glm::vec2(0, windowSize.y * 0.3f));
	auto physComp = soccerBall->addComponent<PhysicsComponent>();
	physComp->initCircle(b2BodyType::b2_dynamicBody, 50 / physicsScale, soccerBall->getPosition() / physicsScale, ballDensity, ballFriction, ballRestitution, ballLinearDamping, ballAngularDamping, false);
	soccerBall->addComponent<BallComponent>();

	//Spawn Player1
	player1 = createGameObject();
	player1->addComponent<PlayerController>();
	spriteComp = player1->addComponent<SpriteComponent>();
	auto player1Sprite = mySpriteAtlas->get("ManUntd.png");
	player1Sprite.setScale(glm::vec2(0.2f, 0.2f));
	spriteComp->setSprite(player1Sprite);
	player1->setPosition(glm::vec2(windowSize.x * 0.2, -windowSize.y * 0.2 + (player1Sprite.getSpriteSize().y * 0.5f)));
	physComp = player1->addComponent<PhysicsComponent>();
	physComp->initCircle(b2BodyType::b2_dynamicBody, 20 / physicsScale, player1->getPosition() / physicsScale, playerDensity, playerFriction, playerRestitution, playerLinearDamping, playerAngularDamping, true);

	//Spawn Player2
	player2 = createGameObject();
	player2->addComponent<PlayerController>();
	spriteComp = player2->addComponent<SpriteComponent>();
	auto player2Sprite = mySpriteAtlas->get("FCBarcelona.png");
	player2Sprite.setScale(glm::vec2(0.2f, 0.2f));
	spriteComp->setSprite(player2Sprite);
	player2->setPosition(glm::vec2(-windowSize.x * 0.2, -windowSize.y * 0.2 + (player2Sprite.getSpriteSize().y * 0.5f)));
	physComp = player2->addComponent<PhysicsComponent>();
	physComp->initCircle(b2BodyType::b2_dynamicBody, 20 / physicsScale, player2->getPosition() / physicsScale, playerDensity, playerFriction, playerRestitution, playerLinearDamping, playerAngularDamping, true);

#pragma endregion

	//SET GAME STATE
	gameState = GameState::Ready;
}

#pragma region Playfield Creators
//Note: needs to be moved to own class
void RocketBall::createGoal(std::string name, sre::Sprite sprite, std::shared_ptr<GameObject> obj, glm::vec2 pos, glm::vec2 scale, glm::vec2 colBuffer, glm::vec4 color, const float phyScale) {
	auto goalSprite = sprite;
	goalSprite.setScale(scale);
	goalSprite.setColor(color);
	auto goal_obj = obj;
	goal_obj->name = name;
	auto goalSpriteComp = goal_obj->addComponent<SpriteComponent>();
	glm::vec2 pos_1{ pos };
	goal_obj->setPosition(pos_1);
	glm::vec2 scaleCol{ (goalSprite.getSpriteSize().x * goalSprite.getScale().x / 2) + colBuffer.x, (goalSprite.getSpriteSize().y * goalSprite.getScale().y / 2) + colBuffer.y };
	goalSpriteComp->setSprite(goalSprite);
	auto goalPhysics = goal_obj->addComponent<PhysicsComponent>();
	goalPhysics->initBox(b2_staticBody, scaleCol / physicsScale, { goal_obj->getPosition().x / physicsScale, goal_obj->getPosition().y / physicsScale }, 0);
	goalPhysics->setSensor(true);
}

void RocketBall::createStaticBox(std::string name, sre::Sprite sprite, std::shared_ptr<GameObject> obj, glm::vec2 pos, glm::vec2 scale, glm::vec2 colBuffer, const float phyScale) {
	auto box = sprite;
	box.setScale({ scale.x, scale.y });
	auto box_obj = obj;
	box_obj->name = name;
	auto boxSprite = box_obj->addComponent<SpriteComponent>();
	box_obj->setPosition(pos);
	glm::vec2 scaleCol{ (box.getSpriteSize().x * box.getScale().x / 2) + colBuffer.x, (box.getSpriteSize().y * box.getScale().y / 2) + colBuffer.y };
	boxSprite->setSprite(box);
	auto box_physics = box_obj->addComponent<PhysicsComponent>();
	box_physics->initBox(b2_staticBody, scaleCol / phyScale, { box_obj->getPosition().x / phyScale, box_obj->getPosition().y / phyScale }, 0);
}

void RocketBall::createStetchedStaticBox(std::string name, sre::Sprite sprite, std::shared_ptr<GameObject> obj, glm::vec2 pos, bool horizontalStretch, bool verticalStretch, glm::vec2 customScale, glm::vec2 colBuffer, float phyScale) {
	float scaleX, scaleY;
	auto stretchedBox = sprite;
	auto stretchedBox_obj = obj;
	stretchedBox_obj->name = name;
	auto stetchedBoxSprite = stretchedBox_obj->addComponent<SpriteComponent>();

	if (horizontalStretch) {
		scaleX = windowSize.x / stretchedBox.getSpriteSize().x;
	}
	else {
		scaleX = customScale.x;
	}
	if (verticalStretch) {
		scaleY = windowSize.y / stretchedBox.getSpriteSize().y;
	}
	else {
		scaleY = customScale.y;
	}
	stretchedBox.setScale({ scaleX, scaleY });
	stetchedBoxSprite->setSprite(stretchedBox);
	stretchedBox_obj->setPosition({ pos.x, pos.y /*+ stretchedBox.getSpriteSize().y *0.5f */ });
	glm::vec2 scaleCol{ (stretchedBox.getSpriteSize().x * stretchedBox.getScale().x / 2) + colBuffer.x, (stretchedBox.getSpriteSize().y * stretchedBox.getScale().y / 2) + colBuffer.y };
	auto stretchedBoxPhysics = stretchedBox_obj->addComponent<PhysicsComponent>();
	stretchedBoxPhysics->initBox(b2BodyType::b2_staticBody, scaleCol / phyScale, stretchedBox_obj->getPosition() / phyScale, 0.0f);
}

void RocketBall::createPlayFieldWall(std::string name, sre::Sprite sprite, std::shared_ptr<GameObject> obj, bool botWall, bool rightWall, glm::vec2 pos, glm::vec2 customScale, float phyScale) {

	auto wall = sprite;
	auto wall_obj = obj;
	wall_obj->name = name;
	auto wallSprite = wall_obj->addComponent<SpriteComponent>();

	wall.setScale(glm::vec2(windowSize.x / ((customScale.x / 2.0f)*(wall.getSpriteSize().x)), windowSize.y / ((customScale.y / 2.0f)*(wall.getSpriteSize().y))));
	wallSprite->setSprite(wall);
	if (!rightWall) {
		if (botWall) {
			wall_obj->setPosition({ pos.x + (windowSize.x / customScale.x), pos.y + (windowSize.y / customScale.y) });
		}
		else {
			wall_obj->setPosition({ pos.x + (windowSize.x / customScale.x), pos.y - (windowSize.y / customScale.y) });
		}
	}
	else {
		if (botWall) {
			wall_obj->setPosition({ pos.x - (windowSize.x / customScale.x), pos.y + (windowSize.y / customScale.y) });
		}
		else {
			wall_obj->setPosition({ pos.x - (windowSize.x / customScale.x), pos.y - (windowSize.y / customScale.y) });
		}
	}

	glm::vec2 scaleCol{ (wall.getSpriteSize().x * wall.getScale().x / 2), (wall.getSpriteSize().y * wall.getScale().y / 2) };
	auto wallPhysics = wall_obj->addComponent<PhysicsComponent>();
	wallPhysics->initBox(b2BodyType::b2_staticBody, scaleCol / phyScale, wall_obj->getPosition() / physicsScale, 0.0f);
}

#pragma endregion

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

/// Core Update
void RocketBall::update(float time) {
	if (gameState == GameState::Running || gameState == GameState::RoundComplete) {
		updatePhysics(time);
	}
	for (int i = 0; i < sceneObjects.size(); i++) {
		sceneObjects[i]->update(time);
	}
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
	ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "Goal P1 %i", player1Goals);
	ImGui::TextColored(ImVec4(0.3f, 0.3f, 1.0f, 1.0f), "Goal P2 %i", player2Goals);
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
	PhysicsComponent* physA = physicsComponentLookup[fixA];
	PhysicsComponent* physB = physicsComponentLookup[fixB];
	auto & aComponents = physA->getGameObject()->getComponents();
	auto & bComponents = physB->getGameObject()->getComponents();
	for (auto & c : aComponents) {
		if (begin) {
			c->onCollisionStart(physB);
		}
		else {
			c->onCollisionEnd(physB);
		}
	}
	for (auto & c : bComponents) {
		if (begin) {
			c->onCollisionStart(physA);
		}
		else {
			c->onCollisionEnd(physA);
		}
	}
}

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
				//call a reset function
				gameState = GameState::Running;
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
		gameObject->setRotation(angle);
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