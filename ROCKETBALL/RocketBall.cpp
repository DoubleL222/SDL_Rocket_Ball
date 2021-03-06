#include "RocketBall.h"
#include "Box2D/Dynamics/Contacts/b2Contact.h"
#include "GameObject.hpp"
#include "sre/RenderPass.hpp"
#include "PhysicsComponent.hpp"
#include "SetPlayfield.h"
#include "SpriteComponent.hpp"
#include "BallComponent.h"
#include "AbilityComponent.h"
#include "PlayerController.h"
#include "sre/profiler.hpp"
#include "SDL_mixer.h"
#include <windows.h>
#include <iostream>
#pragma comment(lib, "user32.lib") 

using namespace std;
using namespace sre;

//Detects system
#ifdef _WIN32
//Assign values if a windows machine
const glm::vec2 RocketBall::windowSize(GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
#else
//Otherwise use 1600x900
const glm::vec2 RocketBall::windowSize(1600, 900);
#   error "Unknown compiler"
#endif

RocketBall* RocketBall::gameInstance = nullptr;

RocketBall::RocketBall()
	:debugDraw(physicsScale)
{
	//create a singleton instance
	gameInstance = this;
	
	r.setWindowSize(windowSize);

	r.init(SDL_INIT_EVERYTHING, SDL_WINDOW_OPENGL);

	//Check to create audio; if failed, just return
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 4, 4096) == 1) {
		cout << "Error in creating audio" << endl;
		return;
	}

	//Initilize the whole game
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

	//Set background if it has not yet been set
	if (!background_Layer_1.isInit) {
		background_Layer_1.init("skybackdrop.png", -windowSize.x*0.5f, -windowSize.y*0.5f, true);
	}

	//Displays window for game parameters
	displayGameParameters = true;

	//Assign pointers to the sound files
	music = Mix_LoadMUS("music.ogg");
//	winnerSound = Mix_LoadWAV("gameOverSound.ogg");
	goalSound = Mix_LoadWAV("goalSound.ogg");
	pickUpSound = Mix_LoadWAV("pickUpSound.ogg");
	readySound = Mix_LoadWAV("readySound.ogg");
	goSound = Mix_LoadWAV("goSound.ogg");

	//Set volume of this one; values between 0 and 128
	pickUpSound->volume = 20;

	if (music != nullptr)
		Mix_PlayMusic(music, -1);
	Mix_VolumeMusic(64); //0 to 128 ( MIX_MAX_VOLUME ) 

	//Create a gameobject to hold the text
	textHolder = createGameObject();
	textHolder->name = "Text_Holder";
	//Add a sprite component
	auto textHolderSpriteComp = textHolder->addComponent<SpriteComponent>();
	//Assign sprites using the spriteatlas
	goalText = mySpriteAtlas->get("goal.png");
	winnerP1Text = mySpriteAtlas->get("WinnerP1.png");
	winnerP2Text = mySpriteAtlas->get("WinnerP2.png");
	readyText = mySpriteAtlas->get("ready.png");
	goText = mySpriteAtlas->get("go.png");
	//Alpha = 0; invis
	goalText.setColor({ 0.2f, 0.8f, 0.2f, 0.0f });
	goalText.setScale({ 1,1 });
	//Assign sprite to component
	textHolderSpriteComp->setSprite(goalText);
	//Set position based on screen resolution
	textHolder->setPosition({ 0, windowSize.y * 0.25f });

	//Bools to control game
	beginGame = false;
	gameOverBool = false;

	//Set gamemode before initilizing the playingfield
	gameModeClassic = true;

	goalSizes = glm::vec2{ 0.8f , 2.0f }; //Determines the size of the goal
	botYBoxSize = 0.0f; //Determines the position of the goal from the grass and up.

	//If the playfield has not been initilized (fail-safe)
	if (!setPlayField.playFieldInit) {
		//Use 3 or 5 boxes
		setPlayField.useFiveAbilityBoxes = false;
		//Set size for the goals
		setPlayField.setGoalSizes(goalSizes);
		//Init playing field
		setPlayField.createPlayField(mySpriteAtlas);
		setPlayField.readyAbilityBoxes(false); //make boxes invis
	}
#pragma region Dynamic GameObjects
	//Spawn Player1
	player1 = createGameObject(); //Create a new gameobject
	player1->name = "Player_1"; //assign name
	//Add player controller component
	std::shared_ptr<PlayerController> pc = player1->addComponent<PlayerController>(); 
	pc->setFacingDirection(false); //facing direction
	//Add sprite component
	auto spriteComp = player1->addComponent<SpriteComponent>();
	auto player1Sprite = mySpriteAtlas->get("car1.png"); //set player 1 sprite
	player1Sprite.setScale(glm::vec2(0.3f, 0.3f)); //set scale
	player1Sprite.setOrderInBatch(15); //Order in batch; determines rendering
	spriteComp->setSprite(player1Sprite); //Assign the sprite to sprite component
	//Set the position of the player; origin used to reset after each round
	P1Origin = glm::vec2(windowSize.x * 0.375, -windowSize.y * 0.2 + (player1Sprite.getSpriteSize().y * 0.5f));
	player1->setPosition(P1Origin);
	//Add a physics component and create a carCollider
	auto physComp = player1->addComponent<PhysicsComponent>();
	physComp->initCarCollider(glm::vec2(0.35f, 0.1f), player1->getPosition() / physicsScale, playerFriction, playerDensity, playerLinearDamping, playerAngularDamping);

	//Spawn Player2; same as player 1
	player2 = createGameObject();
	player2->name = "Player_2";
	pc = player2->addComponent<PlayerController>();
	pc->setFacingDirection(true);
	spriteComp = player2->addComponent<SpriteComponent>();
	auto player2Sprite = mySpriteAtlas->get("car2.png");
	player2Sprite.setFlip(glm::vec2(-1, 0));
	player2Sprite.setScale(glm::vec2(0.3f, 0.3f));
	player2Sprite.setOrderInBatch(15);
	spriteComp->setSprite(player2Sprite);
	P2Origin = glm::vec2(-windowSize.x * 0.375, -windowSize.y * 0.2 + (player2Sprite.getSpriteSize().y * 0.5f));
	player2->setPosition(P2Origin);
	physComp = player2->addComponent<PhysicsComponent>();
	physComp->initCarCollider(glm::vec2(0.35f, 0.1f), player2->getPosition() / physicsScale, playerFriction, playerDensity, playerLinearDamping, playerAngularDamping);

	//Spawn (outer) Soccer Ball w/e sprite
	//Same as with the players
	soccerBall = createGameObject();
	soccerBall->name = "OuterBall";
	spriteComp = soccerBall->addComponent<SpriteComponent>();
	auto soccerBallSprite = mySpriteAtlas->get("SoccerBall.png");
	soccerBallSprite.setScale(glm::vec2(0.4f, 0.4f));
	soccerBallSprite.setOrderInBatch(10);
	spriteComp->setSprite(soccerBallSprite);
	soccerBall->setPosition(glm::vec2(0, windowSize.y * 0.1f));
	OuterBallPhyiscs = soccerBall->addComponent<PhysicsComponent>();
	OuterBallPhyiscs->initCircle(b2BodyType::b2_dynamicBody, 50 / physicsScale, soccerBall->getPosition() / physicsScale, ballDensity, ballFriction, ballRestitution, ballLinearDamping, ballAngularDamping, false, SOCCERBALL, BOUNDARY | PLAYER | SOCCERBALL);
	b2sbOuterOrigin = OuterBallPhyiscs->body->GetPosition();
	sbOuterOrigin = soccerBall->getPosition();

	//Create an inner circle collider in the soccerball to score goals only 
	//when the ball is roughly 50% inside the goal (i.e. a very smaller cirlce collider inside the big)
	//The position of the inner collider is set in the update().
	//Inner ball is assigned the BallComponent which determines the ball's behaviour
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

	//SET INITIAL GAME STATE
	gameState = GameState::InitializeGame;
}

////////// Function to set the text and play the associated sound
void RocketBall::setTextAndPlaySound(int switchIndex) {
	switch (switchIndex) {
	case 0:
		//Set invisible.
		readyText.setColor({ 0.2f, 0.8f, 0.2f, 0.0f });
		textHolder->getComponent<SpriteComponent>()->setSprite(readyText);
		break;
	case 1:
		//ready
		if (readySound != nullptr) //Chekc if nullptr
			Mix_PlayChannel(-1, readySound, 0); //if not, play sound
		else {
			cout << "couldnt find sound" << endl;
		}

		readyText.setColor({ 0.2f, 0.8f, 0.2f, 1.0f }); //Set color 
		textHolder->getComponent<SpriteComponent>()->setSprite(readyText); //assign "updated" sprite
		break;
	case 2:
		//Goal
		if (goalSound != nullptr)
			Mix_PlayChannel(-1, goalSound, 0);
		else {
			cout << "couldnt find sound" << endl;
		}
		goalText.setColor({ 0.2f, 0.8f, 0.2f, 1.0f });
		textHolder->getComponent<SpriteComponent>()->setSprite(goalText);
		break;
	case 3:
		//GameOver
		gameOverBool = true;
		if (winnerSound != nullptr)
			Mix_PlayChannel(-1, winnerSound, 0);
		else {
			cout << "couldnt find sound" << endl;
		}

		Mix_VolumeMusic(32); //half original volume for the music

		//Check who has won, and assign correct sprite and color
		if (player1Goals > player2Goals) {

			winnerP1Text.setColor(player1Color);
			textHolder->getComponent<SpriteComponent>()->setSprite(winnerP1Text);
		}
		else {
			winnerP2Text.setColor(player2Color);
			textHolder->getComponent<SpriteComponent>()->setSprite(winnerP2Text);
		}
		break;
	case 4:
		//Go!
		if (goSound != nullptr)
			Mix_PlayChannel(-1, goSound, 0);
		else {
			cout << "couldnt find sound" << endl;
		}
		goText.setColor({ 0.2f, 0.8f, 0.2f, 1.0f });
		textHolder->getComponent<SpriteComponent>()->setSprite(goText);
		beginGame = true;
		break;
	}
}

//Public function to play sound when ability boxes are picked up
void RocketBall::playPickUp() {
	if (pickUpSound != nullptr)
		Mix_PlayChannel(-1, pickUpSound, 0);
	else {
		cout << "couldnt find sound" << endl;
	}
}

//Function to initilize a new round; resets the position, velocity, inputs etc. of dynamic objects
void RocketBall::nextRound() {
	player1->setPosition(P1Origin);
	glm::vec2 bodPos = P1Origin / physicsScale;
	player1->getComponent<PhysicsComponent>()->body->SetTransform(b2Vec2(bodPos.x, bodPos.y), 0);
	player1->getComponent<PhysicsComponent>()->setLinearVelocity({ 0,0 });;
	player1->getComponent<PlayerController>()->resetInputs();

	player2->setPosition(P2Origin);
	bodPos = P2Origin / physicsScale;
	player2->getComponent<PhysicsComponent>()->body->SetTransform(b2Vec2(bodPos.x, bodPos.y), 0);
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
	soccerBallInner->getComponent<BallComponent>()->engageSlowmotion = false; //Step out of slowmotion
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

//Get the phyiscsScale from other classes; public function
const float RocketBall::getPhysicsScale() {
	return physicsScale;
}

/// Core Update
void RocketBall::update(float time) {
	//If game is running, roundcomplete or gameover, update the phyiscs.
	if (gameState == GameState::Running || gameState == GameState::RoundComplete || gameState == GameState::GameOver) {
		updatePhysics(time);
	}

	//Call update using the time on all objects
	for (int i = 0; i < sceneObjects.size(); i++) {
		sceneObjects[i]->update(time);
	}
	
	//Set the innerball's position in the center of the outer ball on every update call
	InnerBallPhysics->body->SetTransform(OuterBallPhyiscs->body->GetPosition(), OuterBallPhyiscs->body->GetAngle());
	soccerBallInner->setPosition(soccerBall->getPosition());

	//Check if one of the players have reached the required amount of goals
	if ((player1Goals == goalsToScore || player2Goals == goalsToScore) && !gameOverBool) {
		gameState = GameState::GameOver;
		setTextAndPlaySound(3);
	}

	//An update check to remove the "Go" Text after 2 seconds
	if (beginGame && gameState != GameState::Ready && gameState != GameState::GameOver) {
		beginGameTime += time;
		if (beginGameTime > 2.0f) { //Afer 2 seconds...
			beginGame = false;
			beginGameTime = 0; //reset time
			setTextAndPlaySound(0); //play sound and set text "go"
		}
	}

}

//Get gamestate
GameState RocketBall::getGameState() {
	return gameState;
}

/// Render Update
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

	//Small window to display the player's score. 
	ImGui::SetNextWindowPos(ImVec2(Renderer::instance->getWindowSize().x / 2 - 50, .0f), ImGuiSetCond_Always);
	ImGui::SetNextWindowSize(ImVec2(150, 75), ImGuiSetCond_Always);
	ImGui::Begin("", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize);
	ImGui::TextColored(ImVec4(player1Color.x, player1Color.y, player1Color.z, player1Color.w), "Goal P1 %i", player1Goals);
	ImGui::TextColored(ImVec4(player2Color.x, player2Color.y, player2Color.z, player2Color.w), "Goal P2 %i", player2Goals);
	ImGui::SetWindowFontScale(2.0f);
	ImGui::PushFont;
	ImGui::End();

	//Debugging parameters
	if (doDebugDraw) {
		static Profiler profiler;
		profiler.update();
		profiler.gui(/*usewindow=*/ true);
		world->DrawDebugData();
		rp.drawLines(debugDraw.getLines());
		debugDraw.clear();
	}
	//Game Settings
	if (displayGameParameters) {
		RenderSliders();
	}
}
#pragma endregion

//Function to handle contacts.
void RocketBall::handleContact(b2Contact *contact, bool begin) {
	auto fixA = contact->GetFixtureA();
	auto fixB = contact->GetFixtureB();
	auto physAIter = physicsComponentLookup.find(fixA);
	auto physBIter = physicsComponentLookup.find(fixB);
	auto physA = physAIter == physicsComponentLookup.end() ? nullptr : (*physAIter).second;
	auto physB = physBIter == physicsComponentLookup.end() ? nullptr : (*physBIter).second;

	if (physAIter != physicsComponentLookup.end()) {
		auto & aComponents = physA->getGameObject()->getComponents();
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
	if (event.type == SDL_JOYBUTTONDOWN)
	{
		if (gameState == GameState::GameOver) {
			nextRound();
			soccerBallInner->getComponent<BallComponent>()->goalAchieved = false;
			player1Goals = 0;
			player2Goals = 0;
			displayGameParameters = true;
			setTextAndPlaySound(0);
			gameState = GameState::InitializeGame;
			return;
		}
		else if (gameState == GameState::Ready) {
			gameState = GameState::Running;
			player1->getComponent<PhysicsComponent>()->body->SetAwake(true);
			player2->getComponent<PhysicsComponent>()->body->SetAwake(true);
			soccerBall->getComponent<PhysicsComponent>()->body->SetAwake(true);
			setPlayField.readyAbilityBoxes(true);
			beginGame = true;
			Mix_VolumeMusic(64);
			setTextAndPlaySound(4);
			return;
		}
		else if (gameState == GameState::RoundComplete) {
			soccerBallInner->getComponent<BallComponent>()->goalAchieved = false;
			nextRound();
			gameState = GameState::Ready;
			setTextAndPlaySound(1);
			return;
		}
		else if (gameState == GameState::InitializeGame) {
			gameState = GameState::Ready;
			displayGameParameters = false;
			setTextAndPlaySound(1);
			return;
		}
	}
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
		case SDLK_h:
			// press 'h' for physics debug
			doDebugDraw = !doDebugDraw;
			if (doDebugDraw) {
				world->SetDebugDraw(&debugDraw);
			}
			else {
				world->SetDebugDraw(nullptr);
			}
			break;
		case SDLK_g:
			//press g to display game parameters
			displayGameParameters = !displayGameParameters;
			break;
		//During different gameStates, space will do different things...
		case SDLK_RSHIFT:
		case SDLK_LSHIFT:
		case SDLK_RCTRL:
		case SDLK_LCTRL:
		case SDLK_SPACE:
			//GameOver achieved upon reaching required goals
			if (gameState == GameState::GameOver) {
				//If game over, reset the game
				nextRound();
				soccerBallInner->getComponent<BallComponent>()->goalAchieved = false;
				player1Goals = 0;
				player2Goals = 0;
				displayGameParameters = true;
				setTextAndPlaySound(0); //make text invis
				gameState = GameState::InitializeGame; //Enter state to display parameters
			}
			else if (gameState == GameState::Ready) {
				//If ready to play, enter running state: game is now playing
				gameState = GameState::Running;
				//Set dynamic bodies awake to make the engage with phyiscs update
				player1->getComponent<PhysicsComponent>()->body->SetAwake(true);
				player2->getComponent<PhysicsComponent>()->body->SetAwake(true);
				soccerBall->getComponent<PhysicsComponent>()->body->SetAwake(true);
				setPlayField.readyAbilityBoxes(true); //Set the ability boxes ready
				beginGame = true; //being the game bool
				setTextAndPlaySound(4); //Set "Go" text
			}
			else if (gameState == GameState::RoundComplete) {
				//Enters after a goal has been achieved and player pressed Space
				soccerBallInner->getComponent<BallComponent>()->goalAchieved = false;
				nextRound(); //Reset game
				gameState = GameState::Ready; //get readyyyyy!
				setTextAndPlaySound(1); //show ready
			}
			else if (gameState == GameState::InitializeGame) {
				//When game begins or after a game has been concluded;
				gameState = GameState::Ready;
				displayGameParameters = false;
				gameOverBool = false;
				setTextAndPlaySound(1);
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

//USED FOR PHYSICS TINKERING
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
	ImGui::Begin("#GAME SETTINGS", &open, ImVec2(500, 100), 0.7f, ImGuiWindowFlags_NoTitleBar);
	ImGui::LabelText("", "GAME MODE");
	ImGui::SetNextWindowPos(ImVec2(500, 50));
	ImGui::Checkbox(": Classic GameMode", &gameModeClassic);
	ImGui::SliderInt(": Goals to Win", &goalsToScore, 1, 10);
/*//////PHYSICS TINKERING
	ImGui::LabelText("Variables", "BALL SETTINGS");
	ImGui::SetNextWindowPos(ImVec2(500, 100));
	ImGui::SliderFloat(": B_Restitution", &ballRestitution, 0.0f, 1.0f);
	ImGui::SliderFloat(": B_Friction", &ballFriction, 0.0f, 1.0f);
	ImGui::SliderFloat(": B_Density: ", &ballDensity, 0.01f, 1.0f);

	ImGui::LabelText("Variables", "PLAYER SETTINGS");
	ImGui::SetNextWindowPos(ImVec2(500, 100));
	ImGui::SliderFloat(": P_Restitution", &playerRestitution, 0.0f, 1.0f);
	ImGui::SliderFloat(": P_Friction", &playerFriction, 0.0f, 1.0f);
	ImGui::SliderFloat(": P_Density: ", &playerDensity, 0.01f, 1.0f);
	UpdateWithNewPhysics(); 
*/
	ImGui::End();

}

#pragma endregion

void RocketBall::setGameState(GameState newState) {
	this->gameState = newState;
}