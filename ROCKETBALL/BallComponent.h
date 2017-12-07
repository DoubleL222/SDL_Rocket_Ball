#pragma once

#include "Component.hpp"


class BallComponent : public Component {
private:

	std::shared_ptr<PhysicsComponent> ballPhysics;
	glm::vec2 initialPosition;
	float blowBackForce;


	void update(float deltaTime) override;

public:
	explicit BallComponent(GameObject *gameObject);

	bool goalAchieved = false;

	bool engageSlowmotion = false;

	void setOuterBall(std::shared_ptr<PhysicsComponent> _outerBallPhysics);

	float totalTime = 0;
	float tIndex = 0;
	float slowMotionSet = 0.2f;
	float slowMotionReturn = 1.0f;

	void onCollisionStart(PhysicsComponent *comp) override;

	void onCollisionEnd(PhysicsComponent *comp) override;
};
