#include "BallComponent.h"

#include <SDL_events.h>
#include <iostream>
#include "GameObject.hpp"
#include "SpriteComponent.hpp"
#include "PhysicsComponent.hpp"
#include "RocketBall.h"
#include "SpriteComponent.hpp"


BallComponent::BallComponent(GameObject *gameObject) : Component(gameObject) {
	initialPosition = gameObject->getPosition();
	blowBackForce = 0.5f;
}

void BallComponent::update(float deltaTime) {

	if (engageSlowmotion) {
		totalTime += deltaTime; //Increment
		tIndex = fmod(totalTime, 2); //return remainder between totalTime and 2
		tIndex = glm::smoothstep(0.0f, 1.0f, tIndex); //smoothstep using the tIndex; from 0 to 1
		float currentTimeScaleStep = glm::mix(0.1f, 1.0f, tIndex); //set the currentTimeScaleStep
		RocketBall::gameInstance->timeScale = currentTimeScaleStep; //Set the timeScale

		//When totalTime is larger than 0, return to normal speed.
		if (totalTime > 1.0f) {
			engageSlowmotion = false;
			tIndex = 0.0f;
			totalTime = 0.0f;
			RocketBall::gameInstance->timeScale = 1.0f; //Just a fail-safe in case something happens
		}
	}
	else {
		//Ensure the timescale is 0!
		RocketBall::gameInstance->timeScale = 1.0f;
		totalTime = 0;
		tIndex = 0;
	}
}

void BallComponent::setOuterBall(std::shared_ptr<PhysicsComponent> _outerBallPhysics) {
	ballPhysics = _outerBallPhysics;
}


void BallComponent::onCollisionStart(PhysicsComponent *PhysComp) {
	//If Goal 1 has been hit
	if (PhysComp->getGameObject()->name == "Goal_1" && !goalAchieved) {
		engageSlowmotion = true; //Engage slowmotion
		ballPhysics->addImpulse({ -ballPhysics->getLinearVelocity() * blowBackForce }); //blow backwards
		RocketBall::gameInstance->player2Goals++; //assign points
		ballPhysics->getGameObject()->setPosition({ initialPosition }); //reset position
		RocketBall::gameInstance->setGameState(GameState::RoundComplete); //change gamestate
		RocketBall::gameInstance->setTextAndPlaySound(2); //set text and play sound
		goalAchieved = true; //ensure goal cannot be achieved before next round
	}

	else if (PhysComp->getGameObject()->name == "Goal_2" && !goalAchieved) {
		std::cout << "Collision between: " + ballPhysics->getGameObject()->name + " and " + PhysComp->getGameObject()->name << std::endl;
		engageSlowmotion = true;
		ballPhysics->addImpulse({ -ballPhysics->getLinearVelocity() * blowBackForce });
		RocketBall::gameInstance->player1Goals++;
		ballPhysics->getGameObject()->setPosition({ initialPosition });
		RocketBall::gameInstance->setGameState(GameState::RoundComplete);
		std::cout << "GOAAAAAAAAL!" << std::endl;
		RocketBall::gameInstance->setTextAndPlaySound(2);
		goalAchieved = true;
	}
}

void BallComponent::onCollisionEnd(PhysicsComponent *PhysComp) {

}