/*Classes for storing animation.*/
#ifndef __ANIMATION_H_INCLUDED__
#define __ANIMATION_H_INCLUDED__
#include <map>
#include <string>

#include "sdl.h"
#include "sdl_image.h"
#include "error.h"

using namespace std;

typedef map<int, SDL_Texture*> mt_tex;

/*Stores an animation:
Animations stored as int to SDL_Texture* map
Animation frames stored as textures in VRAM
Loads from Bitmap sprite sheet
One initialisation: Constructor loads bitmap, no seperate function required
Use an instance of animationAsignmentor DecayingAnimation to interface properly
*/
class Animation {
protected:
	int height;
	int width;
	/*Amount of frames in animation*/
	int	frames;
	/*Amount of frames the frame is shown for*/
	int	frameSkip;

	/*Bread and butter of the Animation class,
	Associates the position of frame in animation (int) to a pointer to the animation*/
	mt_tex textureMap;
	mt_tex textureShadowMap;
	mt_tex textureLightMap;

	SDL_Rect srcrect;

	void resetRect();
	void addTextureToMap(SDL_Renderer*, SDL_Surface*, int, int, int, float);

public:
	Animation(SDL_Renderer* RENDERER, const char* bmp_location, int widthOfFrame, int frameSkip_param, float scale = (float) 1.0);

	Animation(SDL_Renderer* RENDERER, SDL_Surface* surface, int widthOfFrame, int frameSkip_param, float scale = (float) 1.0);

	~Animation();

	SDL_Texture* getFrame(int);

	SDL_Texture* getShadowFrame(int);

	SDL_Texture* getLightFrame(int);

	int getTotalFrames();

	/*get cycles that each frame is displayed for.*/
	int getSkip();

	int getWidth();

	int getHeight();

};

#endif