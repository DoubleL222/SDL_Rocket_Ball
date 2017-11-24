#include "GameCamera.h"
#include "RocketBall.h"

using namespace glm;

GameCamera::GameCamera(GameObject *gameObject)
	: Component(gameObject)
{
	camera.setOrthographicProjection(RocketBall::windowSize.y / 2, -1, 1);
}

sre::Camera &GameCamera::getCamera() {
	return camera;
}

void GameCamera::update(float deltaTime) {
	glm::vec2 position;

	position.x += offset.x;
	position.y = offset.y;

	gameObject->setPosition(position);
	vec3 eye(position, 0);
	vec3 at(position, -1);
	vec3 up(0, 1, 0);
	camera.lookAt(eye, at, up);
}


void GameCamera::setOffset(glm::vec2 offset) {
	this->offset = offset;
}


