#pragma once

#include "Component.hpp"
#include <Box2D/Box2D.h>


class AbilityComponent : public Component {
private:
	int indexer;

	bool hasProvidedAbility = false;

	sre::Sprite abilitySprite;

	void DestroyAbillityBox(GameObject * gameObject);
	void ProvideAbility(PhysicsComponent *PhysComp);

	glm::vec4 color{1,1,1,1};
	float totalTime = 0.0f;
	float cooldown = 1.0f;
	bool destroy = false;

	//void setCollisionFilter(uint16 categoryBits);
	bool largeBoostRecharge = false;

public:
	explicit AbilityComponent(GameObject *gameObject);

	void update(float deltaTime) override;

	void onCollisionStart(PhysicsComponent *comp) override;
	void setBoostTypeInClassic(bool isLargeBoost);
	void SelectAbilityType();
	void onCollisionEnd(PhysicsComponent* PhysComp) override;

	void readyBox(bool ready);

	void setCooldown(float cooldown);

};
