#include <SDL_events.h>
#include "PlayerController.h"
#include "GameObject.hpp"
#include "SpriteComponent.hpp"
#include "PhysicsComponent.hpp"
#include "RocketBall.h"
#include "SpriteComponent.hpp"
#include <iostream>

using namespace std;

PlayerController::PlayerController(GameObject *gameObject) : Component(gameObject) {


}

bool PlayerController::onKey(SDL_Event &event) {
	movingLeft = false;
	movingRight = false;
	switch (event.key.keysym.sym) {
	case SDLK_SPACE:
	{
		if (isGrounded && event.type == SDL_KEYDOWN) { // prevents double jump
			jump();
		}
	}
	break;
	case SDLK_LEFT:
	{
		movingLeft = event.type == SDL_KEYDOWN;
	}
	break;
	case SDLK_RIGHT:
	{
		movingRight = event.type == SDL_KEYDOWN;
	}
	break;
	}

	return false;
}

void PlayerController::update(float deltaTime) {
	// raycast ignores any shape in the starting point
	if (playerPhysics == nullptr) 
	{
		playerPhysics = gameObject->getComponent<PhysicsComponent>();
	}
	auto from = playerPhysics->body->GetWorldCenter();
	b2Vec2 to{ from.x,from.y - radius*1.3f };
	isGrounded = false;
	RocketBall::gameInstance->world->RayCast(this, from, to);

	//playerPhysics->fixture->GetBody()->SetFixedRotation(true);
	glm::vec2 movement{ 0,0 };

	if (movingLeft) {
		movement.x--;
	}
	if (movingRight) {
		movement.x++;
	}

	float accelerationSpeed = 0.1f;
	playerPhysics->addImpulse(movement*accelerationSpeed);
	float maximumVelocity = 2;
	auto linearVelocity = playerPhysics->getLinearVelocity();
	float currentVelocity = linearVelocity.x;

	if (abs(currentVelocity) > maximumVelocity) {
		linearVelocity.x = glm::sign(linearVelocity.x)*maximumVelocity;
		playerPhysics->setLinearVelocity(linearVelocity);
	}

	//updateSprite(deltaTime);
}

void PlayerController::jump() {
	playerPhysics->addImpulse({ 0,0.15f });
}

void PlayerController::onCollisionStart(PhysicsComponent *comp) {

}

void PlayerController::onCollisionEnd(PhysicsComponent *comp) {

}

float32 PlayerController::ReportFixture(b2Fixture *fixture, const b2Vec2 &point, const b2Vec2 &normal, float32 fraction) {
	isGrounded = true;
	return 0; // terminate raycast
}

void PlayerController::setSprite(sre::Sprite playerSprite_1) {
	this->playerSprite_1 = playerSprite_1;
}

void PlayerController::updateSprite(float deltaTime) {
	auto velocity = playerPhysics->getLinearVelocity();
	spriteComponent->setSprite(playerSprite_1);
}


