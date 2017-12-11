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

	isInit = true; //ensure only 1 init
	
	//create texture
	tex = Texture::create().withFile(filename)
		.withFilterSampling(false)
		.build();

	//Create a single sprite
	auto atlas = SpriteAtlas::createSingleSprite(tex, "backgroundLayer", vec2(0, 0));
	auto sprite = atlas->get("backgroundLayer");
	sprite.setOrderInBatch(-1); //Set behind everything

	//Stretch to fit the whole screen
	if (stretchToFit) {
		float scaleX = RocketBall::windowSize.x / tex->getWidth();
		float scaleY = RocketBall::windowSize.y / tex->getHeight();
		sprite.setScale({ scaleX,scaleY });
	}
	else {
		//Otherwise just use origin size
		float scaleX = 1.0f;
		float scaleY = 1.0f;
		sprite.setScale({ scaleX,scaleY });
	}
	//Create a spriteBatchBuilder
	auto batchBuilder = SpriteBatch::create();

	//Set position
	sprite.setPosition(vec2(initPositionX, initPositionY));

	//Assign the sprite to the batch
	batchBuilder.addSprite(sprite);
	batch = batchBuilder.build(); //Build batch..
}
