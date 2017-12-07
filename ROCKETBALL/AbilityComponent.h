#pragma once

#include "Component.hpp"
#include <Box2D/Box2D.h>


class AbilityComponent : public Component {
private:
	int indexer;

	bool provideAbility = false;

	float InitialmaxSpeed;
	float Initialacceleration;
	float InitialdashSpeed;
	float InitialdashDuration;

	int InitialAirDashesAvailable;
	float32 initialGravity;
	PhysicsComponent *affectedObject = nullptr;
	void update(float deltaTime) override;
	void DestroyAbillityBox(GameObject * gameObject);
	void SelectAndProvide(PhysicsComponent *PhysComp);

public:
	explicit AbilityComponent(GameObject *gameObject);


	void onCollisionStart(PhysicsComponent *comp) override;
	void onCollisionEnd(PhysicsComponent* PhysComp) override;

	void setCollisionFilter(uint16 categoryBits);

	void setSpriteInvis(bool setInvis);

	float abilityDuration;
	float totalTime = 0;
};
