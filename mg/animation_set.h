/*Interface classess for accessing and organising animation*/
#ifndef __ANIMATION_SET_H_INCLUDED__
#define __ANIMATION_SET_H_INCLUDED__

#include "animation.h"
#include "level_enum.h"
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
	mt_ani animationMap;

	//Locally created are loaded at start of level and cleared at end
	bool local;
	bool campaignLocal;

public:
	AnimationAssignment(bool local = false, bool campaignLocal = false);

	~AnimationAssignment();

	void addAnimation(AnimationType, Animation*);

	Animation* getAnimation(AnimationType);

	SDL_Texture* getFrame(AnimationType, int);

	SDL_Texture* getShadowFrame(AnimationType, int);

	SDL_Texture* getLightFrame(AnimationType, int);

	void setLocal(bool local);

	bool getLocal();

};

//Global Animation calls

void addAssToStore(char* assignmentName, AnimationAssignment* newAss);

AnimationAssignment* getFromStore(string assignmentName);

bool checkInStore(string assignmentName);

void loadStore(SDL_Renderer* gRenderer);

void loadStoreLocal(SDL_Renderer* gRenderer, LevelSettings* levelSettings, bool campaignLocal);

void clearAnimaionStore();

void clearLocalAnimaionStore();
#endif