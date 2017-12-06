#pragma once

#include "sre/Sprite.hpp"
#include "SpriteComponent.hpp"
#include <Box2D/Box2D.h>
#include "Component.hpp"

class PlayerController : public Component, public b2RayCastCallback {
public:
	explicit PlayerController(GameObject *gameObject);

	bool onJoyInput(SDL_Event & event);

	float Remap(float value, float from1, float to1, float from2, float to2);

	void setSprite(sre::Sprite playerSprite);

	void updateSprite(float deltaTime);

	void resetJumps();

	void update(float deltaTime) override;

	void applyMovement();

	void stopFalling();

	void stopHorizontalMovement();

	void setRotation(float _rot);

	bool onKey(SDL_Event &event) override;

	void jump();

	void endDash();

	float angleBetweenVectors(glm::vec2 vec1, glm::vec2 vec2);

	//Raycasting
	virtual float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point,
		const b2Vec2& normal, float32 fraction) override;

	void onCollisionStart(PhysicsComponent *comp) override;

	void onCollisionEnd(PhysicsComponent *comp) override;


	glm::vec2 movementVector = glm::vec2(0, 0);
	glm::vec2 facingVector = glm::vec2(0, 0);

private:

	

	sre::Sprite playerSprite_1;

	std::shared_ptr<SpriteComponent> playerSprite;
	std::shared_ptr<PhysicsComponent> playerPhysics;

	//Player car Settings
	const float maxSpeed = 3.0f;
	const float acceleration = 100.0f;
	const float dashSpeed = 0.5f;
	const float dashDuration = 0.07f;
	const float boostSpeed = 1.5f;

	//UPDATE THINGS
	float nextFixedUpdate = 0.0f;
	float updateFrequency = 1.0f / 20.0f;

	int airDashesAvailable = 1;
	int airDashCounter = 0;

	int dashCounter = 0;
	
	bool inDash = false;

	bool isNextToWall = false;
	bool isGrounded = false;
	float timePassed = 0.0f;
	float rotation = 0.0f;
	float size;

	//INPUT VARIABLES
	bool isHoldingGas = false;
	bool isHoldingRevers = false;
	bool isBoosting = false;

	bool movingForward = false;
	bool movingBack = false;
};
