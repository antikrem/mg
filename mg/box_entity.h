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
	float alpha = 255;
	SDL_Texture* currentFrame;
	SDL_Rect renderSize;
	float angle;
};

/*Framework for every rendered entity
Contains methods for applying and accessing texture maps
All entities must updateBox() after every change in position
*/

static bool checkPointInRect(CUS_Point point, SDL_Rect rectangle);

class BoxEntity {
protected:
	bool flag = true;
	CUS_Point position_Entity;

	SDL_Point visiblePosition_Entity;
	SDL_Rect visibleBound_Entity;
	float angle = 0;
	float alpha = (float)255;
	float alphaGrowth = 0;

	float internalScale = 0;
	bool animationPlayed = false;
	int currentNumberFrame = 0;
	int skippingFrame = 0;

	AnimationAssignment* animationSet;
	AnimationType currentFrameType = idle;
	AnimationType backupFrameType = idle;

	void updateBox();

	void itCurrentFrame();

	//plays the new animation, returning to backupFrameType after the animation
	void playAnimation(AnimationType newAnimation);

	//plays the new animation until a new animation is set
	void switchAnimation(AnimationType newAnimation);

public:
	void setAnimationSet(AnimationAssignment* animation_map_param);

	SDL_Rect getRenderSize();

	CUS_Point getPosition();

	SDL_Texture* getCurrentFrame();

	float getAngle();

	int getTotalFrames();

	void setFlag(bool flag);

	bool getFlag();

	bool getAnimationPlayed();

	void setAlpha(float alpha);

	void setAlphaGrowth(float alphaGrowth);

	void addAlpha(float alpha);

	float getAlpha();

	RenderInformation renderCopy();
};

#endif