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
	//check which player and assign color
	if (this->getGameObject()->name == "Player_1") {
		_color = RocketBall::gameInstance->player2Color;
	}
	else {
		_color = RocketBall::gameInstance->player1Color;
	}

#pragma region PlayerUI
	powerupTimers;

	for (int iter = ENUM_POWERUPS::SpeedIncrease; iter != ENUM_POWERUPS::SpeedIncrease; iter++)
	{
		ENUM_POWERUPS curr = static_cast<ENUM_POWERUPS>(iter);
		powerupTimers.insert(std::pair<ENUM_POWERUPS, float>(curr, 0.0f));
	}

	//Create gameobject for the boosting display and its border/background
	boostBox = RocketBall::gameInstance->mySpriteAtlas->get("boostMain.png");
	boostDisplay = RocketBall::gameInstance->createGameObject();
	boostBorderBox = RocketBall::gameInstance->mySpriteAtlas->get("boostBorderBackground.png");
	boostBorder = RocketBall::gameInstance->createGameObject();
	auto boostSpriteComp = boostDisplay->addComponent<SpriteComponent>();
	auto boostBorderSpriteComp = boostBorder->addComponent<SpriteComponent>();

	//create gameobjects to hold the icons
	gravityObj = RocketBall::gameInstance->createGameObject();
	speedObj = RocketBall::gameInstance->createGameObject();
	dashBoostObj = RocketBall::gameInstance->createGameObject();
	dashExtraObj = RocketBall::gameInstance->createGameObject();
	dashInfiniteObj = RocketBall::gameInstance->createGameObject();

	//assign the icons
	gravityIcon = RocketBall::gameInstance->mySpriteAtlas->get("lowGravity.png");
	speedIcon = RocketBall::gameInstance->mySpriteAtlas->get("speedBoostV2.png");
	dashBoostIcon = RocketBall::gameInstance->mySpriteAtlas->get("dashBoost.png");
	dashExtraIcon = RocketBall::gameInstance->mySpriteAtlas->get("ExtraJump.png");
	dashInfiniteIcon = RocketBall::gameInstance->mySpriteAtlas->get("infiniteBoost.png");

	//Size of the icons
	glm::vec2 iconScale({ 0.4,0.4 });
	gravityIcon.setScale(iconScale);
	speedIcon.setScale(iconScale);
	dashBoostIcon.setScale(iconScale);
	dashExtraIcon.setScale(iconScale);
	dashInfiniteIcon.setScale(iconScale);

	//Icon sprite components
	auto gravitySpriteComp = gravityObj->addComponent<SpriteComponent>();
	auto speedSpriteComp = speedObj->addComponent<SpriteComponent>();
	auto dashBoostSpriteComp = dashBoostObj->addComponent<SpriteComponent>();
	auto dashExtraSpriteComp = dashExtraObj->addComponent<SpriteComponent>();
	auto dashInfiniteSpriteComp = dashInfiniteObj->addComponent<SpriteComponent>();

	//Assign icon's position based on which player is holding the PlayerController
	if (this->getGameObject()->name == "Player_2") {

		boostDisplay->name = this->getGameObject()->name + "_BoostDisplay";
		boostBox.setScale({ 1,0.4 });
		boostBox.setColor(RocketBall::gameInstance->player1Color);
		boostBox.setOrderInBatch(11);
		boostSpriteComp->setSprite(boostBox);
		boostDisplay->setPosition({
			-RocketBall::gameInstance->windowSize.x * 0.5f + 22.5f,
			(-RocketBall::gameInstance->windowSize.y * 0.5f) + ((RocketBall::gameInstance->mySpriteAtlas->get("Grass.png").getSpriteSize().y * 0.5f) * 0.5f) - 20.0f });
		boostDisplay->setRotation(0);

		boostBorder->name = "boostboxborder";
		boostBorderBox.setScale({ 1,0.4 });
		boostBorderBox.setOrderInBatch(10);
		boostBorderSpriteComp->setSprite(boostBorderBox);
		boostBorder->setPosition({ (-RocketBall::gameInstance->windowSize.x * 0.5f) + (boostBorderBox.getSpriteSize().x*0.5) + 20.0f,
			(-RocketBall::gameInstance->windowSize.y * 0.5f) + ((RocketBall::gameInstance->mySpriteAtlas->get("Grass.png").getSpriteSize().y * 0.5f) * 0.5f) - 20.0f });

		gravityObj->setPosition({ (boostBorder->getPosition().x + (boostBorderBox.getSpriteSize().x*boostBorderBox.getScale().x)*0.5) + ((gravityIcon.getSpriteSize().x*gravityIcon.getScale().x)*0.5), boostBorder->getPosition().y });
		speedObj->setPosition({ (gravityObj->getPosition().x + ((gravityIcon.getSpriteSize().x*gravityIcon.getScale().x))), boostBorder->getPosition().y });
		dashBoostObj->setPosition({ (speedObj->getPosition().x + ((gravityIcon.getSpriteSize().x*gravityIcon.getScale().x))), boostBorder->getPosition().y });
		dashExtraObj->setPosition({ (dashBoostObj->getPosition().x + ((gravityIcon.getSpriteSize().x*gravityIcon.getScale().x))), boostBorder->getPosition().y });
		dashInfiniteObj->setPosition({ (dashExtraObj->getPosition().x + ((gravityIcon.getSpriteSize().x*gravityIcon.getScale().x))), boostBorder->getPosition().y });

	}
	else {

		boostDisplay->name = this->getGameObject()->name + "_BoostDisplay";
		boostBox.setScale({ 1,0.4 });
		boostBox.setOrderInBatch(1);
		boostBox.setColor(RocketBall::gameInstance->player2Color);
		boostSpriteComp->setSprite(boostBox);
		boostDisplay->setPosition({
			RocketBall::gameInstance->windowSize.x * 0.5f - 22.5f,
			(-RocketBall::gameInstance->windowSize.y * 0.5f) + ((RocketBall::gameInstance->mySpriteAtlas->get("Grass.png").getSpriteSize().y * 0.5f) * 0.5f) - 20.0f });
		boostDisplay->setRotation(180);

		boostBorder->name = "boostboxborder";
		boostBorderBox.setScale({ 1,0.4 });
		boostBorderBox.setOrderInBatch(0);
		boostBorderSpriteComp->setSprite(boostBorderBox);
		boostBorder->setPosition({ (RocketBall::gameInstance->windowSize.x * 0.5f) - (boostBorderBox.getSpriteSize().x*0.5) - 20.0f,
			(-RocketBall::gameInstance->windowSize.y * 0.5f) + ((RocketBall::gameInstance->mySpriteAtlas->get("Grass.png").getSpriteSize().y * 0.5f) * 0.5f) - 20.0f });

		gravityObj->setPosition({ (boostBorder->getPosition().x - (boostBorderBox.getSpriteSize().x*boostBorderBox.getScale().x)*0.5) - ((gravityIcon.getSpriteSize().x*gravityIcon.getScale().x)*0.5), boostBorder->getPosition().y });
		speedObj->setPosition({ (gravityObj->getPosition().x - ((gravityIcon.getSpriteSize().x*gravityIcon.getScale().x))), boostBorder->getPosition().y });
		dashBoostObj->setPosition({ (speedObj->getPosition().x - ((gravityIcon.getSpriteSize().x*gravityIcon.getScale().x))), boostBorder->getPosition().y });
		dashExtraObj->setPosition({ (dashBoostObj->getPosition().x - ((gravityIcon.getSpriteSize().x*gravityIcon.getScale().x))), boostBorder->getPosition().y });
		dashInfiniteObj->setPosition({ (dashExtraObj->getPosition().x - ((gravityIcon.getSpriteSize().x*gravityIcon.getScale().x))), boostBorder->getPosition().y });

	}

	//Set the sprites
	gravitySpriteComp->setSprite(gravityIcon);
	speedSpriteComp->setSprite(speedIcon);
	dashBoostSpriteComp->setSprite(dashBoostIcon);
	dashExtraSpriteComp->setSprite(dashExtraIcon);
	dashInfiniteSpriteComp->setSprite(dashInfiniteIcon);
#pragma endregion

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
				cout << "Joystix indx: 1; Val: " << newVal << " curr movement vector: " << movementVector.x << std::endl;
				if ((newVal <= 0 && movementVector.x > 0) || (newVal >= 0 && movementVector.x < 0))
				{
					if (abs(newVal) < abs(movementVector.x))
					{
						cout << "Returning" << std::endl;
						return false;
					}
				}
				movementVector.x = newVal;
			}
			if (axisIndex == 2)
			{
				float newVal = Remap(event.jaxis.value, -32768, 32767, 0, 1);

				//SETTING PROPER DIRECTION
				if (facingRight)
				{
					newVal = -newVal;
				}
				cout << "Joystix indx: 2; Val: " << newVal << " curr movement vector: " << movementVector.x << std::endl;
				if ((newVal <= 0 && movementVector.x > 0) || (newVal >= 0 && movementVector.x < 0))
				{
					if (abs(newVal) < abs(movementVector.x))
					{
						cout << "Returning" << std::endl;
						return false;
					}
				}
				movementVector.x = newVal;
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
			if (facingRight) {
				if (event.key.keysym.sym == SDLK_RIGHT)
				{
					movementVector.x = 1;
				}
				else if (event.key.keysym.sym == SDLK_LEFT)
				{
					movementVector.x = -1;
				}
				if (event.key.keysym.sym == SDLK_RCTRL)
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
			}
			else
			{
				if (event.key.keysym.sym == SDLK_d)
				{
					movementVector.x = 1;
				}
				else if (event.key.keysym.sym == SDLK_a)
				{
					movementVector.x = -1;
				}
				if (event.key.keysym.sym == SDLK_LCTRL)
				{
					jump();
				}
				if (event.key.keysym.sym == SDLK_s)
				{
					rotatingDown = true;
				}
				else if (event.key.keysym.sym == SDLK_w)
				{
					rotatingUp = true;
				}
				if (event.key.keysym.sym == SDLK_LSHIFT)
				{
					isBoosting = true;
				}
			}
		}
		else if (event.type == SDL_KEYUP)
		{
			if (facingRight) {
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
			else
			{
				if (event.key.keysym.sym == SDLK_d && movementVector.x > 0)
				{
					movementVector.x = 0;
				}
				if (event.key.keysym.sym == SDLK_a && movementVector.x < 0)
				{
					movementVector.x = 0;
				}
				if (event.key.keysym.sym == SDLK_s)
				{
					rotatingDown = false;
				}
				else if (event.key.keysym.sym == SDLK_w)
				{
					rotatingUp = false;
				}
				if (event.key.keysym.sym == SDLK_LSHIFT)
				{
					isBoosting = false;
				}
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

	auto from = playerPhysics->body->GetWorldCenter();

	glm::vec2 currentVelocity = playerPhysics->getLinearVelocity();
	//RAYCASTS
	b2Vec2 to{ from.x + currentVelocity.x*0.1f,from.y + currentVelocity.y*0.1f };
	b2Vec2 toGround{ from.x,from.y - 0.3f };
	if (isGrounded)
	{
		wasGrounded = true;
	}
	else
	{
		wasGrounded = false;
	}
	isGrounded = false;
	if (from != toGround) {
		RocketBall::gameInstance->world->RayCast(this, from, toGround);
	}

	//Rotate player
	float targetRotation = 0.0f;
	//ROTATE WITH KEYBOARD
	if (rotatingUp)
	{
		float rotationSpeed = keyboardRotateSpeed;
		if (facingRight)
		{
			rotationSpeed = -rotationSpeed;
		}
		facingVector = glm::rotate(glm::vec2(1, 0), glm::radians(rotation - rotationSpeed*deltaTime));
	}
	else if (rotatingDown)
	{
		float rotationSpeed = keyboardRotateSpeed;
		if (facingRight)
		{
			rotationSpeed = -rotationSpeed;
		}
		facingVector = glm::rotate(glm::vec2(1, 0), glm::radians(rotation + rotationSpeed * deltaTime));
	}

	//ROTATE WITH JOYSTICKK
	if (!(facingVector.x == 0 && facingVector.y == 0))
	{
		targetRotation = angleBetweenVectors(glm::vec2(1, 0), facingVector);
	}

	targetRotation = glm::degrees(targetRotation);

	if (rotation != targetRotation && !isGrounded && !inDash)
	{
		setDelayedRotation(targetRotation);
	}
	if (isGrounded)
	{
		setRotation(0);
	}

	//Moving player
	if (isGrounded) {
		if (movementVector.x != 0)
		{
			//If Speed is more than maxSpeed
			float currentSpeed = currentVelocity.x;
			if (abs(currentSpeed) > maxSpeed && !isBoosting)
			{
			}
			//IF not, increase speed
			else
			{
				playerPhysics->setLinearVelocity(glm::vec2(currentSpeed + movementVector.x * acceleration * deltaTime, currentVelocity.y));
			}
		}
	}

	//Boosting player
	if (isBoosting && (currBoost > 0.0f || infiniteBoost))
	{
		if (!infiniteBoost)
		{
			currBoost -= boostBurnPerSecond*deltaTime;
		}
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
				playerPhysics->addForce(glm::vec2(glm::rotate(glm::vec2(1, 0), glm::radians(rotation)))* bostAccaleration *deltaTime);
			}
			else
			{
				playerPhysics->addForce(glm::vec2(glm::rotate(glm::vec2(-1, 0), glm::radians(rotation)))* bostAccaleration*deltaTime);
			}
		}
	}

	//CHECK FOR DASH
	if (inDash)
	{
		cout << "in dash: " << deltaTime << std::endl;
		dashCounter += deltaTime;
		float rotationChange = rotationPerSecond*deltaTime;
		if (facingRight)
		{
			rotationChange = -rotationChange;

		}
		if (!dashingForward)
		{
			rotationChange = -rotationChange;
		}
		setRotation(rotation + rotationChange);
		//Fcout << "inDash; " << dashCounter << " "<<deltaTime<<std::endl;
		if (dashCounter > dashDuration)
		{
			inDash = false;
			endDash();
		}
	}

	//boost sprite, set scale according to current boost (between 0 and 1)
	//The boost sprite has a custom pivot position in the JSON (x = 0) to
	//ensure the sprite scales from x = 0
	boostSprite.setScale({ currBoost, 0.4 });
	boostDisplay->getComponent<SpriteComponent>()->setSprite(boostSprite);


	//////////////////// CHECK FOR POWERUPS
	if (this->getGameObject()->name == "Player_1") {
		glm::vec4 _color = RocketBall::gameInstance->player1Color;
	}
	else {
		glm::vec4 _color = RocketBall::gameInstance->player2Color;
	}
	for (std::map<ENUM_POWERUPS, float>::iterator it = powerupTimers.begin(); it != powerupTimers.end(); ++it)
	{
		if (it->second > 0)
		{
			it->second += deltaTime;
		}

		if (it->second >= powerupDuration)
		{
			it->second = 0;
			switch (it->first)
			{
			case ENUM_POWERUPS::DashCountIncrease:
				dashCountPowerUp(false);
				break;
			case ENUM_POWERUPS::SpeedIncrease:
				speedPowerUp(false);
				break;
			case ENUM_POWERUPS::DashSpeedIncrease:
				dashPowerUp(false);
				break;
			case ENUM_POWERUPS::InfiniteBoost:
				infiniteBoostPowerUp(false);
				break;
			case ENUM_POWERUPS::GravityMod:
				gravityPowerUp(false);
				break;
			default:
				break;
			}
		}
	}
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

	//CLERP->Circular lerp smooth transition between 0 and 360
	rotation = clerp(rotation, _rot, 0.2f);
	//ROTATE car
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
			dashCounter = 0;
			inDash = true;
			if (facingVector.x > 0)
			{
				dashingForward = true;
			}
			else if (facingVector.x < 0)
			{
				dashingForward = false;
			}
		}
	}
	else
	{
		airDashCounter = 0;
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
	if (verticalJump || (facingVector.x == 0 && facingVector.y == 0))
	{
		moveNormalized = glm::vec2(0, 1);
		inDash = false;
	}

	//Set Y speed to 0
	if (airDashCounter > 0)
	{
		playerPhysics->setLinearVelocity(glm::vec2(playerPhysics->getLinearVelocity().x, 0));
	}
	//JUMP
	playerPhysics->addImpulse(moveNormalized*dashSpeed);

}

void PlayerController::resetInputs()
{
	movementVector = glm::vec2(0.0f, 0.0f);
	facingVector = glm::vec2(0.0f, 0.0f);
	rotation = 0.0f;
	currBoost = maxBoost / 4;
	isBoosting = false;
	rotatingDown = false;
	rotatingUp = false;
	if (!RocketBall::gameInstance->gameModeClassic)
		DisableAllPowerups();
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

void PlayerController::DisableAllPowerups()
{
	if (powerupTimers[ENUM_POWERUPS::GravityMod] > 0)
	{
		gravityPowerUp(false);
	}

	if (powerupTimers[ENUM_POWERUPS::SpeedIncrease] > 0)
	{
		speedPowerUp(false);
	}

	if (powerupTimers[ENUM_POWERUPS::DashSpeedIncrease] > 0)
	{
		dashPowerUp(false);
	}
	if (powerupTimers[ENUM_POWERUPS::DashCountIncrease] > 0)
	{
		dashCountPowerUp(false);
	}
	if (powerupTimers[ENUM_POWERUPS::InfiniteBoost] > 0)
	{
		infiniteBoostPowerUp(false);
	}

}

void PlayerController::gravityPowerUp(bool _enable)
{
	if (_enable)
	{
		gravityIcon.setColor(_color);
		if (powerupTimers[ENUM_POWERUPS::GravityMod] != 0)
		{
			return;
		}
		playerPhysics->body->SetGravityScale(playerPhysics->body->GetGravityScale()*0.5f);
		powerupTimers[ENUM_POWERUPS::GravityMod] = 0.01f;
	}
	else
	{
		gravityIcon.setColor({ 1,1,1,1 });
		powerupTimers[ENUM_POWERUPS::GravityMod] = 0;
		playerPhysics->body->SetGravityScale(playerPhysics->body->GetGravityScale() / 0.5f);
	}
	gravityObj->getComponent<SpriteComponent>()->setSprite(gravityIcon);
}

void PlayerController::speedPowerUp(bool _enable)
{
	if (_enable)
	{
		speedIcon.setColor(_color);
		if (powerupTimers[ENUM_POWERUPS::SpeedIncrease] != 0)
		{
			return;
		}
		maxSpeed*=1.3f;
		acceleration *= 1.3f;
		bostAccaleration *= 1.3f;
		maxSpeedWhenBoosting *= 1.3f;
		powerupTimers[ENUM_POWERUPS::SpeedIncrease] = 0.01f;
	}
	else
	{
		speedIcon.setColor({ 1,1,1,1 });
		powerupTimers[ENUM_POWERUPS::SpeedIncrease] = 0;
		maxSpeed /= 1.3f;
		acceleration /= 1.3f;
		bostAccaleration /= 1.3f;
		maxSpeedWhenBoosting /= 1.3f;
	}
	speedObj->getComponent<SpriteComponent>()->setSprite(speedIcon);

}

void PlayerController::dashPowerUp(bool _enable)
{
	if (_enable)
	{
		dashBoostIcon.setColor(_color);
		if (powerupTimers[ENUM_POWERUPS::DashSpeedIncrease] != 0)
		{
			return;
		}
		dashSpeed *= 1.3f;
		dashDuration *= 1.3f;
		powerupTimers[ENUM_POWERUPS::DashSpeedIncrease] = 0.01f;
	}
	else
	{
		dashBoostIcon.setColor({ 1,1,1,1 });
		powerupTimers[ENUM_POWERUPS::DashSpeedIncrease] = 0;
		dashSpeed /= 1.3;
		dashDuration /= 1.3;
	}
	dashBoostObj->getComponent<SpriteComponent>()->setSprite(dashBoostIcon);
}

void PlayerController::dashCountPowerUp(bool _enable)
{
	if (_enable)
	{
		dashExtraIcon.setColor(_color);
		if (powerupTimers[ENUM_POWERUPS::DashCountIncrease] != 0)
		{
			return;
		}
		airDashesAvailable = 2;
		powerupTimers[ENUM_POWERUPS::DashCountIncrease] = 0.01f;
	}
	else
	{
		dashExtraIcon.setColor({ 1,1,1,1 });
		powerupTimers[ENUM_POWERUPS::DashCountIncrease] = 0;
		airDashesAvailable = 1;
	}
	dashExtraObj->getComponent<SpriteComponent>()->setSprite(dashExtraIcon);
}

void PlayerController::infiniteBoostPowerUp(bool _enable)
{
	if (_enable)
	{
		dashInfiniteIcon.setColor(_color);
		infiniteBoost = true;
		rechargeBoost(1);
		powerupTimers[ENUM_POWERUPS::InfiniteBoost] = 0.01f;
	}
	else
	{
		dashInfiniteIcon.setColor({ 1,1,1,1 });
		powerupTimers[ENUM_POWERUPS::InfiniteBoost] = 0;
		infiniteBoost = false;
	}
	dashInfiniteObj->getComponent<SpriteComponent>()->setSprite(dashInfiniteIcon);
}

float32 PlayerController::ReportFixture(b2Fixture *fixture, const b2Vec2 &point, const b2Vec2 &normal, float32 fraction) {
	string objName = ((GameObject*)fixture->GetBody()->GetUserData())->name;
	//CHECK IF ON FLOOR OR BALL OR OTHER PLAYER
	if (objName == "grass" || objName == "OuterBall" || objName == "Player_1" || objName == "Player_2" || objName == "Wall2")
	{
		if (!wasGrounded)
		{
			facingVector = glm::vec2(0, 0);
		}
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
