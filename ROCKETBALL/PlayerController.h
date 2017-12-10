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

	void setDelayedRotation(float _rot);

	float lerp(float a, float b, float f);

	bool onKey(SDL_Event &event) override;

	float clerp(float start, float end, float value);

	void jump();

	void resetInputs();

	void endDash();

	float angleBetweenVectors(glm::vec2 vec1, glm::vec2 vec2);

	//SetDirection
	void setFacingDirection(bool _facingRight);

	//Raycasting
	virtual float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point,
		const b2Vec2& normal, float32 fraction) override;

	void onCollisionStart(PhysicsComponent *comp) override;

	void onCollisionEnd(PhysicsComponent *comp) override;

	//PICKIN UP BOOST (BETWWEN 0 and 1)
	void rechargeBoost(float _val);

	//POWERUP SETTING FUNCTIONS
	void gravityPowerUp();
	void speedPowerUp();
	void dashPowerUp();
	void dashCountPowerUp();


	glm::vec2 movementVector = glm::vec2(0, 0);
	glm::vec2 facingVector = glm::vec2(0, 0);

private:



	sre::Sprite playerSprite_1;

	std::shared_ptr<SpriteComponent> playerSprite;
	std::shared_ptr<PhysicsComponent> playerPhysics;

	std::shared_ptr<GameObject> boostDisplay, boostBorder, gravityObj, speedObj, dashBoostObj, dashExtraObj, dashInfiniteObj;
	sre::Sprite boostBox, boostBorderBox, gravityIcon, speedIcon, dashBoostIcon, dashExtraIcon, dashInfiniteIcon;
	sre::Sprite& boostSprite = boostBox;


	//Player car Settings
	const float maxSpeed = 5.0f;
	const float acceleration = 10.0f;
	const float dashSpeed = 0.4f;
	const float dashDuration = 0.5f;
	const float rotationPerSecond = 360.0f*(1.0f / dashDuration);

	//BoostSetting
	const float maxSpeedWhenBoosting = 10.0f;
	const float bostAccaleration = 600.0f;
	const float boostBurnPerSecond = 0.5;
	const float maxBoost = 1.0f;

	//boost variables
	float currBoost = 1.0f;


	//On right or left?
	bool facingRight = false;

	//UPDATE THINGS
	float nextFixedUpdate = 0.0f;
	float updateFrequency = 1.0f / 20.0f;

	int airDashesAvailable = 1;
	int airDashCounter = 0;

	float dashCounter = 0;

	bool dashingForward = true;
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

	//Keyboard rotations
	bool rotatingDown = false;
	bool rotatingUp = false;

};
