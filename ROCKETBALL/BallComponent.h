#pragma once

#include "Component.hpp"


class BallComponent : public Component {
private:

	std::shared_ptr<PhysicsComponent> ballPhysics;
	glm::vec2 initialPosition;
	float blowBackForce;

	void update(float deltaTime) override;

	bool engageSlowmotion = false;


public:
	explicit BallComponent(GameObject *gameObject);

	bool goalAchieved = false;

	float totalTime = 0;
	float tIndex = 0;
	float slowMotionSet = 0.2f;
	float slowMotionReturn = 1.0f;


	void onCollisionStart(PhysicsComponent *comp) override;

	void onCollisionEnd(PhysicsComponent *comp) override;
};
