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

//Update to determine when the box is ready again
void AbilityComponent::update(float deltaTime) {
	if (hasProvidedAbility && RocketBall::gameInstance->getGameState() == GameState::Running) {
		totalTime += deltaTime;

		if (totalTime >= cooldown) {
			readyBox(true);
		}
	}

	/* //Call to destroy if needed
	if (destroy) {
		DestroyAbillityBox(this->gameObject);
	}
	*/
}

void AbilityComponent::onCollisionStart(PhysicsComponent *PhysComp) {
	//Check if the collided object is player 1 or player 2
	if (PhysComp->getGameObject()->name == "Player_1" ||
		PhysComp->getGameObject()->name == "Player_2" &&
		!hasProvidedAbility) {
		ProvideAbility(PhysComp); //Call function on the object collided with
		RocketBall::gameInstance->playPickUp(); //Play sound for picking up pellet
		//destroy = true;
	}
}

void AbilityComponent::onCollisionEnd(PhysicsComponent* PhysComp) {

}

//Sets cooldown of pellet; time before ready again
void AbilityComponent::setCooldown(float _cooldown) {
	cooldown = _cooldown;
}

//Set whether it is a large boost type
void AbilityComponent::setBoostTypeInClassic(bool isLargeBoost) {
	largeBoostRecharge = isLargeBoost;
}

//Function to Select the type of ability
void AbilityComponent::SelectAbilityType() {
	//Keep the scale
	glm::vec2 originalScale = this->getGameObject()->getComponent<SpriteComponent>()->getSprite().getScale();

	//If classic gamemode;
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
		//Chose a random between 0 and 6
		indexer = rand() % 7;
	}

	//Select ability type using the indexer assigned above
	switch (indexer)
	{
	case 0:
		//Boost Power
		/*Sprite*/abilitySprite = RocketBall::gameInstance->mySpriteAtlas->get("smallBoost.png");
		/*Color*/color = glm::vec4{ 1.0f, 0.2f, 0.2f, 1.0f };
		/*Set Color*/abilitySprite.setColor(color);
		/*Set Scale*/abilitySprite.setScale(originalScale);
		/*Set order in batch render*/abilitySprite.setOrderInBatch(9);
		/*Assign sprite to component*/this->gameObject->getComponent<SpriteComponent>()->setSprite(abilitySprite);
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

//Provide an ability to a player
void AbilityComponent::ProvideAbility(PhysicsComponent *PhysComp) {
	//Use the indexer to select and assign an ability power
	switch (indexer)
	{
	case 0:
		//Boost Power
		//Class the function on the PlayerController to engage power-up
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
	//Box has been taken; make it inaccessible.
	readyBox(false);
}

void AbilityComponent::readyBox(bool ready) {
	//Sprite holder
	sre::Sprite _sprite = this->getGameObject()->getComponent<SpriteComponent>()->getSprite();
	totalTime = 0; //Reset timer

	if (!ready) {
		//If not ready, hide it (alpha = 0) and make it inaccessible
		hasProvidedAbility = true;
		_sprite.setColor({ color.x, color.y, color.z, 0 });
		this->getGameObject()->getComponent<SpriteComponent>()->setSprite(_sprite);
	}
	else {
		//Make ready again and select new ability type
		hasProvidedAbility = false;
		SelectAbilityType();
	}
}

//Function to destroy the box; not used but works.
void AbilityComponent::DestroyAbillityBox(GameObject * gameObject) {
	gameObject = this->gameObject;
	RocketBall::gameInstance->deregisterPhysicsComponent(gameObject->getComponent<PhysicsComponent>().get());
	gameObject->removeComponent(gameObject->getComponent<PhysicsComponent>());
	delete gameObject;
}

/* Runtime change of collision filtering if needed
void AbilityComponent::setCollisionFilter(uint16 categoryBits) {
	b2Filter filter = this->getGameObject()->getComponent<PhysicsComponent>()->fixture->GetFilterData();
	filter.categoryBits = categoryBits;
	this->getGameObject()->getComponent<PhysicsComponent>()->fixture->SetFilterData(filter);
}
*/