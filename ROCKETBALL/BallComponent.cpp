#include "BallComponent.h"

#include <SDL_events.h>
#include <iostream>
#include "GameObject.hpp"
#include "SpriteComponent.hpp"
#include "PhysicsComponent.hpp"
#include "RocketBall.h"
#include "SpriteComponent.hpp"


BallComponent::BallComponent(GameObject *gameObject) : Component(gameObject) {
	// initiate bird physics
	ballPhysics = gameObject->getComponent<PhysicsComponent>();
	initialPosition = gameObject->getPosition();
	blowBackForce = 0.1f;
}

void BallComponent::update(float deltaTime) {

	if (engageSlowmotion) {
		totalTime += deltaTime;

		tIndex = fmod(totalTime, 2);
		tIndex = glm::smoothstep(0.0f, 1.0f, tIndex);
		float currentTimeScaleStep = glm::mix(0.1f, 1.0f, tIndex);
		std::cout << currentTimeScaleStep << std::endl;
		RocketBall::gameInstance->timeScale = currentTimeScaleStep;

		if (totalTime > 1.0f) {
			engageSlowmotion = false;
			tIndex = 0.0f;
			totalTime = 0.0f;
			RocketBall::gameInstance->timeScale = 1.0f; //Just a fail-safe in case something happens
		}
	}
}


void BallComponent::onCollisionStart(PhysicsComponent *PhysComp) {
	if (PhysComp->getGameObject()->name == "Goal_1") {
		std::cout << "Collision between: " + ballPhysics->getGameObject()->name + " and " + PhysComp->getGameObject()->name << std::endl;
		engageSlowmotion = true;
		//RocketBall::gameInstance->timeScale = timeScaleConstant;
		ballPhysics->addImpulse({ -ballPhysics->getLinearVelocity() * blowBackForce });
		RocketBall::gameInstance->player2Goals++;
		ballPhysics->getGameObject()->setPosition({ initialPosition });
		RocketBall::gameInstance->setGameState(GameState::RoundComplete);
	}

	else if (PhysComp->getGameObject()->name == "Goal_2") {
		std::cout << "Collision between: " + ballPhysics->getGameObject()->name + " and " + PhysComp->getGameObject()->name << std::endl;
		engageSlowmotion = true;
		//RocketBall::gameInstance->timeScale = timeScaleConstant;
		ballPhysics->addImpulse({ -ballPhysics->getLinearVelocity() * blowBackForce });
		RocketBall::gameInstance->player1Goals++;
		ballPhysics->getGameObject()->setPosition({ initialPosition });
		RocketBall::gameInstance->setGameState(GameState::RoundComplete);
	}
}

void BallComponent::onCollisionEnd(PhysicsComponent *PhysComp) {

}