#include "AbilityComponent.h"
#include <SDL_events.h>
#include <iostream>
#include "GameObject.hpp"
#include "SpriteComponent.hpp"
#include "PhysicsComponent.hpp"
#include "RocketBall.h"
#include "SpriteComponent.hpp"
#include "PlayerController.h"



AbilityComponent::AbilityComponent(GameObject *gameObject) : Component(gameObject) {
	abilityDuration = 0.0f;
}

void AbilityComponent::update(float deltaTime) {

	if (provideAbility && affectedObject != nullptr) {

		totalTime += deltaTime;

		if (totalTime >= abilityDuration) {
			if (indexer == 0) {
				affectedObject->body->SetGravityScale(initialGravity);
			}
			else if (indexer == 1) {
				affectedObject->getGameObject()->getComponent<PlayerController>()->maxSpeed = InitialmaxSpeed;
				affectedObject->getGameObject()->getComponent<PlayerController>()->acceleration = Initialacceleration;
			}
			else if (indexer == 2) {
				affectedObject->getGameObject()->getComponent<PlayerController>()->dashSpeed = InitialdashSpeed;
				affectedObject->getGameObject()->getComponent<PlayerController>()->dashDuration = InitialdashDuration;
			}
			else if (indexer == 3) {
				affectedObject->getGameObject()->getComponent<PlayerController>()->airDashesAvailable = InitialAirDashesAvailable;
			}

			totalTime = 0;
			setSpriteInvis(false);
			setCollisionFilter(RocketBall::gameInstance->ABILITYBOX);
			affectedObject = nullptr;
			provideAbility = false;

			DestroyAbillityBox(this->gameObject);
		}
	}
}

void AbilityComponent::DestroyAbillityBox(GameObject * gameObject) {
	gameObject = this->gameObject;
	RocketBall::gameInstance->deregisterPhysicsComponent(gameObject->getComponent<PhysicsComponent>().get());
	gameObject->removeComponent(gameObject->getComponent<PhysicsComponent>());
	delete gameObject;
}

void AbilityComponent::onCollisionStart(PhysicsComponent *PhysComp) {
	if (PhysComp->getGameObject()->name == "Player_1" ||
		PhysComp->getGameObject()->name == "Player_2" &&
		!provideAbility) {
		provideAbility = true;
		std::cout << PhysComp->getGameObject()->name + " collected an ability" << std::endl;
		SelectAndProvide(PhysComp);
	}
}

void AbilityComponent::onCollisionEnd(PhysicsComponent* PhysComp) {

}

void AbilityComponent::setCollisionFilter(uint16 categoryBits) {
	b2Filter filter = this->getGameObject()->getComponent<PhysicsComponent>()->fixture->GetFilterData();
	filter.categoryBits = categoryBits;
	this->getGameObject()->getComponent<PhysicsComponent>()->fixture->SetFilterData(filter);
}

void AbilityComponent::setSpriteInvis(bool setInvis) {
	glm::vec4 color = this->getGameObject()->getComponent<SpriteComponent>()->getSprite().getColor();
	sre::Sprite _sprite = this->getGameObject()->getComponent<SpriteComponent>()->getSprite();

	if (setInvis) {
		_sprite.setColor({ color.x, color.y, color.z, 0 });
		this->getGameObject()->getComponent<SpriteComponent>()->setSprite(_sprite);
	}
	else {
		_sprite.setColor({ color.x, color.y, color.z, 1 });
		this->getGameObject()->getComponent<SpriteComponent>()->setSprite(_sprite);
	} 
}

void AbilityComponent::SelectAndProvide(PhysicsComponent *PhysComp) {

	setSpriteInvis(true);
	setCollisionFilter(RocketBall::gameInstance->ABILITYBOXUSED);
	affectedObject = PhysComp;

	indexer = rand() % 4;

	switch (indexer)
	{
	case 0:
		std::cout << "Ability 1: Lower Gravity!" << std::endl;
		initialGravity = affectedObject->body->GetGravityScale();
		affectedObject->body->SetGravityScale(PhysComp->body->GetGravityScale()*0.5);
		abilityDuration = 3.0f;
		break;
	case 1:
		std::cout << "Ability 2: Faster!" << std::endl;
		InitialmaxSpeed = affectedObject->getGameObject()->getComponent<PlayerController>()->maxSpeed;
		Initialacceleration = affectedObject->getGameObject()->getComponent<PlayerController>()->acceleration;
		affectedObject->getGameObject()->getComponent<PlayerController>()->maxSpeed = InitialmaxSpeed * 2;
		affectedObject->getGameObject()->getComponent<PlayerController>()->acceleration = Initialacceleration * 2;
		abilityDuration = 2.0f;
		break;
	case 2:
		std::cout << "Ability 3: Better Dash!" << std::endl;
		InitialdashSpeed = affectedObject->getGameObject()->getComponent<PlayerController>()->dashSpeed;
		InitialdashDuration = affectedObject->getGameObject()->getComponent<PlayerController>()->dashDuration;
		affectedObject->getGameObject()->getComponent<PlayerController>()->dashSpeed = InitialdashSpeed * 2;
		affectedObject->getGameObject()->getComponent<PlayerController>()->dashDuration = InitialdashDuration * 2;
		abilityDuration = 3.0f;
		break;
	case 3:
		std::cout << "Ability 3: Moaaar Dash!" << std::endl;
		InitialAirDashesAvailable = affectedObject->getGameObject()->getComponent<PlayerController>()->airDashesAvailable;
		affectedObject->getGameObject()->getComponent<PlayerController>()->airDashesAvailable = InitialAirDashesAvailable + 2;
		abilityDuration = 3.0f;
		break;
	}

	totalTime = 0;

}