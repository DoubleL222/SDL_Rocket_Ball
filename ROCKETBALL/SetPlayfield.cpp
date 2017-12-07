#include "SetPlayfield.h"

#include <SDL_events.h>
#include <iostream>
#include "GameObject.hpp"
#include "SpriteComponent.hpp"
#include "PhysicsComponent.hpp"
#include "RocketBall.h"
#include "AbilityComponent.h"
#include "SpriteComponent.hpp"

using namespace std;

void SetPlayfield::setGoalSizes(glm::vec2 _goalSizes) {
	goalSize = _goalSizes;
	grassColBuffer = -20.0f;
}

void SetPlayfield::createPlayField(std::shared_ptr<sre::SpriteAtlas> _mySpriteAtlas) {

	//Floor
	createStaticBox(
		/*Name:*/ "grass",
		/*Sprite:*/_mySpriteAtlas->get("Grass.png"),
		/*Position:*/{ 0, (-RocketBall::gameInstance->windowSize.y * 0.5f) + ((_mySpriteAtlas->get("Grass.png").getSpriteSize().y * 0.5f) * 0.5f) },
		/*Scale:*/{ RocketBall::windowSize.x / _mySpriteAtlas->get("Grass.png").getSpriteSize().x, 0.5f },
		/*Collider Buffer:*/{ 0, grassColBuffer },
		/*Physics Scale:*/ RocketBall::gameInstance->getPhysicsScale(),
		RocketBall::gameInstance->BOUNDARY,
		0xFFFF /*0xFFFF is default for collision with everything*/
	);

	//Ceiling
	createStaticBox(
		/*Obj Name:*/ "ceiling",
		/*Sprite:*/_mySpriteAtlas->get("gray.png"),
		/*Position:*/{ 0, (RocketBall::gameInstance->windowSize.y * 0.5f) - ((_mySpriteAtlas->get("gray.png").getSpriteSize().y * 0.5)* 0.5f) },
		/*Scale:*/{ RocketBall::windowSize.x / _mySpriteAtlas->get("gray.png").getSpriteSize().x, 0.5 },
		/*Collider Buffer:*/{ 0,0 },
		/*Physics Scale:*/ RocketBall::gameInstance->getPhysicsScale(),
		RocketBall::gameInstance->BOUNDARY,
		0xFFFF
	);

	//Left wall and goal
	createWallAndGoals(
		"Goal_1",
		_mySpriteAtlas->get("gray.png"),
		_mySpriteAtlas->get("gray.png"),
		{ -1,  ((_mySpriteAtlas->get("gray.png").getSpriteSize().y)) - 20.0f },
		{ goalSize },
		{ 0,0 },
		{ RocketBall::gameInstance->player1Color.x, RocketBall::gameInstance->player1Color.y, RocketBall::gameInstance->player1Color.z, RocketBall::gameInstance->player1Color.w },
		RocketBall::gameInstance->getPhysicsScale(),
		RocketBall::gameInstance->BOUNDARY,
		0xFFFF
	);

	//Right wall and goal
	createWallAndGoals(
		"Goal_2",
		_mySpriteAtlas->get("gray.png"),
		_mySpriteAtlas->get("gray.png"),
		{ 1,  ((_mySpriteAtlas->get("gray.png").getSpriteSize().y)) - 20.0f },
		{ goalSize },
		{ 0,0 },
		{ RocketBall::gameInstance->player2Color.x, RocketBall::gameInstance->player2Color.y, RocketBall::gameInstance->player2Color.z, RocketBall::gameInstance->player2Color.w },
		RocketBall::gameInstance->getPhysicsScale(),
		RocketBall::gameInstance->BOUNDARY,
		0xFFFF
	);
	
	createAbilityBox("Boost_1", _mySpriteAtlas->get("gray.png"), RocketBall::gameInstance->createGameObject(), { 0, ((_mySpriteAtlas->get("gray.png").getSpriteSize().y)) - 20.0f }, { 0.5, 0.5 }, { 0,0 }, RocketBall::gameInstance->getPhysicsScale());
	

	//Platforms
	if (usePlatforms) {

	}
}

void SetPlayfield::createStaticBox(std::string name, sre::Sprite sprite, glm::vec2 pos, glm::vec2 customScale, glm::vec2 colBuffer, float phyScale, uint16 categoryBits, uint16 maskBits) {
	auto staticBox = sprite;
	auto staticBox_obj = RocketBall::gameInstance->createGameObject();
	staticBox_obj->name = name;
	auto staticBoxSpriteComp = staticBox_obj->addComponent<SpriteComponent>();
	staticBox.setScale({ customScale });
	staticBoxSpriteComp->setSprite(staticBox);
	staticBox_obj->setPosition({ pos.x, pos.y });
	glm::vec2 scaleCol{
		(staticBox.getSpriteSize().x * staticBox.getScale().x / 2) + colBuffer.x,
		(staticBox.getSpriteSize().y * staticBox.getScale().y / 2) + colBuffer.y
	};
	auto staticBoxPhysicsComp = staticBox_obj->addComponent<PhysicsComponent>();
	staticBoxPhysicsComp->initBox(b2BodyType::b2_staticBody, scaleCol / phyScale, staticBox_obj->getPosition() / phyScale, 0.0f, categoryBits, maskBits);
}


void SetPlayfield::createWallAndGoals(std::string name, sre::Sprite Goalsprite, sre::Sprite Wallsprite, glm::vec2 pos, glm::vec2 customScale, glm::vec2 colBuffer, glm::vec4 playerColor, float phyScale, uint16 categoryBits, uint16 maskBits) {
	float wallScaleX, wallScaleY;

	auto goalBox = Goalsprite;
	auto staticBox = Wallsprite;

	auto goalBox_obj = RocketBall::gameInstance->createGameObject();
	auto staticBox_obj = RocketBall::gameInstance->createGameObject();
	auto boundingColliderRight_obj = RocketBall::gameInstance->createGameObject();
	auto boundingColliderLeft_obj = RocketBall::gameInstance->createGameObject();

	goalBox_obj->name = name;
	staticBox_obj->name = "Wall";
	boundingColliderRight_obj->name = "BoundingColliderRight";
	boundingColliderLeft_obj->name = "BoundingColliderLeft";

	auto goalBoxSpriteComp = goalBox_obj->addComponent<SpriteComponent>();
	auto staticBoxSpriteComp = staticBox_obj->addComponent<SpriteComponent>();
	goalBox.setColor({ playerColor.x, playerColor.y, playerColor.z, 0.5 });
	goalBox.setScale({ customScale });
	wallScaleX = goalBox.getScale().x;
	wallScaleY = goalBox.getScale().y * 4;
	staticBox.setScale({ wallScaleX, wallScaleY });

	boundingColliderRight_obj->setPosition({ RocketBall::gameInstance->windowSize.x * 0.5f, 0.0f });
	boundingColliderLeft_obj->setPosition({ -RocketBall::gameInstance->windowSize.x * 0.5f, 0.0f });
	auto boundingColRight_Physics = boundingColliderRight_obj->addComponent<PhysicsComponent>();
	auto boundingColLeft_Physics = boundingColliderLeft_obj->addComponent<PhysicsComponent>();

	boundingColRight_Physics->initBox(b2BodyType::b2_staticBody, { 0,RocketBall::gameInstance->windowSize.y / phyScale }, boundingColliderRight_obj->getPosition() / phyScale, 0.0f, categoryBits, maskBits);
	boundingColLeft_Physics->initBox(b2BodyType::b2_staticBody, { 0,RocketBall::gameInstance->windowSize.y / phyScale }, boundingColliderLeft_obj->getPosition() / phyScale, 0.0f, categoryBits, maskBits);

	goalBoxSpriteComp->setSprite(goalBox);
	staticBoxSpriteComp->setSprite(staticBox);

	goalBox_obj->setPosition(
	{ pos.x * (((RocketBall::gameInstance->windowSize.x *0.5f)) - (Goalsprite.getSpriteSize().x * 0.5f)*customScale.x),
	  pos.y + (-(RocketBall::gameInstance->windowSize.y * 0.5f) + (Goalsprite.getSpriteSize().x * 0.5f)*customScale.y) }
	);

	staticBox_obj->setPosition(
	{ goalBox_obj->getPosition().x,
	  goalBox_obj->getPosition().y + (Goalsprite.getSpriteSize().y)* customScale.y * 2.5 }
	);

	glm::vec2 scaleCol_Goal{
		(goalBox.getSpriteSize().x * goalBox.getScale().x / 2) + colBuffer.x,
		(goalBox.getSpriteSize().y * goalBox.getScale().y / 2) + colBuffer.y };
	glm::vec2 scaleCol_staticBox{
		(staticBox.getSpriteSize().x * staticBox.getScale().x / 2) + colBuffer.x,
		(staticBox.getSpriteSize().y * staticBox.getScale().y / 2) + colBuffer.y };

	auto goalBoxPhysicsComp = goalBox_obj->addComponent<PhysicsComponent>();
	goalBoxPhysicsComp->initBox(b2BodyType::b2_staticBody, scaleCol_Goal / phyScale, goalBox_obj->getPosition() / phyScale, 0.0f, categoryBits, maskBits);
	goalBoxPhysicsComp->setSensor(true);
	auto staticBoxPhysicsComp = staticBox_obj->addComponent<PhysicsComponent>();
	staticBoxPhysicsComp->initBox(b2BodyType::b2_staticBody, scaleCol_staticBox / phyScale, staticBox_obj->getPosition() / phyScale, 0.0f, categoryBits, maskBits);

}


void SetPlayfield::createAbilityBox(std::string name, sre::Sprite sprite, std::shared_ptr<GameObject> obj, glm::vec2 pos, glm::vec2 scale, glm::vec2 colBuffer, const float phyScale) {
	auto abilityBox = sprite;
	abilityBox.setScale({ scale.x, scale.y });
	auto abilityBox_obj = obj;
	abilityBox_obj->name = name;
	auto abilityBoxSpriteComp = abilityBox_obj->addComponent<SpriteComponent>();
	abilityBox_obj->setPosition({pos.x, pos.y + (-(RocketBall::gameInstance->windowSize.y * 0.5f) + (sprite.getSpriteSize().x * 0.5f)*scale.y) });
	glm::vec2 scaleCol{ (abilityBox.getSpriteSize().x * abilityBox.getScale().x / 2) + colBuffer.x, (abilityBox.getSpriteSize().y * abilityBox.getScale().y / 2) + colBuffer.y };
	abilityBoxSpriteComp->setSprite(abilityBox);
	auto abilityBox_physics = abilityBox_obj->addComponent<PhysicsComponent>();
	abilityBox_physics->initBox(b2_staticBody, scaleCol / phyScale, { abilityBox_obj->getPosition().x / phyScale, abilityBox_obj->getPosition().y / phyScale }, 0, RocketBall::gameInstance->ABILITYBOX, RocketBall::gameInstance->ABILITYBOX | RocketBall::gameInstance->BOUNDARY | RocketBall::gameInstance->PLAYER);
	abilityBox_physics->setSensor(true);
	auto abilityBoxBehaviour = abilityBox_obj->addComponent<AbilityComponent>();
}
