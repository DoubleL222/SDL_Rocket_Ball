#include "Box2D\Dynamics\b2World.h"
#include "sre/SDLRenderer.hpp"
#include "sre/imgui_sre.hpp"
#include "sre/SpriteAtlas.hpp"
#include "Box2DDebugDraw.hpp"
#include "BackgroundComponent.hpp"
#include "GameObject.hpp"
#include "SetPlayfield.h"
#include "GameCamera.h"
#include <vector>

class PhysicsComponent;

enum class GameState {
	Ready,
	Running,
	RoundComplete,
	GameOver
};


class RocketBall : public b2ContactListener
{
public:
	RocketBall();

	std::shared_ptr<GameObject> createGameObject();

	void registerPhysicsComponent(PhysicsComponent * r);
	void deregisterPhysicsComponent(PhysicsComponent *r);

	void updatePhysics(float _deltaTime);

	void UpdateWithNewPhysics();

	void RenderSliders();

	//CALCULATE DISTANCE TO PLAYER
	void applyForcesToPlayersAfterGoal();

	static const glm::vec2 windowSize;

	float timeScale = 1.0f;


	int player1Goals = 0;
	int player2Goals = 0;
	glm::vec4 player1Color{ 1.0f, 0.2f, 0.2f, 1.0f };
	glm::vec4 player2Color{ 0.2f, 0.2f, 1.0f, 1.0f };

	//Collision Filtering
	enum _entityCategory {
		BOUNDARY = 0x0001,
		PLAYER = 0x0002,
		SOCCERBALL = 0x0004,
		ABILITYBOX = 0x0008,
		ABILITYBOXUSED = 0x0010,
	};

	void nextRound();

	virtual void BeginContact(b2Contact *contact) override;
	virtual void EndContact(b2Contact *contact) override;

	std::shared_ptr<sre::SpriteAtlas> mySpriteAtlas;

	const float getPhysicsScale();

	static RocketBall* gameInstance;
	void setGameState(GameState newState);
	GameState getGameState();

	b2World * world = nullptr;

private:

	std::shared_ptr<GameObject> soccerBall, soccerBallInner;
	std::shared_ptr<GameObject> player1;
	std::shared_ptr<GameObject> player2;
	std::shared_ptr<GameObject> abilityBoxes;
	std::shared_ptr<PhysicsComponent> OuterBallPhyiscs, InnerBallPhysics;
	b2Vec2 b2P1Origin, b2P2Origin, b2sbOuterOrigin, b2sbInnerOrigin;
	glm::vec2 P1Origin, P2Origin, sbOuterOrigin, sbInnerOrigin;


	SDL_Joystick * joy1;
	SDL_Joystick * joy2;

	//Ball
	float ballRestitution = 0.6f;
	float ballFriction = 0.3f;
	float ballDensity = 0.04f;
	float ballLinearDamping = 0.00f;
	float ballAngularDamping = 0.00f;

	//Players
	float playerRestitution = 0.0f;
	float playerFriction = 0.3f;
	float playerDensity = 0.9f;
	float playerLinearDamping = 0.05f;
	float playerAngularDamping = 0.01f;

	//Playfield
	glm::vec2 goalSizes{ 0.7, 2.0f };
	SetPlayfield setPlayField;

	sre::SDLRenderer r;

	bool usePlatforms = false;

	void initGame();

	void initPhysics();
	void handleContact(b2Contact *contact, bool begin);

	//std::shared_ptr<sre::SpriteAtlas> mySpriteAtlas;
	std::map<b2Fixture*, PhysicsComponent *> physicsComponentLookup;

	const float physicsScale = 100;

	Box2DDebugDraw debugDraw;
	bool doDebugDraw = false;

	std::shared_ptr<GameCamera> camera;

	void createAbilityBox(std::string name, sre::Sprite sprite, std::shared_ptr<GameObject> obj, glm::vec2 pos, glm::vec2 scale, glm::vec2 colBuffer, const float phyScale);

	void update(float time);

	void render();

	void onJoyInput(SDL_Event & event);

	void onKey(SDL_Event &event);

	std::vector<std::shared_ptr<GameObject>> sceneObjects;

	BackgroundComponent background_Layer_1;

	GameState gameState = GameState::Ready;

	friend class PhysicsComponent;

};