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
	movementVector = glm::vec2(0,0);
	if (event.type == SDL_KEYDOWN) 
	{
		if (event.key.keysym.sym == SDLK_RIGHT) 
		{
			movementVector.x = 1;
		}
		else if (event.key.keysym.sym == SDLK_LEFT)
		{
			movementVector.x = -1;
		}
		if (event.key.keysym.sym == SDLK_SPACE) 
		{
			cout << "JUMP" << std::endl;
			jump();
		}
	}


	//switch (event.key.keysym.sym) {
	//case SDLK_SPACE:
	//{
	//	if (isGrounded && event.type == SDL_KEYDOWN) { // prevents double jump
	//		jump();
	//	}
	//}
	//break;
	//case SDLK_LEFT:
	//{
	//	movingLeft = event.type == SDL_KEYDOWN;
	//}
	//break;
	//case SDLK_RIGHT:
	//{
	//	cout << "Pressing Right" << std::endl;
	//	movingRight = event.type == SDL_KEYDOWN;
	//}
	//break;
	//}

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
	//RocketBall::gameInstance->world->RayCast(this, from, to);

	if (movementVector.x != 0) 
	{
		glm::vec2 currentVelocity = playerPhysics->getLinearVelocity();
		if ((currentVelocity.x < 0 && movementVector.x>0) || (currentVelocity.x > 0 && movementVector.x<0))
		{
			playerPhysics->setLinearVelocity(glm::vec2(0, currentVelocity.y));
		}

	}
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
	playerPhysics->addImpulse({ 0,0.1f});
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


