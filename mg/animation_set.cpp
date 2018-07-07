#include "animation_set.h"

#include <iostream>

/*Stores a set of animations for default entity
Use to avoid redundant texture storage and reduce memory footprint
*/
AnimationAssignment::~AnimationAssignment() {
	mt_ani::iterator it;
	for (it = animation_map.begin(); it != animation_map.end(); it++) {
		if (it->second != NULL) {
			delete it->second;
			it->second = NULL;
		}
	}
}

void AnimationAssignment::addAnimation(AnimationType animationType_param, Animation* animation) {
	animation_map[animationType_param] = animation;
}

Animation* AnimationAssignment::getAnimation(AnimationType animationType_param) {
	if (this == NULL) {
		err::logMessage("Getting animation from an assignment that does not exist");
	}

	if (animation_map[animationType_param] == NULL) {
		err::logMessage("Animation not initialised for this set");
	}
	return animation_map[animationType_param];
}

SDL_Texture* AnimationAssignment::getFrame(AnimationType animationType_param, int frameNumber) {
	return (animation_map[animationType_param])->getFrame(frameNumber);
}

AnimationStore::AnimationStore(SDL_Renderer* in) {
	RENDERER = in;
}

void AnimationStore::addAniToStore(const char* location, string assignmentName, AnimationType newType, int width, int frameSkip) {
	Animation* temp = new Animation(RENDERER, location, width, frameSkip);

	if (localStore[assignmentName] == NULL) {
		AnimationAssignment* aniAss = new AnimationAssignment();
		aniAss->addAnimation(newType, temp);
		localStore[assignmentName] = aniAss;
		err::logMessage("New aniAss made:");
		err::logMessage(assignmentName);
	}
	else {
		localStore[assignmentName]->addAnimation(newType, temp);
	}
}

void AnimationStore::addAssToStore(char* assignmentName, AnimationAssignment* newAss) {
	if (localStore[assignmentName] == NULL) {
		localStore[assignmentName] = newAss;
	}
	else {
		err::logMessage(assignmentName);
	}
}

AnimationAssignment* AnimationStore::getFromStore(string assignmentName) {

	if (localStore[assignmentName] != NULL) {
		return localStore[assignmentName];
	}
	else {
		err::logMessage("An animation assignment was requested from a store when it doesn't exist");
		return NULL;
	}

}