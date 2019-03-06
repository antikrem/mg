#ifndef __BOX_ENTITY_H_INCLUDED__
#define __BOX_ENTITY_H_INCLUDED__

#include <map>

#include "sdl.h"
#include "error.h"
#include "cus_structs.h"
#include "animation_set.h"

/*Structure used by engine for rendering a BoxEntity
*/
struct RenderInformation {
	//If false don't render
	bool inView = true;
	float alpha = 255;
	SDL_Texture* currentFrame;
	SDL_Texture* shadowFrame;
	SDL_Texture* lightFrame;
	SDL_Rect renderSize;
	float angle;
};

/*Framework for every rendered entity
Contains methods for applying and accessing texture maps
All entities must updateBox() after every change in position
*/
class BoxEntity {
protected:
	CUS_Point position;

	SDL_Point visiblePosition_Entity;
	SDL_Rect visibleBound_Entity;
	float angle = 0;
	float alpha = (float)255;
	float alphaGrowth = 0;

	float internalScale = 0;
	bool animationPlayed = false;
	int currentNumberFrame = 0;
	int skippingFrame = 0;

	AnimationAssignment* animationSet = NULL;
	/*Currently played animation*/
	AnimationType currentFrameType = idle;
	/*Animation to play on fail, or if current animation ends*/
	AnimationType backupFrameType = idle;

	/*updates box, where the box gets rendered*/
	void updateBox(bool supressError = true);

	/*Go to next frame*/
	void itCurrentFrame();

	//plays the new animation, returning to backupFrameType after the animation
	//when fed supress = true, no error will occur when playAnimation fails, and no effect will occur
	void playAnimation(AnimationType newAnimation, bool suppress = false);

	//plays the new animation until a new animation is set
	void switchAnimation(AnimationType newAnimation, bool suppress = false);

public:
	void setAnimationSet(AnimationAssignment* animation_map_param);

	CUS_Point getPosition();

	SDL_Rect getRenderSize();

	SDL_Texture* getCurrentShadowFrame();

	SDL_Texture* getCurrentLightFrame();

	SDL_Texture* getCurrentFrame();

	float getAngle();

	int getTotalFrames();

	bool getAnimationPlayed();

	void setAlpha(float alpha);

	void setAlphaGrowth(float alphaGrowth);

	void addAlpha(float alpha);

	float getAlpha();

	RenderInformation renderCopy(int shift);
};

#endif