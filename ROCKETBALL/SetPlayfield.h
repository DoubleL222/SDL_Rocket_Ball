#pragma once

#include "Component.hpp"
#include <Box2D/Box2D.h>

class SetPlayfield {

private:
	void createStaticBox(std::string name, sre::Sprite sprite, glm::vec2 pos, glm::vec2 customScale, glm::vec2 colBuffer, float phyScale, uint16 categoryBits, uint16 maskBits);
	void createWallAndGoals(std::string name, sre::Sprite Goalsprite, sre::Sprite Wallsprite, glm::vec2 pos, glm::vec2 customScale, glm::vec2 colBuffer, glm::vec4 goalColor, float phyScale, uint16 categoryBits, uint16 maskBits);
	void createAbilityBox(std::string name, sre::Sprite sprite, std::shared_ptr<GameObject> obj, glm::vec2 pos, glm::vec2 scale, glm::vec2 colBuffer, const float phyScale);
	glm::vec2 goalSize{};
	float grassColBuffer;
public:
	void createPlayField(std::shared_ptr<sre::SpriteAtlas> _mySpriteAtlas);
	void setGoalSizes(glm::vec2 _goalSizes);
	bool usePlatforms = false;
};