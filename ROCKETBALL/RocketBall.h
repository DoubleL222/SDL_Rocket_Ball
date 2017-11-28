#include "Box2D\Dynamics\b2World.h"
#include "sre/SDLRenderer.hpp"
#include "sre/imgui_sre.hpp"
#include "sre/SpriteAtlas.hpp"
#include "Box2DDebugDraw.hpp"
#include "BackgroundComponent.hpp"
#include "GameObject.hpp"
#include "GameCamera.h"
#include <vector>

class PhysicsComponent;

enum class GameState {
	Ready,
	Running,
	GameOver
};


class RocketBall : public b2ContactListener
{
public:
	RocketBall();

	std::shared_ptr<GameObject> createGameObject();

	void registerPhysicsComponent(PhysicsComponent * r);

	void updatePhysics(float _deltaTime);

	void UpdateWithNewPhysics();

	void RenderSliders();

	static const glm::vec2 windowSize;

	//void BeginContact(b2Contact *contact) override;
	//void EndContact(b2Contact *contact) override;

	static RocketBall* gameInstance;
	void setGameState(GameState newState);
	
	b2World * world = nullptr;

private:

	std::shared_ptr<GameObject> soccerBall;
	std::shared_ptr<GameObject> player1;
	std::shared_ptr<GameObject> player2;

	float ballRestitution = 0.7f;
	float ballFriction = 1.0f;
	float ballDensity = 0.05f;
	float ballLinearDamping = 0.0f;
	float ballAngularDamping = 0.01f;

	float playerRestitution = 0.0f;
	float playerFriction = 1.0f;
	float playerDensity = 0.7f;
	float playerLinearDamping = 0.0f;
	float playerAngularDamping = 0.01f;

	sre::SDLRenderer r;

	const float floorHeight = 25;
	const float ceilingHeight = 40;
	const float wallWidth = 80;

	void initGame();

	void initPhysics();
	//void updatePhysics();
	//void handleContact(b2Contact *contact, bool begin);
	//void registerPhysicsComponent(PhysicsComponent *r);
	//void deregisterPhysicsComponent(PhysicsComponent *r);

	std::shared_ptr<sre::SpriteAtlas> mySpriteAtlas;
	std::map<b2Fixture*, PhysicsComponent *> physicsComponentLookup;

	Box2DDebugDraw debugDraw;
	bool doDebugDraw = true;

	const float physicsScale = 100;

	std::shared_ptr<GameCamera> camera;

	void update(float time);

	void render();

	void onKey(SDL_Event &event);

	std::vector<std::shared_ptr<GameObject>> sceneObjects;

	BackgroundComponent background_Layer_1;

	GameState gameState = GameState::Ready;

	friend class PhysicsComponent;

};