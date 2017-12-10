#include "AbilityComponent.h"
#include <SDL_events.h>
#include <iostream>
#include "GameObject.hpp"
#include "SpriteComponent.hpp"
#include "PhysicsComponent.hpp"
#include "RocketBall.h"
#include "SpriteComponent.hpp"
#include "PlayerController.h"

using namespace std;

AbilityComponent::AbilityComponent(GameObject *gameObject) : Component(gameObject) {
	color = glm::vec4{ 0,0,0,0 };
	destroy = false;
}
void AbilityComponent::update(float deltaTime) {

	if (hasProvidedAbility && RocketBall::gameInstance->getGameState() == GameState::Running) {
		totalTime += deltaTime;

		if (totalTime >= cooldown) {
			readyBox(true);
		}
	}

	if (destroy) {
		DestroyAbillityBox(this->gameObject);
	}
}

void AbilityComponent::onCollisionStart(PhysicsComponent *PhysComp) {
	if (PhysComp->getGameObject()->name == "Player_1" ||
		PhysComp->getGameObject()->name == "Player_2" &&
		!hasProvidedAbility) {
		ProvideAbility(PhysComp);
		RocketBall::gameInstance->playPickUp();
		//destroy = true;
	}
}

void AbilityComponent::onCollisionEnd(PhysicsComponent* PhysComp) {

}

void AbilityComponent::setCooldown(float _cooldown) {
	cooldown = _cooldown;
}

void AbilityComponent::setBoostTypeInClassic(bool isLargeBoost) {
	largeBoostRecharge = isLargeBoost;
}

void AbilityComponent::SelectAbilityType() {
	glm::vec2 originalScale = this->getGameObject()->getComponent<SpriteComponent>()->getSprite().getScale();

	if (RocketBall::gameInstance->gameModeClassic) {
		if (largeBoostRecharge) {
			//select large boost
			indexer = 1;
		}
		else {
			//select small boost
			indexer = 0;
		}
	}
	else {
		indexer = rand() % 7;
	}

	switch (indexer)
	{
	case 0:
		//Boost Power
		abilitySprite = RocketBall::gameInstance->mySpriteAtlas->get("smallBoost.png");
		color = glm::vec4{ 1.0f, 0.2f, 0.2f, 1.0f };
		abilitySprite.setColor(color);
		abilitySprite.setScale(originalScale);
		abilitySprite.setOrderInBatch(9);
		this->gameObject->getComponent<SpriteComponent>()->setSprite(abilitySprite);
		break;
	case 1:
		//Large Boost
		abilitySprite = RocketBall::gameInstance->mySpriteAtlas->get("largeBoost.png");
		color = glm::vec4{ 1.0f, 0.1f, 0.1f, 1.0f };
		abilitySprite.setColor(color);
		abilitySprite.setScale(originalScale);
		abilitySprite.setOrderInBatch(9);
		this->gameObject->getComponent<SpriteComponent>()->setSprite(abilitySprite);
		break;
	case 2:
		//Gravity
		abilitySprite = RocketBall::gameInstance->mySpriteAtlas->get("lowGravity.png");
		color = glm::vec4{ 0.2f, 0.2f, 1.0f, 1.0f };
		abilitySprite.setColor(color);
		abilitySprite.setScale(originalScale);
		abilitySprite.setOrderInBatch(9);
		this->gameObject->getComponent<SpriteComponent>()->setSprite(abilitySprite);
		break;
	case 3:
		//Speed
		abilitySprite = RocketBall::gameInstance->mySpriteAtlas->get("speedBoostV2.png");
		color = glm::vec4{ 1.0f, 0.2f, 1.0f, 1.0f };
		abilitySprite.setColor(color);
		abilitySprite.setScale(originalScale);
		abilitySprite.setOrderInBatch(9);
		this->gameObject->getComponent<SpriteComponent>()->setSprite(abilitySprite);
		break;
	case 4:
		//Dash Boost
		abilitySprite = RocketBall::gameInstance->mySpriteAtlas->get("dashBoost.png");
		color = glm::vec4{ 1.0f, 1.0f, 0.0f, 1.0f };
		abilitySprite.setColor(color);
		abilitySprite.setScale(originalScale);
		abilitySprite.setOrderInBatch(9);
		this->gameObject->getComponent<SpriteComponent>()->setSprite(abilitySprite);
		break;
	case 5:
		//Dash Extra
		abilitySprite = RocketBall::gameInstance->mySpriteAtlas->get("ExtraJump.png");
		color = glm::vec4{ 0.2f, 1.0f, 1.0f, 1.0f };
		abilitySprite.setColor(color);
		abilitySprite.setScale(originalScale);
		abilitySprite.setOrderInBatch(9);
		this->gameObject->getComponent<SpriteComponent>()->setSprite(abilitySprite);
		break;
	case 6:
		//Infinite Boost
		abilitySprite = RocketBall::gameInstance->mySpriteAtlas->get("infiniteBoost.png");
		color = glm::vec4{ 0.2f, 1.0f, 0.2f, 1.0f };
		abilitySprite.setColor(color);
		abilitySprite.setScale(originalScale);
		abilitySprite.setOrderInBatch(9);
		this->gameObject->getComponent<SpriteComponent>()->setSprite(abilitySprite);
		break;
	}
}

void AbilityComponent::ProvideAbility(PhysicsComponent *PhysComp) {
	switch (indexer)
	{
	case 0:
		//Boost Power
		PhysComp->getGameObject()->getComponent<PlayerController>()->rechargeBoost(0.25f);
		break;
	case 1:
		//Large Boost
		PhysComp->getGameObject()->getComponent<PlayerController>()->rechargeBoost(1.0f);
		break;
	case 2:
		//Gravity
		PhysComp->getGameObject()->getComponent<PlayerController>()->gravityPowerUp(true);
		break;
	case 3:
		//Speed
		PhysComp->getGameObject()->getComponent<PlayerController>()->speedPowerUp(true);
		break;
	case 4:
		//Dash boost
		PhysComp->getGameObject()->getComponent<PlayerController>()->dashPowerUp(true);
		break;
	case 5:
		//Dash Extra
		PhysComp->getGameObject()->getComponent<PlayerController>()->dashCountPowerUp(true);
		break;
	case 6:
		//Dash Extra
		PhysComp->getGameObject()->getComponent<PlayerController>()->infiniteBoostPowerUp(true);
		break;
	}
	readyBox(false);
}

void AbilityComponent::readyBox(bool ready) {

	sre::Sprite _sprite = this->getGameObject()->getComponent<SpriteComponent>()->getSprite();
	totalTime = 0;

	if (!ready) {
		hasProvidedAbility = true;
		_sprite.setColor({ color.x, color.y, color.z, 0 });
		this->getGameObject()->getComponent<SpriteComponent>()->setSprite(_sprite);
	}
	else {
		hasProvidedAbility = false;
		SelectAbilityType();
	}
}

void AbilityComponent::DestroyAbillityBox(GameObject * gameObject) {
	gameObject = this->gameObject;
	RocketBall::gameInstance->deregisterPhysicsComponent(gameObject->getComponent<PhysicsComponent>().get());
	gameObject->removeComponent(gameObject->getComponent<PhysicsComponent>());
	delete gameObject;
}



/*
void AbilityComponent::setCollisionFilter(uint16 categoryBits) {
	b2Filter filter = this->getGameObject()->getComponent<PhysicsComponent>()->fixture->GetFilterData();
	filter.categoryBits = categoryBits;
	this->getGameObject()->getComponent<PhysicsComponent>()->fixture->SetFilterData(filter);
}

*/