/*Interface classess for accessing and organising animation*/
#ifndef __ANIMATION_SET_H_INCLUDED__
#define __ANIMATION_SET_H_INCLUDED__

#include "animation.h"
#include <string>

/*Enumeration of different animations avalible as index
*/
enum AnimationType {
	spawn,
	idle,
	move_left,
	move_right,
	death
};

typedef map<AnimationType, Animation*> mt_ani;

/*Stores a set of animations for default entity
Use to avoid redundant texture storage and reduce memory footprint
*/
class AnimationAssignment {
private:
	mt_ani animation_map;

public:
	~AnimationAssignment();

	void addAnimation(AnimationType, Animation*);

	Animation* getAnimation(AnimationType);

	SDL_Texture* getFrame(AnimationType, int);

};
#endif