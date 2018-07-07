/*Interface classess for accessing and organising animation*/
#ifndef __ANIMATION_SET_H_INCLUDED__
#define __ANIMATION_SET_H_INCLUDED__

#include "animation.h"
#include <string>

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

/*Stores every entity animation into an easy to access store for later use*/
class AnimationStore {
private:
	SDL_Renderer * RENDERER;
	map<string, AnimationAssignment*> localStore;

public:
	AnimationStore(SDL_Renderer* in);

	void addAniToStore(const char* location, string assignmentName, AnimationType newType, int width, int frameSkip);

	void addAssToStore(char* assignmentName, AnimationAssignment* newAss);

	AnimationAssignment* getFromStore(string assignmentName);
};

#endif