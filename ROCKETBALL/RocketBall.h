#include "Box2D\Dynamics\b2World.h"
#include "sre/SDLRenderer.hpp"
#include "GameObject.hpp"
#include "Box2DDebugDraw.hpp"

class RocketBall : public b2ContactListener 
{
public:
	RocketBall();
	std::shared_ptr<GameObject> createGameObject();

private:
	sre::SDLRenderer r;

	Box2DDebugDraw debugDraw;

	const float physicsScale = 100;

	void init();
	void initPhysics();
	void updatePhysics();
};