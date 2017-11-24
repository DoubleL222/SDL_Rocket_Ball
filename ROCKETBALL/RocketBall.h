#include "Box2D\Dynamics\b2World.h"
#include "sre/SDLRenderer.hpp"
#include "GameObject.hpp"
#include "sre/SpriteAtlas.hpp"
#include "Box2DDebugDraw.hpp"
#include "GameCamera.h"

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

	static const glm::vec2 windowSize;

	void BeginContact(b2Contact *contact) override;
	void EndContact(b2Contact *contact) override;

	static RocketBall* gameInstance;
	void setGameState(GameState newState);

private:
	sre::SDLRenderer r;

	Box2DDebugDraw debugDraw;

	const float physicsScale = 100;

	void initGame();

	void initPhysics();
	void updatePhysics();
	void handleContact(b2Contact *contact, bool begin);
	void registerPhysicsComponent(PhysicsComponent *r);
	void deregisterPhysicsComponent(PhysicsComponent *r);
	b2World * world = nullptr;
	std::map<b2Fixture*, PhysicsComponent *> physicsComponentLookup;
	Box2DDebugDraw debugDraw;
	bool doDebugDraw = false;

	std::shared_ptr<GameCamera> camera;

	void update(float time);
	void render();

	void onKey(SDL_Event &event);

	std::vector<std::shared_ptr<GameObject>> sceneObjects;

	friend class PhysicsComponent;
};