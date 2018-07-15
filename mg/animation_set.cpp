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