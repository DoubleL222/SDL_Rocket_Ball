#include "RocketBall.h"
#include "Box2D/Dynamics/Contacts/b2Contact.h"
#include "GameObject.hpp"
#include "sre/RenderPass.hpp"
#include "PhysicsComponent.hpp"
#include "SpriteComponent.hpp"
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
	r.keyEvent = [&](SDL_Event& e) {
		onKey(e);
	};
	r.frameUpdate = [&](float deltaTime) {
		update(deltaTime);
	};
	r.frameRender = [&]() {
		render();
	};

	// start game loop
	r.startEventLoop();
}

void RocketBall::initGame() {
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

#pragma region Playing Field
	//Spawning Floor
	auto floorGameObj = createGameObject();
	auto spriteComp = floorGameObj->addComponent<SpriteComponent>();
	auto floorSprite = mySpriteAtlas->get("gray.png");
	floorSprite.setScale(glm::vec2(windowSize.x / floorSprite.getSpriteSize().x, windowSize.y / ((floorHeight / 2.0f)*(floorSprite.getSpriteSize().y))));
	spriteComp->setSprite(floorSprite);
	floorGameObj->setPosition({ 0, -windowSize.y / (2.0) + windowSize.y / floorHeight });
	auto physComp = floorGameObj->addComponent<PhysicsComponent>();
	physComp->initBox(b2BodyType::b2_staticBody, glm::vec2(windowSize.x / (physicsScale * 2), windowSize.y / (physicsScale*floorHeight)), floorGameObj->getPosition() / physicsScale, 0.0f);

	//Spawning Ceiling
	auto ceilingGameObj = createGameObject();
	spriteComp = ceilingGameObj->addComponent<SpriteComponent>();
	floorSprite.setScale(glm::vec2(windowSize.x / floorSprite.getSpriteSize().x, windowSize.y / ((ceilingHeight / 2.0f)*(floorSprite.getSpriteSize().y))));
	spriteComp->setSprite(floorSprite);
	ceilingGameObj->setPosition({ 0, windowSize.y / (2.0) - windowSize.y / ceilingHeight });
	physComp = ceilingGameObj->addComponent<PhysicsComponent>();
	physComp->initBox(b2BodyType::b2_staticBody, glm::vec2(windowSize.x / (physicsScale * 2), windowSize.y / (physicsScale*ceilingHeight)), ceilingGameObj->getPosition() / physicsScale, 0.0f);

	//Spawning LeftWall
	auto leftWall = createGameObject();
	spriteComp = leftWall->addComponent<SpriteComponent>();
	floorSprite.setScale(glm::vec2(windowSize.x / ((wallWidth / 2.0f)*(floorSprite.getSpriteSize().x)), windowSize.y / floorSprite.getSpriteSize().y));
	spriteComp->setSprite(floorSprite);
	leftWall->setPosition({ windowSize.x / (2.0) - windowSize.x / wallWidth,0 });
	physComp = leftWall->addComponent<PhysicsComponent>();
	physComp->initBox(b2BodyType::b2_staticBody, glm::vec2(windowSize.x / (physicsScale*wallWidth), windowSize.y / (physicsScale * 2)), leftWall->getPosition() / physicsScale, 0.0f);

	//Spawning RightWall
	auto rightWall = createGameObject();
	spriteComp = rightWall->addComponent<SpriteComponent>();
	floorSprite.setScale(glm::vec2(windowSize.x / ((wallWidth / 2.0f)*(floorSprite.getSpriteSize().x)), windowSize.y / floorSprite.getSpriteSize().y));
	spriteComp->setSprite(floorSprite);
	rightWall->setPosition({ -windowSize.x / (2.0) + windowSize.x / wallWidth,0 });
	physComp = rightWall->addComponent<PhysicsComponent>();
	physComp->initBox(b2BodyType::b2_staticBody, glm::vec2(windowSize.x / (physicsScale*wallWidth), windowSize.y / (physicsScale * 2)), rightWall->getPosition() / physicsScale, 0.0f);

	//Set background if it has not yet been set
	if (!background_Layer_1.isInit) {
		//(image, initial position X, initial position Y)
		//background_Layer_1.init("backdropImage.jpg", -windowSize.x*0.5f, -windowSize.y*0.5f, true);
	}

#pragma endregion

#pragma region Dynamic Elements

	//Spawn Soccer Ball
	soccerBall = createGameObject();
	spriteComp = soccerBall->addComponent<SpriteComponent>();
	auto soccerBallSprite = mySpriteAtlas->get("SoccerBall.png");
	soccerBallSprite.setScale(glm::vec2(0.4f, 0.4f));
	spriteComp->setSprite(soccerBallSprite);
	soccerBall->setPosition(glm::vec2(0, 0));
	physComp = soccerBall->addComponent<PhysicsComponent>();
	physComp->initCircle(b2BodyType::b2_dynamicBody, 50 / physicsScale, soccerBall->getPosition() / physicsScale, ballDensity, ballFriction, ballRestitution, ballMass);

	//Spawn Player1
	player1 = createGameObject();
	spriteComp = player1->addComponent<SpriteComponent>();
	auto player1Sprite = mySpriteAtlas->get("ManUntd.png");
	player1Sprite.setScale(glm::vec2(0.2f, 0.2f));
	spriteComp->setSprite(player1Sprite);
	player1->setPosition(glm::vec2(windowSize.x / 4, 0));
	physComp = player1->addComponent<PhysicsComponent>();
	physComp->initCircle(b2BodyType::b2_dynamicBody, 20 / physicsScale, player1->getPosition() / physicsScale, playerDensity, playerFriction, playerRestitution, playerMass);

	//Spawn Player2
	player2 = createGameObject();
	player2->addComponent<PlayerController>();
	spriteComp = player2->addComponent<SpriteComponent>();
	auto player2Sprite = mySpriteAtlas->get("FCBarcelona.png");
	player2Sprite.setScale(glm::vec2(0.2f, 0.2f));
	spriteComp->setSprite(player2Sprite);
	player2->setPosition(glm::vec2(-windowSize.x / 4, 0));
	physComp = player2->addComponent<PhysicsComponent>();
	physComp->initCircle(b2BodyType::b2_dynamicBody, 20 / physicsScale, player2->getPosition() / physicsScale, playerDensity, playerFriction, playerRestitution, playerMass);

#pragma endregion

	//SET GAME STATE
	gameState = GameState::Running;
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

/// Core Update
void RocketBall::update(float time) {
	if (gameState == GameState::Running) {
		updatePhysics(time);
	}
	for (int i = 0; i < sceneObjects.size(); i++) {
		sceneObjects[i]->update(time);
	}
}

/// Region for Render engine
#pragma region Render
void RocketBall::render() {
	auto rp = RenderPass::create()
		.withCamera(camera->getCamera())
		.build();

	static Profiler profiler;
	profiler.update();
	profiler.gui(/*usewindow=*/ true);

	auto pos = camera->getGameObject()->getPosition();

	//Render the background Layer_1
	background_Layer_1.renderBackground(rp, +pos.x*0.8f);

	auto spriteBatchBuilder = SpriteBatch::create();
	for (auto & go : sceneObjects) {
		go->renderSprite(spriteBatchBuilder);
	}

	auto sb = spriteBatchBuilder.build();
	rp.draw(sb);

	if (doDebugDraw) {
		world->DrawDebugData();
		rp.drawLines(debugDraw.getLines());
		debugDraw.clear();
	}

	RenderSliders();
	//ImGui::SetNextWindowPos(ImVec2(Renderer::instance->getWindowSize().x / 2 - 50, .0f), ImGuiSetCond_Always);
	//ImGui::SetNextWindowSize(ImVec2(100, 50), ImGuiSetCond_Always);
	//ImGui::Begin("", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
	//ImGui::SetWindowFontScale(2.0f);
	//ImGui::PushFont;
	//ImGui::End();

}
#pragma endregion


#pragma region Handle_Inputs
void RocketBall::onKey(SDL_Event &event) {
	for (auto & gameObject : sceneObjects) {
		for (auto & c : gameObject->getComponents()) {
			bool consumed = c->onKey(event);
			if (consumed) {
				//return;
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
///Place phyiscs
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
	soccerBallPhysics->myData.mass = ballMass;

	auto player1Physics = player1->getComponent<PhysicsComponent>();
	player1Physics->fixture->SetFriction(playerFriction);
	player1Physics->fixture->SetRestitution(playerRestitution);
	player1Physics->fixture->SetDensity(playerDensity);
	player1Physics->myData.mass = playerMass;

	auto player2Physics = player2->getComponent<PhysicsComponent>();
	player2Physics->fixture->SetFriction(playerFriction);
	player2Physics->fixture->SetRestitution(playerRestitution);
	player2Physics->fixture->SetDensity(playerDensity);
	player2Physics->myData.mass = playerMass;

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
	ImGui::SliderFloat(": B_Mass: ", &ballMass, 0.01f, 20.0f);
	ImGui::SliderFloat(": B_Density: ", &ballDensity, 0.0f, 1.0f);
	ImGui::LabelText("Variables", "PKAYER SETTINGS");
	ImGui::SetNextWindowPos(ImVec2(500, 100));
	ImGui::SliderFloat(": P_Restitution", &playerRestitution, 0.0f, 1.0f);
	ImGui::SliderFloat(": P_Friction", &playerFriction, 0.0f, 1.0f);
	ImGui::SliderFloat(": P_Mass: ", &playerMass, 0.01f, 20.0f);
	ImGui::SliderFloat(": P_Density: ", &playerDensity, 0.0f, 1.0f);
	ImGui::End();
	UpdateWithNewPhysics();

}

#pragma endregion

void RocketBall::setGameState(GameState newState) {
	this->gameState = newState;
}