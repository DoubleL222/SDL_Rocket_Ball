//
// Created by Morten Nobel-Jørgensen on 10/10/2017.
//

#include <sre/SpriteAtlas.hpp>
#include <sre/Texture.hpp>
#include <glm/gtx/transform.hpp>
#include "BackgroundComponent.hpp"
#include "RocketBall.h"

using namespace sre;
using namespace glm;

BackgroundComponent::BackgroundComponent()
{
}

void BackgroundComponent::renderBackground(sre::RenderPass &renderPass, float offset) {
	renderPass.draw(batch, glm::translate(vec3(offset, 0, 0)));
}

void BackgroundComponent::init(std::string filename) {
	isInit = true;
	tex = Texture::create().withFile(filename)
		.withFilterSampling(false)
		.build();

	auto atlas = SpriteAtlas::createSingleSprite(tex, "background", vec2(0, 0)); //Change name
	auto sprite = atlas->get("background"); //Changen name
	float scale = RocketBall::windowSize.y / tex->getHeight();
	sprite.setScale({ scale,scale });
	auto batchBuilder = SpriteBatch::create();
	sprite.setPosition(vec2(tex->getWidth() * scale, 0));
	batchBuilder.addSprite(sprite);
	batch = batchBuilder.build();
}

