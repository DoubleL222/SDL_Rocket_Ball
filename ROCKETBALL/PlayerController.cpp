#include <SDL_events.h>
#include "PlayerController.h"
#include "GameObject.hpp"
#include "SpriteComponent.hpp"
#include "PhysicsComponent.hpp"
#include "RocketBall.h"
#include "SpriteComponent.hpp"
#include <iostream>
#include <glm\gtx\vector_angle.hpp>

using namespace std;

PlayerController::PlayerController(GameObject *gameObject) : Component(gameObject)
{

}

bool PlayerController::onJoyInput(SDL_Event &event)
{
	if (RocketBall::gameInstance->getGameState() == GameState::Running) {
		if (event.type == SDL_JOYAXISMOTION)
		{
			int axisIndex = (int)(event.jaxis.axis);
			if (axisIndex == 0)
			{
				float newX = (float)(event.jaxis.value) / (float)(32767);
				//SENSITIVITY
				if (abs(newX) < 0.1f) 
				{
					newX = 0;
				}

				//SETTING PROPER DIRECTION
				if (!facingRight)
				{
					newX = -newX;
				}
				facingVector.x = newX;
			}
			else if (axisIndex == 1)
			{
				float newY = -(float)(event.jaxis.value) / (float)(32767);
				//SENSITIVITY
				if (abs(newY) < 0.1f)
				{
					newY = 0;
				}

				//SETTING PROPER DIRECTION
				if (!facingRight)
				{
					newY = -newY;
				}
				facingVector.y = newY;
			}
			if (axisIndex == 5) 
			{
				float newVal = Remap(event.jaxis.value, -32768, 32767, 0, 1);

				//SETTING PROPER DIRECTION
				if (!facingRight) 
				{
					newVal = -newVal;
				}
				movementVector.x = newVal;
			}
			if (axisIndex == 2)
			{
				float newVal = Remap(event.jaxis.value, -32768, 32767, 0, 1);

				//SETTING PROPER DIRECTION
				if (!facingRight)
				{
					newVal = -newVal;
				}
				movementVector.x = -newVal;
			}
		}
		if (event.type == SDL_JOYBUTTONDOWN)
		{
			//cout << "Joy Button: " << (int)(event.jbutton.button) << std::endl;
			if (event.jbutton.button == 0)
			{
				jump();
			}
			if (event.jbutton.button == 1) 
			{
				isBoosting = true;
			}
		}
		else if (event.type == SDL_JOYBUTTONUP) 
		{
			if (event.jbutton.button == 1)
			{
				isBoosting = false;
			}
		}
		return false;
	}
}

float PlayerController::Remap(float value, float from1, float to1, float from2, float to2)
{
	return (value - from1) / (to1 - from1) * (to2 - from2) + from2;
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
				rotatingDown = true;
			}
			else if (event.key.keysym.sym == SDLK_UP)
			{
				rotatingUp = true;
			}
			if (event.key.keysym.sym == SDLK_RSHIFT) 
			{
				isBoosting = true;
			}
			if (event.key.keysym.sym == SDLK_r)
			{
				playerPhysics->body->SetAwake(true);
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
			if (event.key.keysym.sym == SDLK_DOWN)
			{
				rotatingDown = false;
			}
			else if (event.key.keysym.sym == SDLK_UP)
			{
				rotatingUp = false;
			}
			if (event.key.keysym.sym == SDLK_RSHIFT)
			{
				isBoosting = false;
			}
		}
	}

	return false;
}

void PlayerController::update(float deltaTime)
{
	timePassed += deltaTime;


	// raycast ignores any shape in the starting point
	if (playerPhysics == nullptr)
	{
		playerPhysics = gameObject->getComponent<PhysicsComponent>();
	}
	if (playerSprite == nullptr)
	{
		playerSprite = gameObject->getComponent<SpriteComponent>();
	}
	//playerPhysics->body->SetTransform(playerPhysics->body->GetPosition(), timePassed / 10);

	auto from = playerPhysics->body->GetWorldCenter();
	//radius = playerPhysics->radius;

	//cout << "0: UPDATE" <<std::endl;

	glm::vec2 currentVelocity = playerPhysics->getLinearVelocity();

	//RAYCASTS
	b2Vec2 to{ from.x + currentVelocity.x*0.1f,from.y + currentVelocity.y*0.1f };
	b2Vec2 toGround{ from.x,from.y - 0.3f };
	isGrounded = false;
	if (from != toGround) {
		RocketBall::gameInstance->world->RayCast(this, from, toGround);
	}

	//RAYCAST INTO VELOCITY
	//if (from != to)
	//{
	//	//RocketBall::gameInstance->world->RayCast(this, from, to);
	//}


	//FIXED UPDATE

	//if (timePassed > nextFixedUpdate) {
	//	nextFixedUpdate += updateFrequency;
	//	//cout << "1: FIXED UPDATE" << std::endl;
	//	if (!isGrounded)
	//	{
			//APPLY GRAVITY
	//		//cout << "2: APPLYING GRAVITY" << std::endl;
	//		if (currentVelocity.y > -1.0f) 
	//		{

	//			//playerPhysics->setLinearVelocity(glm::vec2(currentVelocity.x, currentVelocity.y - 5));
	//		}
	//	}
	//	else 
	//	{
			//STOP FALLING
	//		if (currentVelocity.y < 0) 
	//		{
	//			playerPhysics->setLinearVelocity(glm::vec2(currentVelocity.x, 0));
	//		}
	//	}
	//}

	//Rotate player
	float targetRotation = 0.0f;
	//ROTATE WITH KEYBOARD
	if (rotatingUp) 
	{
		float rotationSpeed = 3.5f;
		if (facingRight)
		{
			rotationSpeed = -rotationSpeed;
		}
		facingVector = glm::rotate(glm::vec2(1, 0), glm::radians(rotation - rotationSpeed));
	}
	else if (rotatingDown) 
	{
		float rotationSpeed = 3.5f;
		if (facingRight) 
		{
			rotationSpeed = -rotationSpeed;
		}
		facingVector = glm::rotate(glm::vec2(1, 0), glm::radians(rotation + rotationSpeed));
	}

	if (!(facingVector.x == 0 && facingVector.y == 0))
	{
		targetRotation = angleBetweenVectors(glm::vec2(1, 0), facingVector);
	}

	targetRotation = glm::degrees(targetRotation);

	//cout << "Target: " << targetRotation << std::endl;
	if (rotation != targetRotation && !isGrounded && !inDash) 
	{
		setDelayedRotation(targetRotation);
		//setRotation(targetRotation);
	}
	if(isGrounded)
	{
		setRotation(0);
	}

	//Moving player
	if (isGrounded) {
		if (movementVector.x != 0)
		{
			float currentX = currentVelocity.x;

			//If player presses the opposite direction set horizontal speed to 0
			//if ((currentVelocity.x < 0 && movementVector.x>0) || (currentVelocity.x > 0 && movementVector.x < 0))
			//{
			//	currentX = 0;
			//	playerPhysics->setLinearVelocity(glm::vec2(currentX, currentVelocity.y));
			//}
			//If Speed is more than maxSpeed
			float currentSpeed = currentVelocity.x;
			//cout << "0. curr speed: " << currentSpeed << std::endl;
			if (currentSpeed > maxSpeed && !isBoosting)
			{
				playerPhysics->setLinearVelocity((maxSpeed / currentSpeed) * currentVelocity);
			}
			//IF not, increase speed
			else
			{
				playerPhysics->addForce(glm::vec2(movementVector.x * acceleration * deltaTime, 0));
			}
		}
		else
		{
			//playerPhysics->setLinearVelocity(glm::vec2(0, currentVelocity.y));
		}
	}

	//Boosting player
	if (isBoosting && currBoost > 0.0f) 
	{
		currBoost -= boostBurnPerSecond*deltaTime;
		if (currBoost < 0) 
		{
			currBoost = 0;
		}
		currentVelocity = playerPhysics->getLinearVelocity();
		float currentSpeed = glm::length(currentVelocity);
		//cout << "1. curr BOOST speed: " << currentSpeed << std::endl;
		if (glm::length(currentVelocity) > maxSpeedWhenBoosting) 
		{
			playerPhysics->setLinearVelocity((maxSpeedWhenBoosting / currentSpeed) * currentVelocity);
		}
		else 
		{
			//Adjusting for direction
			if (facingRight)
			{
				playerPhysics->addForce(glm::vec2(glm::rotate(glm::vec2(1, 0), glm::radians(rotation)))* bostAccaleration);
			}
			else 
			{
				playerPhysics->addForce(glm::vec2(glm::rotate(glm::vec2(-1, 0), glm::radians(rotation)))* bostAccaleration);
			}
		}
	}

	//CHECK FOR DASH
	if (inDash)
	{
		dashCounter += deltaTime;
		if (facingRight) 
		{
			setRotation(rotation - rotationPerSecond*deltaTime);
		}
		else 
		{
			setRotation(rotation + rotationPerSecond*deltaTime);
		}
		
		//Fcout << "inDash; " << dashCounter << " "<<deltaTime<<std::endl;
		if (dashCounter > dashDuration)
		{
			inDash = false;
			endDash();
		}
	}
	//// BOOST GUI MARTIN DO HERE THE THING 
	//cout << "boost amount: " << currBoost <<std::endl;
}

float PlayerController::angleBetweenVectors(glm::vec2 vec1, glm::vec2 vec2)
{
	float dot = vec1.x*vec2.x + vec1.y*vec2.y;
	float det = vec1.x*vec2.y + vec1.y*vec2.x;
	float angle = atan2(det, dot);
	return angle;
}

void PlayerController::setFacingDirection(bool _facingRight)
{
	facingRight = _facingRight;
}

void PlayerController::applyMovement()
{

}

void PlayerController::stopFalling() 
{
	if (playerPhysics == nullptr)
	{
		playerPhysics = gameObject->getComponent<PhysicsComponent>();
	}
	glm::vec2 currentVelocity = playerPhysics->getLinearVelocity();
	if (currentVelocity.y < 0)
	{
		playerPhysics->setLinearVelocity(glm::vec2(currentVelocity.x, 0));
	}
}

void PlayerController::stopHorizontalMovement()
{
	if (playerPhysics == nullptr)
	{
		playerPhysics = gameObject->getComponent<PhysicsComponent>();
	}
	glm::vec2 currentVelocity = playerPhysics->getLinearVelocity();
	
	playerPhysics->setLinearVelocity(glm::vec2(0, currentVelocity.y));
}

void PlayerController::setRotation(float _rot)
{
	_rot = fmod(_rot, 360.0f);
	if (_rot < 0)
	{
		_rot += 360;
	}
	//cout << "Rotation: " << _rot << std::endl;
	rotation = _rot;
	//gameObject->setRotation(rotation);
	playerPhysics->body->SetTransform(playerPhysics->body->GetPosition(), glm::radians(rotation));
}

void PlayerController::setDelayedRotation(float _rot)
{
	_rot = fmod(_rot, 360.0f);
	if (_rot < 0)
	{
		_rot += 360;
	}

	//cout << "Rotation: " << _rot << std::endl;
	rotation = clerp(rotation, _rot, 0.2f);
	//gameObject->setRotation(rotation);
	playerPhysics->body->SetTransform(playerPhysics->body->GetPosition(), glm::radians(rotation));
}

float PlayerController::lerp(float a, float b, float f)
{
	return a + f * (b - a);
}

float PlayerController::clerp(float start, float end, float value) 
{
	float min = 0.0f;
	float max = 360.0f;
	float half = abs((max - min) / 2.0f);//half the distance between min and max
	float retval = 0.0f;
	float diff = 0.0f;

	if ((end - start) < -half)
	{
		diff = ((max - start) + end) * value;
		retval = start + diff;
	}
	else if ((end - start) > half)
	{
		diff = -((max - end) + start) * value;
		retval = start + diff;
	}
	else retval = start + (end - start) * value;

	// Debug.Log("Start: "  + start + "   End: " + end + "  Value: " + value + "  Half: " + half + "  Diff: " + diff + "  Retval: " + retval);
	return retval;
}
void PlayerController::jump()
{
	bool verticalJump = false;
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
			inDash = true;
			dashCounter = 0;
		}
	}
	else 
	{
		verticalJump = true;
		inDash = false;
	}
	//glm::vec2 currentVelocity = playerPhysics->getLinearVelocity();
	//

	cout << "facing vector: x: " << facingVector.x << "; y:" << facingVector.y << std::endl;

	//Check if direction vecor is zero, apply vertical jump

	glm::vec2 moveNormalized = glm::normalize(facingVector);
	//Adjusting for direction
	if (!facingRight)
	{
		moveNormalized = -moveNormalized;
	}
	if(verticalJump || (facingVector.x==0 && facingVector.y==0))
	{
		moveNormalized = glm::vec2(0, 1);
		inDash = false;
	}

	//Set Y speed to 0
	if (airDashCounter > 0) 
	{
		playerPhysics->setLinearVelocity(glm::vec2(playerPhysics->getLinearVelocity().x, 0));
	}

	playerPhysics->addImpulse(moveNormalized*dashSpeed);

}

void PlayerController::resetInputs()
{
	movementVector = glm::vec2(0.0f, 0.0f);
	facingVector = glm::vec2(0.0f, 0.0f);
	rotation = 0.0f;
	currBoost = maxBoost;
	isBoosting = false;
	rotatingDown = false;
	rotatingUp = false;
}

void PlayerController::endDash()
{
	inDash = false;
}

void PlayerController::onCollisionStart(PhysicsComponent *comp) {

}

void PlayerController::onCollisionEnd(PhysicsComponent *comp) {

}

void PlayerController::rechargeBoost(float _val)
{
	currBoost += _val;
	if (currBoost > maxBoost) 
	{
		currBoost = maxBoost;
	}
}

void PlayerController::gravityPowerUp()
{
}

void PlayerController::speedPowerUp()
{
}

void PlayerController::dashPowerUp()
{
}

void PlayerController::dashCountPowerUp()
{
}

float32 PlayerController::ReportFixture(b2Fixture *fixture, const b2Vec2 &point, const b2Vec2 &normal, float32 fraction) {
	string objName = ((GameObject*)fixture->GetBody()->GetUserData())->name;
	if (objName == "grass" || objName == "OuterBall" || objName == "Player_1" || objName == "Player_2") 
	{
		resetJumps();
		//cout << "GROUNDED" << std::endl;
		//facingVector = glm::vec2(0, 0);
		isGrounded = true;
		inDash = false;
	}
	return 0; // terminate raycast
}

void PlayerController::setSprite(sre::Sprite playerSprite_1) {
	this->playerSprite_1 = playerSprite_1;
}

void PlayerController::updateSprite(float deltaTime) {
	auto velocity = playerPhysics->getLinearVelocity();
	playerSprite->setSprite(playerSprite_1);
}

void PlayerController::resetJumps()
{
	airDashCounter = 0;
}

