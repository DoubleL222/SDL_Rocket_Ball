//
// Created by Morten Nobel-Jørgensen on 10/10/2017.
//

#include <sre/SpriteAtlas.hpp>
#include <sre/Texture.hpp>
#include <glm/gtx/transform.hpp>
#include "BackgroundComponent.hpp"
#include "RocketBall.h"
#include <iostream>


using namespace sre;
using namespace glm;
using namespace std;

BackgroundComponent::BackgroundComponent()
{
}

void BackgroundComponent::renderBackground(sre::RenderPass &renderPass, float offset) {
	renderPass.draw(batch, glm::translate(vec3(offset, 0, 0)));
}

void BackgroundComponent::init(std::string filename, float initPositionX, float initPositionY, bool stretchToFit) {

	isInit = true;
	tex = Texture::create().withFile(filename)
		.withFilterSampling(false)
		.build();

	auto atlas = SpriteAtlas::createSingleSprite(tex, "backgroundLayer", vec2(0, 0));
	auto sprite = atlas->get("backgroundLayer");

	if (stretchToFit) {
		float scaleX = RocketBall::windowSize.x / tex->getWidth();
		float scaleY = RocketBall::windowSize.y / tex->getHeight();
		sprite.setScale({ scaleX,scaleY });
	}
	else {
		float scaleX = 1.0f;
		float scaleY = 1.0f;
		sprite.setScale({ scaleX,scaleY });
	}
	auto batchBuilder = SpriteBatch::create();

	sprite.setPosition(vec2(initPositionX, initPositionY));

	batchBuilder.addSprite(sprite);
	batch = batchBuilder.build();
}
