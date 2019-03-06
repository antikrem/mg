#include "box_entity.h"
#include "constants.h"
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

void BoxEntity::updateBox(bool supressError) {
	alpha += alphaGrowth;

	if (animationSet != NULL) {
		if (!internalScale) {
			visiblePosition_Entity.x = (int)position.x;
			visiblePosition_Entity.y = (int)position.y;
			visibleBound_Entity.w = animationSet->getAnimation(currentFrameType)->getWidth();
			visibleBound_Entity.h = animationSet->getAnimation(currentFrameType)->getHeight();
			visibleBound_Entity.x = visiblePosition_Entity.x - visibleBound_Entity.w / 2;
			visibleBound_Entity.y = visiblePosition_Entity.y - visibleBound_Entity.h / 2;
		}
		else {
			visiblePosition_Entity.x = (int)position.x;
			visiblePosition_Entity.y = (int)position.y;
			visibleBound_Entity.w = (int)(animationSet->getAnimation(currentFrameType)->getWidth() * internalScale);
			visibleBound_Entity.h = (int)(animationSet->getAnimation(currentFrameType)->getHeight() * internalScale);
			visibleBound_Entity.x = visiblePosition_Entity.x - visibleBound_Entity.w / 2;
			visibleBound_Entity.y = visiblePosition_Entity.y - visibleBound_Entity.h / 2;
		}
	}
	else if (!supressError) {
		err::logMessage("line 39, b_e.cpp TODO");
	}
}

void BoxEntity::itCurrentFrame() {
	skippingFrame++;
	if (animationSet == NULL)
		return;
	if (skippingFrame >= animationSet->getAnimation(currentFrameType)->getSkip()) {
		skippingFrame = 0;
		currentNumberFrame++;
		if (currentNumberFrame >= animationSet->getAnimation(currentFrameType)->getTotalFrames()) {
			animationPlayed = true;
			currentFrameType = backupFrameType;
			skippingFrame = 0;
			currentNumberFrame = 0;
			itCurrentFrame();
			updateBox();
		}
	}
}

void BoxEntity::playAnimation(AnimationType newAnimation, bool suppress) {
	if (animationSet->getAnimation(newAnimation) == NULL) {
		if not(suppress) {
			currentFrameType = idle;
			err::logMessage("playAnimation failed: no animation exists"); //TODO
			err::logMessage(returnLookUp(newAnimation));
		}
	}
	else if (newAnimation != currentFrameType) {
		currentFrameType = newAnimation;
		skippingFrame = 0;
		currentNumberFrame = 0;
		itCurrentFrame();
		updateBox();
	}
}


void BoxEntity::switchAnimation(AnimationType newAnimation, bool suppress) {
	if (animationSet->getAnimation(newAnimation) == NULL) {
		if not(suppress) {
			currentFrameType = idle;
			err::logMessage("switchAnimation failed: no animation exists");
			err::logMessage(returnLookUp(newAnimation));
		}
		
	}
	else if (newAnimation != currentFrameType) {
		currentFrameType = newAnimation;
		backupFrameType = newAnimation;
		skippingFrame = 0;
		currentNumberFrame = 0;
		itCurrentFrame();
		updateBox();
	}
}

void BoxEntity::setAnimationSet(AnimationAssignment* animation_map_param) {
	if (animation_map_param == NULL) {
		err::logMessage("ERROR: Animation set to global default"); //TODO: set to global default
	}
	animationSet = animation_map_param;
	currentFrameType = idle;
	backupFrameType = idle;
}

CUS_Point BoxEntity::getPosition() {
	return position;
}

SDL_Rect BoxEntity::getRenderSize() {
	return visibleBound_Entity;
}



SDL_Texture* BoxEntity::getCurrentFrame() {
	if (animationSet == NULL) {
		err::logMessage("Animation set not specified");
		return NULL;
	}
	else {
		return animationSet->getFrame(currentFrameType, currentNumberFrame);
	}
}

SDL_Texture* BoxEntity::getCurrentShadowFrame() {
	if (animationSet == NULL) {
		err::logMessage("Animation set not specified");
		return NULL;
	}
	else {
		return animationSet->getShadowFrame(currentFrameType, currentNumberFrame);
	}
}

SDL_Texture* BoxEntity::getCurrentLightFrame() {
	if (animationSet == NULL) {
		err::logMessage("Animation set not specified");
		return NULL;
	}
	else {
		return animationSet->getLightFrame(currentFrameType, currentNumberFrame);
	}
}

float BoxEntity::getAngle() {
	return angle;
}

int BoxEntity::getTotalFrames() {
	return animationSet->getAnimation(currentFrameType)->getTotalFrames();
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

#define VIEWRECT { 0,0,RENDERED_X,WORK_SPACE_Y }

RenderInformation  BoxEntity::renderCopy(int shift) {
	RenderInformation returnValue;
	returnValue.currentFrame = getCurrentFrame();
	returnValue.shadowFrame = getCurrentShadowFrame();
	returnValue.lightFrame = getCurrentLightFrame();
	returnValue.renderSize = getRenderSize();
	returnValue.angle = getAngle();
	returnValue.alpha = getAlpha();

	int offset = (int)sqrt(returnValue.renderSize.x + returnValue.renderSize.y);
	SDL_Rect view = VIEWRECT;
	view.x -= offset;
	view.y -= offset;
	view.h += 2*offset;
	view.w += 2*offset;
	view.x -= shift;
	
	CUS_Point pos = { (float)returnValue.renderSize.x + (float)returnValue.renderSize.w/2 , (float)returnValue.renderSize.y + (float)returnValue.renderSize.h / 2 };
	returnValue.inView = (checkPointInRect(pos, view));

	return returnValue;
}