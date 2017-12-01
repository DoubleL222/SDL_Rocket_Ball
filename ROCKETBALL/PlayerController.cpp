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

bool PlayerController::onJoyInput(SDL_Event &event)
{
	//	movementVector = glm::vec2(0,0);
	if (RocketBall::gameInstance->getGameState() == GameState::Running) {
		if (event.type == SDL_JOYAXISMOTION)
		{
			int axisIndex = (int)(event.jaxis.axis);
			if (axisIndex == 0)
			{
				float newX = (float)(event.jaxis.value) / (float)(32767);

				//SENSITIVITY
				if (abs(newX) <= 0.2f)
				{
					movementVector.x = 0;
				}
				else
				{
					movementVector.x = newX;
				}

			}
			else if (axisIndex == 1)
			{
				float newY = -(float)(event.jaxis.value) / (float)(32767);

				//SENSITIVITY
				if (abs(newY) <= 0.2f)
				{
					movementVector.y = 0;
				}
				else
				{
					movementVector.y = newY;
				}
			}
		}
		if (event.type == SDL_JOYBUTTONDOWN)
		{
			if (event.jbutton.button == 0)
			{
				jump();
			}
		}
		return false;
	}
}


bool PlayerController::onKey(SDL_Event &event) {
	//	movementVector = glm::vec2(0,0);

	if (RocketBall::gameInstance->getGameState() == GameState::Running) {
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
				jump();
			}
			if (event.key.keysym.sym == SDLK_DOWN)
			{
				movementVector.y = -1;
			}
			else if (event.key.keysym.sym == SDLK_UP)
			{
				movementVector.y = 1;
			}
		}
		else if (event.type == SDL_KEYUP)
		{
			if (event.key.keysym.sym == SDLK_RIGHT && movementVector.x > 0)
			{
				movementVector.x = 0;
			}
			if (event.key.keysym.sym == SDLK_LEFT && movementVector.x < 0)
			{
				movementVector.x = 0;
			}
			if (event.key.keysym.sym == SDLK_UP || event.key.keysym.sym == SDLK_DOWN)
			{
				movementVector.y = 0;
			}
		}
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
	radius = playerPhysics->radius;
	b2Vec2 to{ from.x,from.y - radius*1.3f };
	isGrounded = false;
	RocketBall::gameInstance->world->RayCast(this, from, to);

	glm::vec2 currentVelocity = playerPhysics->getLinearVelocity();
	if (!inDash) {
		if (movementVector.x != 0)
		{
			float currentX = currentVelocity.x;
			//If player presses the opposite direction set horizontal speed to 0
			if ((currentVelocity.x < 0 && movementVector.x>0) || (currentVelocity.x > 0 && movementVector.x < 0))
			{
				currentX = 0;
				playerPhysics->setLinearVelocity(glm::vec2(currentX, currentVelocity.y));
			}
			//If Speed is more than maxSpeed
			if (abs(currentX) >= maxSpeed)
			{
				if (currentX < 0)
				{
					playerPhysics->setLinearVelocity(glm::vec2(-maxSpeed, currentVelocity.y));
				}
				else if (currentX > 0)
				{
					playerPhysics->setLinearVelocity(glm::vec2(maxSpeed, currentVelocity.y));
				}
			}
			//IF not, increase speed
			else
			{
				//currentX += movementVector.x*acceleration;
				playerPhysics->setLinearVelocity(glm::vec2(movementVector.x * maxSpeed, currentVelocity.y));
			}
		}
		else
		{
			playerPhysics->setLinearVelocity(glm::vec2(0, currentVelocity.y));
		}
	}
	//CHECK FOR DASH
	if (inDash)
	{
		dashCounter += deltaTime;
		if (dashCounter >= dashDuration)
		{
			inDash = false;
			endDash();
		}
	}
}

void PlayerController::jump()
{
	if (!isGrounded)
	{
		//IF player has already dashed mid jump
		if (airDashCounter >= airDashesAvailable)
		{
			return;
		}
		else
		{
			airDashCounter++;
		}
	}
	//glm::vec2 currentVelocity = playerPhysics->getLinearVelocity();
	//
	inDash = true;
	dashCounter = 0;
	cout << "moveVector: x: " << movementVector.x << "; y:" << movementVector.y << std::endl;

	//Check if direction vecor is zero, apply vertical jump
	bool fakeJump = false;
	if (movementVector.y == 0 && movementVector.x == 0)
	{
		fakeJump = true;
		movementVector.y = 1;
	}
	playerPhysics->setLinearVelocity(movementVector*dashSpeed);
	if (fakeJump)
	{
		movementVector.y = 0;
	}
	//playerPhysics->addImpulse(glm::vec2(0.0f, 5.0f));
}

void PlayerController::endDash()
{
	playerPhysics->setLinearVelocity(glm::vec2(0, 0));
}

void PlayerController::onCollisionStart(PhysicsComponent *comp) {

}

void PlayerController::onCollisionEnd(PhysicsComponent *comp) {

}

float32 PlayerController::ReportFixture(b2Fixture *fixture, const b2Vec2 &point, const b2Vec2 &normal, float32 fraction) {
	resetJumps();
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

void PlayerController::resetJumps()
{
	airDashCounter = 0;
}

