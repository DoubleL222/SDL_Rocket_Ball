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
	grassColBuffer = 20.0f;
	playFieldInit = true;
}

void SetPlayfield::createPlayField(std::shared_ptr<sre::SpriteAtlas> _mySpriteAtlas) {

	//Floor
	createStaticBox(
		/*Name:*/ "grass",
		/*Sprite:*/_mySpriteAtlas->get("Grass.png"),
		/*Position:*/{ 0, (-RocketBall::gameInstance->windowSize.y * 0.5f) + ((_mySpriteAtlas->get("Grass.png").getSpriteSize().y * 0.5f) * 0.5f) },
		/*Scale:*/{ RocketBall::windowSize.x / _mySpriteAtlas->get("Grass.png").getSpriteSize().x, 0.5f },
		/*Collider Buffer:*/{ 0, -grassColBuffer },
		/*Rotation*/0.0f,
		/*Physics Scale:*/ RocketBall::gameInstance->getPhysicsScale(),
		/*CategoryBits:*/RocketBall::gameInstance->BOUNDARY,
		/*MaskBits:*/0xFFFF /*0xFFFF is default for collision with everything*/
	);

	//Ceiling
	createStaticBox(
		/*Obj Name:*/ "ceiling",
		/*Sprite:*/_mySpriteAtlas->get("gray.png"),
		/*Position:*/{ 0, (RocketBall::gameInstance->windowSize.y * 0.5f) - ((_mySpriteAtlas->get("gray.png").getSpriteSize().y * 0.5)* 0.5f) },
		/*Scale:*/{ RocketBall::windowSize.x / _mySpriteAtlas->get("gray.png").getSpriteSize().x, RocketBall::gameInstance->goalSizes.x },
		/*Collider Buffer:*/{ 0,0 },
		/*Rotation*/0.0f,
		/*Physics Scale:*/ RocketBall::gameInstance->getPhysicsScale(),
		/*CategoryBits:*/RocketBall::gameInstance->BOUNDARY,
		/*MaskBits:*/0xFFFF
	);

	//Left wall and goal
	createWallAndGoals(
		"Goal_1",
		_mySpriteAtlas->get("goalMain.png"),
		_mySpriteAtlas->get("gray.png"),
		{ -1,  ((_mySpriteAtlas->get("goalMain.png").getSpriteSize().y)) - grassColBuffer }, /*-1 to opposite side*/
		{ goalSize },
		RocketBall::gameInstance->botYBoxSize,
		{ 0,0 },
		{ RocketBall::gameInstance->player1Color.x, RocketBall::gameInstance->player1Color.y, RocketBall::gameInstance->player1Color.z, RocketBall::gameInstance->player1Color.w },
		RocketBall::gameInstance->BOUNDARY,
		0xFFFF
	);

	//Right wall and goal
	createWallAndGoals(
		"Goal_2",
		_mySpriteAtlas->get("goalMain.png"),
		_mySpriteAtlas->get("gray.png"),
		{ 1,  ((_mySpriteAtlas->get("goalMain.png").getSpriteSize().y)) - grassColBuffer },
		{ goalSize },
		RocketBall::gameInstance->botYBoxSize,
		{ 0,0 },
		{ RocketBall::gameInstance->player2Color.x, RocketBall::gameInstance->player2Color.y, RocketBall::gameInstance->player2Color.z, RocketBall::gameInstance->player2Color.w },
		RocketBall::gameInstance->BOUNDARY,
		0xFFFF
	);

	if (useFiveAbilityBoxes) {
		createAbilityBox(
			/*Name*/"AbiBox_0",
			/*Sprite*/_mySpriteAtlas->get("gray.png"), _mySpriteAtlas->get("PowerUpPlatformGreen.png"),
			/*GameObject*/RocketBall::gameInstance->AbiBox_0,
			/*Position*/{ 0, ((_mySpriteAtlas->get("gray.png").getSpriteSize().y)) - grassColBuffer },
			/*Scale*/{ 0.5, 0.5 },
			/*Col Buffer*/{ 0,0 },
			/*PhysScale*/RocketBall::gameInstance->getPhysicsScale(),
			/*Is large boost?*/true,
			/*Cooldown*/10.0f
		);

		//Left box 1
		createAbilityBox(
			/*Name*/"AbiBox_1",
			/*Sprite*/_mySpriteAtlas->get("gray.png"), _mySpriteAtlas->get("PowerUpPlatformGreen.png"),
			/*GameObject*/RocketBall::gameInstance->AbiBox_1,
			/*Position*/{ -RocketBall::gameInstance->windowSize.x * 0.15f, ((_mySpriteAtlas->get("gray.png").getSpriteSize().y)) - grassColBuffer },
			/*Scale*/{ 0.5, 0.5 },
			/*Col Buffer*/{ 0,0 },
			/*PhysScale*/RocketBall::gameInstance->getPhysicsScale(),
			/*Is large boost?*/false,
			/*Cooldown*/5.0f
		);

		//Left box 2
		createAbilityBox(
			/*Name*/"AbiBox_2",
			/*Sprite*/_mySpriteAtlas->get("gray.png"), _mySpriteAtlas->get("PowerUpPlatformGreen.png"),
			/*GameObject*/RocketBall::gameInstance->AbiBox_2,
			/*Position*/{ -RocketBall::gameInstance->windowSize.x * 0.30f, ((_mySpriteAtlas->get("gray.png").getSpriteSize().y)) - grassColBuffer },
			/*Scale*/{ 0.5, 0.5 },
			/*Col Buffer*/{ 0,0 },
			/*PhysScale*/RocketBall::gameInstance->getPhysicsScale(),
			/*Is large boost?*/false,
			/*Cooldown*/5.0f
		);

		//Right box 1
		createAbilityBox(
			/*Name*/"AbiBox_3",
			/*Sprite*/_mySpriteAtlas->get("gray.png"), _mySpriteAtlas->get("PowerUpPlatformGreen.png"),
			/*GameObject*/RocketBall::gameInstance->AbiBox_3,
			/*Position*/{ RocketBall::gameInstance->windowSize.x * 0.15f, ((_mySpriteAtlas->get("gray.png").getSpriteSize().y)) - grassColBuffer },
			/*Scale*/{ 0.5, 0.5 },
			/*Col Buffer*/{ 0,0 },
			/*PhysScale*/RocketBall::gameInstance->getPhysicsScale(),
			/*Is large boost?*/false,
			/*Cooldown*/5.0f
		);

		//Right box 2
		createAbilityBox(
			/*Name*/"AbiBox_4",
			/*Sprite*/_mySpriteAtlas->get("gray.png"), _mySpriteAtlas->get("PowerUpPlatformGreen.png"),
			/*GameObject*/RocketBall::gameInstance->AbiBox_4,
			/*Position*/{ RocketBall::gameInstance->windowSize.x * 0.30f, ((_mySpriteAtlas->get("gray.png").getSpriteSize().y)) - grassColBuffer },
			/*Scale*/{ 0.5, 0.5 },
			/*Col Buffer*/{ 0,0 },
			/*PhysScale*/RocketBall::gameInstance->getPhysicsScale(),
			/*Is large boost?*/false,
			/*Cooldown*/5.0f
		);
	}
	else {
		createAbilityBox(
			/*Name*/"AbiBox_0",
			/*Sprite*/_mySpriteAtlas->get("gray.png"), _mySpriteAtlas->get("PowerUpPlatformGreen.png"),
			/*GameObject*/RocketBall::gameInstance->AbiBox_0,
			/*Position*/{ 0, ((_mySpriteAtlas->get("gray.png").getSpriteSize().y)) - grassColBuffer },
			/*Scale*/{ 0.5, 0.5 },
			/*Col Buffer*/{ 0,0 },
			/*PhysScale*/RocketBall::gameInstance->getPhysicsScale(),
			/*Is large boost?*/true,
			/*Cooldown*/10.0f
		);

		//Left box 1
		createAbilityBox(
			/*Name*/"AbiBox_1",
			/*Sprite*/_mySpriteAtlas->get("gray.png"), _mySpriteAtlas->get("PowerUpPlatformGreen.png"),
			/*GameObject*/RocketBall::gameInstance->AbiBox_1,
			/*Position*/{ -RocketBall::gameInstance->windowSize.x * 0.25f, ((_mySpriteAtlas->get("gray.png").getSpriteSize().y)) - grassColBuffer },
			/*Scale*/{ 0.5, 0.5 },
			/*Col Buffer*/{ 0,0 },
			/*PhysScale*/RocketBall::gameInstance->getPhysicsScale(),
			/*Is large boost?*/false,
			/*Cooldown*/5.0f
		);

		//Right box 1
		createAbilityBox(
			/*Name*/"AbiBox_3",
			/*Sprite*/_mySpriteAtlas->get("gray.png"), _mySpriteAtlas->get("PowerUpPlatformGreen.png"),
			/*GameObject*/RocketBall::gameInstance->AbiBox_2,
			/*Position*/{ RocketBall::gameInstance->windowSize.x * 0.25f, ((_mySpriteAtlas->get("gray.png").getSpriteSize().y)) - grassColBuffer },
			/*Scale*/{ 0.5, 0.5 },
			/*Col Buffer*/{ 0,0 },
			/*PhysScale*/RocketBall::gameInstance->getPhysicsScale(),
			/*Is large boost?*/false,
			/*Cooldown*/5.0f
		);
	}

	//Left Corner Box
	createStaticBox(
		/*Obj Name:*/ "CornerBoxLeft",
		/*Sprite:*/_mySpriteAtlas->get("gray.png"),
		/*Position:*/{ -RocketBall::gameInstance->windowSize.x *0.5 , RocketBall::gameInstance->windowSize.y *0.5 },
		/*Scale:*/{ 3,3 },
		/*Collider Buffer:*/{ 0,0 },
		/*Rotation*/135.0f,
		/*Physics Scale:*/ RocketBall::gameInstance->getPhysicsScale(),
		/*CategoryBits:*/RocketBall::gameInstance->BOUNDARY,
		/*MaskBits:*/0xFFFF
	);

	//Right Corner Box
	createStaticBox(
		/*Obj Name:*/ "CornerBoxRight",
		/*Sprite:*/_mySpriteAtlas->get("gray.png"),
		/*Position:*/{ RocketBall::gameInstance->windowSize.x *0.5 , RocketBall::gameInstance->windowSize.y *0.5 },
		/*Scale:*/{ 3,3 },
		/*Collider Buffer:*/{ 0,0 },
		/*Rotation*/45.0f,
		/*Physics Scale:*/ RocketBall::gameInstance->getPhysicsScale(),
		/*CategoryBits:*/RocketBall::gameInstance->BOUNDARY,
		/*MaskBits:*/0xFFFF
	);
}

////////////// FUNCTION TO CREATE A STATIC BOX
void SetPlayfield::createStaticBox(std::string name, sre::Sprite sprite, glm::vec2 pos, glm::vec2 customScale, glm::vec2 colBuffer, float _rotation, float phyScale, uint16 categoryBits, uint16 maskBits) {
	auto staticBox = sprite; //Sprite to be used
	auto staticBox_obj = RocketBall::gameInstance->createGameObject(); //Create a new gameobject
	staticBox_obj->name = name; //Assign name
	auto staticBoxSpriteComp = staticBox_obj->addComponent<SpriteComponent>(); //Add sprite component to gameobject
	staticBox.setScale({ customScale }); //Set the scale of the object
	staticBoxSpriteComp->setSprite(staticBox); //Assign the sprite to the SpriteComponent
	staticBox_obj->setPosition({ pos.x, pos.y }); //Set the position of the object
	staticBox_obj->setRotation(_rotation); //Set rotation of the object
	glm::vec2 scaleCol{ /*Set scale for collision including an optional buffer*/
		(staticBox.getSpriteSize().x * staticBox.getScale().x / 2) + colBuffer.x,
		(staticBox.getSpriteSize().y * staticBox.getScale().y / 2) + colBuffer.y
	};
	auto staticBoxPhysicsComp = staticBox_obj->addComponent<PhysicsComponent>(); //Assign PhysicsComponent to gameobject
	//Initializes a physicsbox based on parameters; set the type, scale, rotation and collision mask
	staticBoxPhysicsComp->initBox(
		b2BodyType::b2_staticBody, /*Type*/
		scaleCol / phyScale, /*scale*/
		staticBox_obj->getPosition() / phyScale, /*position*/
		glm::radians(_rotation), /*rotation*/
		0.0f, /*density*/
		categoryBits, maskBits /*collision filtering*/
	);
}

////////////// FUNCTION TO CREATE A BOTTOM WALL; A GOAL; AN UPPER WALL; BOUNDING COLLIDERS
void SetPlayfield::createWallAndGoals(std::string name, sre::Sprite Goalsprite, sre::Sprite Wallsprite, glm::vec2 pos, glm::vec2 customScaleGoal, float botYScale, glm::vec2 colBuffer, glm::vec4 playerColor, uint16 categoryBits, uint16 maskBits) {
	float wallScaleX, wallScaleY; //Scales for the walls

	//Sprites 
	auto goalBox = Goalsprite;
	auto staticBox_1 = Wallsprite;
	auto staticBox_2 = Wallsprite;

	//Create GameObjects
	auto goalBox_obj = RocketBall::gameInstance->createGameObject();
	auto staticBox_obj_1 = RocketBall::gameInstance->createGameObject();
	auto staticBox_obj_2 = RocketBall::gameInstance->createGameObject();

	//Assign Names
	goalBox_obj->name = name;
	staticBox_obj_1->name = "Wall1";
	staticBox_obj_2->name = "Wall2";

	//Assign SpriteComponents to each GameObject
	auto goalBoxSpriteComp = goalBox_obj->addComponent<SpriteComponent>();
	auto staticBoxSpriteComp_1 = staticBox_obj_1->addComponent<SpriteComponent>();
	auto staticBoxSpriteComp_2 = staticBox_obj_2->addComponent<SpriteComponent>();

	//Flip the goal sprite to face the other way if on the other side
	if (name == "Goal_2")
		goalBox.setFlip(glm::vec2(-1, 0));

	goalBox.setColor({ playerColor.x, playerColor.y, playerColor.z, 0.5 }); //Assign color based on player
	goalBox.setScale({ customScaleGoal }); //Set scale
	wallScaleX = goalBox.getScale().x; //Scale for wall
	//Scale for wall; * 4 is an arbitrary number to ensure the wall goes outside of camera.
	wallScaleY = goalBox.getScale().y * 4; 
	staticBox_1.setScale({ wallScaleX, wallScaleY }); //Assign scale using values
	staticBox_2.setScale({ wallScaleX, botYScale }); // -----||-----

	goalBox.setOrderInBatch(20); //Set order in batch to be infront of car and soccerball
	//Assigns the sprites to the spritecomponents
	goalBoxSpriteComp->setSprite(goalBox);
	staticBoxSpriteComp_1->setSprite(staticBox_1);
	staticBoxSpriteComp_2->setSprite(staticBox_2);

	//Sets the position of the wallbox to fit the position of the grass including the buffer
	//This is the bottom wall
	staticBox_obj_2->setPosition(
	{ pos.x * (((RocketBall::gameInstance->windowSize.x *0.5f)) - (Wallsprite.getSpriteSize().x * 0.5f)*wallScaleX),
		pos.y + (-(RocketBall::gameInstance->windowSize.y * 0.5f) + (Goalsprite.getSpriteSize().y * 0.5f)*botYScale) }
	);

	//Computes and sets the position of the goal to be stacked ontop of staticBox_obj_2
	goalBox_obj->setPosition(
	{ staticBox_obj_2->getPosition().x,
		staticBox_obj_2->getPosition().y + ((Wallsprite.getSpriteSize().y *0.5)* botYScale) + ((Goalsprite.getSpriteSize().y *0.5)* customScaleGoal.y) }
	);

	//Computes and sets the position to be ontop of the the goal with a large Y scale
	staticBox_obj_1->setPosition(
	{ goalBox_obj->getPosition().x,
		/*2.5 accounts for the arbitrary number for the scaling of the upper wall (above the goal)*/
	  goalBox_obj->getPosition().y + (Goalsprite.getSpriteSize().y) * customScaleGoal.y * 2.5 }
	);

	//Computes the required scale for the phyiscs to fit the sprites
	glm::vec2 scaleCol_Goal{
		(goalBox.getSpriteSize().x * goalBox.getScale().x / 2) + colBuffer.x,
		(goalBox.getSpriteSize().y * goalBox.getScale().y / 2) + colBuffer.y };
	glm::vec2 scaleCol_staticBox_1{
		(staticBox_1.getSpriteSize().x * staticBox_1.getScale().x / 2) + colBuffer.x,
		(staticBox_1.getSpriteSize().y * staticBox_1.getScale().y / 2) + colBuffer.y };
	glm::vec2 scaleCol_staticBox_2{
		(staticBox_2.getSpriteSize().x * staticBox_2.getScale().x / 2) + colBuffer.x,
		(staticBox_2.getSpriteSize().y * staticBox_2.getScale().y / 2) + colBuffer.y };


	float phyScale = RocketBall::gameInstance->getPhysicsScale(); //PhysicsScale from Singleton

	//Assigns the PhysicsCompnent and inits the boxes to create colliders. All static.
	auto goalBoxPhysicsComp = goalBox_obj->addComponent<PhysicsComponent>();
	goalBoxPhysicsComp->initBox(b2BodyType::b2_staticBody, scaleCol_Goal / phyScale, goalBox_obj->getPosition() / phyScale, 0.0f, categoryBits, maskBits);
	goalBoxPhysicsComp->setSensor(true); //Set as a trigger for the goal
	auto staticBoxPhysicsComp_1 = staticBox_obj_1->addComponent<PhysicsComponent>();
	staticBoxPhysicsComp_1->initBox(b2BodyType::b2_staticBody, scaleCol_staticBox_1 / phyScale, staticBox_obj_1->getPosition() / phyScale, 0.0f, categoryBits, maskBits);
	auto staticBoxPhysicsComp_2 = staticBox_obj_2->addComponent<PhysicsComponent>();
	staticBoxPhysicsComp_2->initBox(b2BodyType::b2_staticBody, scaleCol_staticBox_2 / phyScale, staticBox_obj_2->getPosition() / phyScale, 0.0f, categoryBits, maskBits);

	//Bounding Colliders: Ensures the players cannot pass out of the game area through the goals (as they are triggers)
	auto boundingCollider_obj = RocketBall::gameInstance->createGameObject();
	boundingCollider_obj->name = "BoundingCollider";
	boundingCollider_obj->setPosition({ pos.x * RocketBall::gameInstance->windowSize.x * 0.5f, 0.0f });
	auto boundingColRight_Physics = boundingCollider_obj->addComponent<PhysicsComponent>();
	boundingColRight_Physics->initBox(b2BodyType::b2_staticBody, { 0,RocketBall::gameInstance->windowSize.y / phyScale }, boundingCollider_obj->getPosition() / phyScale, 0.0f, categoryBits, maskBits);
}

////////////// FUNCTION TO CREATE AN ABILITYBOX
void SetPlayfield::createAbilityBox(std::string name, sre::Sprite sprite, sre::Sprite spritePlatform, std::shared_ptr<GameObject> &abilityBox_obj, glm::vec2 pos, glm::vec2 scale, glm::vec2 colBuffer, const float phyScale, bool isLargeBoost, float _cooldown) {
	auto abilityBox = sprite; //Sprite
	abilityBox_obj = RocketBall::gameInstance->createGameObject(); //create new gameobject
	abilityBox_obj->name = name; //assign name
	auto abiBoxPlatform = RocketBall::gameInstance->createGameObject(); //create gameobject to platform
	abiBoxPlatform->name = name + "_abiPlatform"; //assign name

	//Assign spritecomponents
	auto abilityBoxSpriteComp = abilityBox_obj->addComponent<SpriteComponent>();
	auto abilityPlatformSpriteComp = abiBoxPlatform->addComponent<SpriteComponent>();

	//Based on whether it is a large boost, the size should be altered in classic gamemode
	if (isLargeBoost && RocketBall::gameInstance->gameModeClassic)
		abilityBox.setScale({ scale.x * 1.25, scale.y * 1.25 });
	else
		abilityBox.setScale({ scale });

	//Set scale of platform
	spritePlatform.setScale({ scale.x*1.5 , scale.y *1.5 });

	spritePlatform.setOrderInBatch(5); //Set order to be behind the abilityboxes, player, ball etc.
	abilityPlatformSpriteComp->setSprite(spritePlatform); //Assign sprite to spritecomponent
	//Set position of the platform and ability pellets
	abilityBox_obj->setPosition({ pos.x, pos.y + (-(RocketBall::gameInstance->windowSize.y * 0.5f) + (sprite.getSpriteSize().x * 0.5f)*scale.y) });
	abiBoxPlatform->setPosition({ pos.x, abilityBox_obj->getPosition().y - (abilityBox.getSpriteSize().y*scale.y)*0.5 });
	abilityBox.setOrderInBatch(9); //Set order of rendering
	abilityBoxSpriteComp->setSprite(abilityBox); //assign sprite to spritecomponent

	//Determine size for collider
	glm::vec2 scaleCol_Goal{
		(abilityBox.getSpriteSize().x * abilityBox.getScale().x / 2) + colBuffer.x,
		(abilityBox.getSpriteSize().y * abilityBox.getScale().y / 2) + colBuffer.y };

	//Assign phyiscs component and set values
	auto abilityBox_physics = abilityBox_obj->addComponent<PhysicsComponent>();
	abilityBox_physics->initBox(b2_staticBody, scaleCol_Goal / phyScale, { abilityBox_obj->getPosition().x / phyScale, abilityBox_obj->getPosition().y / phyScale }, 0, RocketBall::gameInstance->ABILITYBOX, RocketBall::gameInstance->ABILITYBOX | RocketBall::gameInstance->BOUNDARY | RocketBall::gameInstance->PLAYER);
	abilityBox_physics->setSensor(true); //set sensor to interact with player

	//Add the ability behaviour componenet; see class for more information
	auto abilityBoxBehaviour = abilityBox_obj->addComponent<AbilityComponent>();
	abilityBoxBehaviour->setBoostTypeInClassic(isLargeBoost);
	abilityBoxBehaviour->setCooldown(_cooldown);
	abilityBoxBehaviour->SelectAbilityType();
}

//Function to set the ability boxes ready; if true, boxes appear, if false, they disppaer.
void SetPlayfield::readyAbilityBoxes(bool readyTheBox) {
	if (useFiveAbilityBoxes) {
		RocketBall::gameInstance->AbiBox_0->getComponent<AbilityComponent>()->readyBox(readyTheBox);
		RocketBall::gameInstance->AbiBox_1->getComponent<AbilityComponent>()->readyBox(readyTheBox);
		RocketBall::gameInstance->AbiBox_2->getComponent<AbilityComponent>()->readyBox(readyTheBox);
		RocketBall::gameInstance->AbiBox_3->getComponent<AbilityComponent>()->readyBox(readyTheBox);
		RocketBall::gameInstance->AbiBox_4->getComponent<AbilityComponent>()->readyBox(readyTheBox);
	}
	else {
		RocketBall::gameInstance->AbiBox_0->getComponent<AbilityComponent>()->readyBox(readyTheBox);
		RocketBall::gameInstance->AbiBox_1->getComponent<AbilityComponent>()->readyBox(readyTheBox);
		RocketBall::gameInstance->AbiBox_2->getComponent<AbilityComponent>()->readyBox(readyTheBox);
	}
}
