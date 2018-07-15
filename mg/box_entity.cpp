#include "box_entity.h"
#include <iostream>

static string returnLookUp(AnimationType type) {
	switch (type) {
	case spawn: return "spawn";
	case idle: return "idle";
	case move_left: return "move_left";
	case move_right: return "move_right";
	case death: return "death";
	}
	err::logMessage("returnLookUp() envoked with AnimationType not supported: " + type);
	return "idle";
}

bool checkPointInRect(CUS_Point point, SDL_Rect rectangle) {
	if (point.x > rectangle.x && point.x < (rectangle.w + rectangle.x)) {
		if (point.y > rectangle.y && point.y < (rectangle.h + rectangle.y)) {
			return true;
		}
	}
	return false;
}

void BoxEntity::updateBox() {
	alpha += alphaGrowth;

	if (!internalScale) {
		visiblePosition_Entity.x = (int)position_Entity.x;
		visiblePosition_Entity.y = (int)position_Entity.y;
		visibleBound_Entity.w = animationSet->getAnimation(currentFrameType)->getWidth();
		visibleBound_Entity.h = animationSet->getAnimation(currentFrameType)->getHeight();
		visibleBound_Entity.x = visiblePosition_Entity.x - visibleBound_Entity.w / 2;
		visibleBound_Entity.y = visiblePosition_Entity.y - visibleBound_Entity.h / 2;
	}
	else {
		visiblePosition_Entity.x = (int)position_Entity.x;
		visiblePosition_Entity.y = (int)position_Entity.y;
		visibleBound_Entity.w = (int)(animationSet->getAnimation(currentFrameType)->getWidth() * internalScale);
		visibleBound_Entity.h = (int)(animationSet->getAnimation(currentFrameType)->getHeight() * internalScale);
		visibleBound_Entity.x = visiblePosition_Entity.x - visibleBound_Entity.w / 2;
		visibleBound_Entity.y = visiblePosition_Entity.y - visibleBound_Entity.h / 2;
	}

}

void BoxEntity::itCurrentFrame() {
	skippingFrame++;
	if (skippingFrame >= animationSet->getAnimation(currentFrameType)->getSkip()) {
		skippingFrame = 0;
		currentNumberFrame++;
		if (currentNumberFrame >= animationSet->getAnimation(currentFrameType)->getTotalFrames()) {
			animationPlayed = true;
			playAnimation(backupFrameType);
		}
	}
}

void BoxEntity::playAnimation(AnimationType newAnimation) {
	if (animationSet->getAnimation(newAnimation) == NULL) {
		currentFrameType = idle;
		err::logMessage("playAnimation failed");
		err::logMessage( returnLookUp(newAnimation) );
	}
	else {
		currentFrameType = newAnimation;
	}
	skippingFrame = 0;
	currentNumberFrame = 0;
	updateBox();
}

void BoxEntity::switchAnimation(AnimationType newAnimation) {
	if (animationSet->getAnimation(newAnimation) == NULL) {
		currentFrameType = idle;
		err::logMessage("playAnimation failed: no annimation exists");
		err::logMessage( returnLookUp(newAnimation) );
	}
	else {
		currentFrameType = newAnimation;
		backupFrameType = newAnimation;
	}
	itCurrentFrame();
	updateBox();
}

void BoxEntity::setAnimationSet(AnimationAssignment* animation_map_param) {
	if (animation_map_param == NULL) {
		err::logMessage("playAnimation failed : no annimation exists, animation that failed is ");
	}
	animationSet = animation_map_param;
	currentFrameType = idle;
	backupFrameType = idle;
}

SDL_Rect BoxEntity::getRenderSize() {
	return visibleBound_Entity;
}

CUS_Point BoxEntity::getPosition() {
	return position_Entity;
}

SDL_Texture* BoxEntity::getCurrentFrame() {
	if (animationSet == NULL) {
		return NULL;
		err::logMessage("Animation set to null");
	}
	else {
		return animationSet->getFrame(currentFrameType, currentNumberFrame);
	}
}

float BoxEntity::getAngle() {
	return angle;
}

int BoxEntity::getTotalFrames() {
	return animationSet->getAnimation(currentFrameType)->getTotalFrames();
}

void BoxEntity::setFlag(bool flag) {
	this->flag = flag;
}

bool BoxEntity::getFlag() {
	return flag;
}

bool BoxEntity::getAnimationPlayed() {
	return animationPlayed;
}

void BoxEntity::setAlpha(float alpha) {
	this->alpha = alpha;
}

void BoxEntity::setAlphaGrowth(float alphaGrowth) {
	this->alphaGrowth = alphaGrowth;
}

void BoxEntity::addAlpha(float alpha) {
	this->alpha += alpha;
}

float BoxEntity::getAlpha() {
	if (alpha < 0) {
		alpha = 0;
		return 0;
	}
	else if (alpha > 255) {
		alpha = 255;
		return 255;
	}
	else {
		return alpha;
	}
}

RenderInformation  BoxEntity::renderCopy() {
	RenderInformation returnValue;
	returnValue.currentFrame = getCurrentFrame();
	returnValue.renderSize = getRenderSize();
	returnValue.angle = getAngle();
	returnValue.alpha = getAlpha();
	return returnValue;
}