#pragma once

#include "sre/Sprite.hpp"
#include "SpriteComponent.hpp"
#include <Box2D/Box2D.h>
#include "Component.hpp"

class PlayerController : public Component, public b2RayCastCallback {
public:
	explicit PlayerController(GameObject *gameObject);

	bool onJoyInput(SDL_Event & event);

	void setSprite(sre::Sprite playerSprite);

	void updateSprite(float deltaTime);

	void resetJumps();

	void update(float deltaTime) override;

	bool onKey(SDL_Event &event) override;

	void jump();

	void endDash();

	//Raycasting
	virtual float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point,
		const b2Vec2& normal, float32 fraction) override;

	void onCollisionStart(PhysicsComponent *comp) override;

	void onCollisionEnd(PhysicsComponent *comp) override;

	glm::vec2 movementVector = glm::vec2(0, 0);

private:

	

	sre::Sprite playerSprite_1;

	std::shared_ptr<SpriteComponent> spriteComponent;
	std::shared_ptr<PhysicsComponent> playerPhysics;

	const float maxSpeed = 5.0f;
	const float acceleration = 1.0f;
	const float dashSpeed = 15.0f;
	const float dashDuration = 0.15f;

	int airDashesAvailable = 1;
	int airDashCounter = 0;

	float dashCounter = 0;
	
	bool inDash = false;

	bool isGrounded = false;
	float radius;

	bool movingLeft = false;
	bool movingRight = false;
};
