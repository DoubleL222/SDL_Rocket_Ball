#pragma once

#include "sre/Sprite.hpp"
#include "SpriteComponent.hpp"
#include <Box2D/Box2D.h>
#include "Component.hpp"

class PlayerController : public Component, public b2RayCastCallback {
public:
	explicit PlayerController(GameObject *gameObject);

	void setSprite(sre::Sprite playerSprite);

	void updateSprite(float deltaTime);

	void update(float deltaTime) override;

	bool onKey(SDL_Event &event) override;

	void jump();

	//Raycasting
	virtual float32 ReportFixture(b2Fixture* fixture, const b2Vec2& point,
		const b2Vec2& normal, float32 fraction) override;

	void onCollisionStart(PhysicsComponent *comp) override;

	void onCollisionEnd(PhysicsComponent *comp) override;

private:

	glm::vec2 movementVector;

	sre::Sprite playerSprite_1;

	std::shared_ptr<SpriteComponent> spriteComponent;
	std::shared_ptr<PhysicsComponent> playerPhysics;

	bool isGrounded = false;
	float radius;

	bool movingLeft = false;
	bool movingRight = false;
};
