#include "RocketBall.h"
#include "Box2D/Dynamics/Contacts/b2Contact.h"
#include "GameObject.hpp"
#include "sre/RenderPass.hpp"
#include "PhysicsComponent.hpp"
#include "SpriteComponent.hpp"
#include "sre/profiler.hpp"
#include <iostream>


using namespace std;
using namespace sre;

const glm::vec2 RocketBall::windowSize(800, 600); // some size for the window

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

	//INITIALIZING PHYSICS
	initPhysics();

	if (world) {
		world->SetContactListener(nullptr);
	}

	//Setup Camera
	sceneObjects.clear();
	camera.reset();
	auto camGameObj = createGameObject();
	camGameObj->name = "Camera";
	camera = camGameObj->addComponent<GameCamera>();
	camGameObj->setPosition(windowSize*0.5f);

	//Set background if it has not yet been set
	if (!background_Layer_1.isInit) {
		//(image, initial position X, initial position Y)
		//
		//background_Layer_1.init("testimage.png", -windowSize.x*0.5f, -windowSize.y*0.5f, true);
	}

	//Spawning Floor
	auto floorGameObj = createGameObject();
	auto spriteComp = floorGameObj->addComponent<SpriteComponent>();
	auto floorSprite = mySpriteAtlas->get("gray.png");
	floorSprite.setScale(glm::vec2(windowSize.x/floorSprite.getSpriteSize().x, windowSize.y / ((floorHeight/2.0f )*(floorSprite.getSpriteSize().y))));
	spriteComp->setSprite(floorSprite);
	floorGameObj->setPosition({ 0, -windowSize.y/(2.0) + windowSize.y / floorHeight });
	auto physComp = floorGameObj->addComponent<PhysicsComponent>();
	physComp->initBox(b2BodyType::b2_staticBody,glm::vec2(windowSize.x/(physicsScale*2), windowSize.y/(physicsScale*floorHeight)),floorGameObj->getPosition()/physicsScale, 0.0f);
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
		//updatePhysics();
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

	ImGui::SetNextWindowPos(ImVec2(Renderer::instance->getWindowSize().x / 2 - 50, .0f), ImGuiSetCond_Always);
	ImGui::SetNextWindowSize(ImVec2(100, 50), ImGuiSetCond_Always);
	ImGui::Begin("", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
	ImGui::SetWindowFontScale(2.0f);
	ImGui::PushFont;
	ImGui::End();

}
#pragma endregion


#pragma region Handle_Inputs
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


#pragma endregion



void RocketBall::setGameState(GameState newState) {
	this->gameState = newState;
}