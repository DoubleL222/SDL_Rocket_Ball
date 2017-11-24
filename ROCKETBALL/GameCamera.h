#pragma once

#include "sre/Camera.hpp"
#include "glm/glm.hpp"
#include "Component.hpp"


class GameCamera : public Component {
public:
	explicit GameCamera(GameObject *gameObject);

	void update(float deltaTime) override;

	void setOffset(glm::vec2 offset);

	sre::Camera& getCamera();

private:
	sre::Camera camera;
	glm::vec2 offset;
};

