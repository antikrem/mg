#ifndef __GRAPHICS_STATE_H_INCLUDED__
#define __GRAPHICS_STATE_H_INCLUDED__

#include "SDL.h"
#include "dict.h"
#include "animation_set.h"
#include "cus_structs.h"

const SDL_Point FULL_SPACE = { 1920 , 1080 };

class GraphicsState {
private:
	SDL_Window* gWindow;
	SDL_Renderer* gRenderer;

	SDL_Point resolution;
	CUS_Point scale;
	bool fullscreen;
	bool vsync;
	bool hardwareAccelerated;
	bool showFPS;

	//store variables
	map<string, AnimationAssignment*> localStore;
	
public:
	GraphicsState() {
		Dictionary graphicsSettings("config//gfx_settings.ini");
		resolution.x = graphicsSettings.byIntKey("window.w");
		resolution.y = graphicsSettings.byIntKey("window.h");
		fullscreen = graphicsSettings.byIntKey("fullscreen");
		vsync = graphicsSettings.byIntKey("fullscreen");
		hardwareAccelerated = graphicsSettings.byIntKey("hardware_accelerated");

		err::logMessage("Resolution: " + to_string(resolution.x) + " by " + to_string(resolution.y) );
		err::logMessage(to_string(fullscreen));
		err::logMessage(to_string(vsync));

		Dictionary engineSettings("config//engine_settings.ini");
		showFPS = engineSettings.byIntKey("showFPS");
		
		gWindow = SDL_CreateWindow("2tick(+1)-in_dev",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			resolution.x, resolution.y, fullscreen);
			
		gRenderer = SDL_CreateRenderer( gWindow, -1, (SDL_RENDERER_ACCELERATED * hardwareAccelerated) | (SDL_RENDERER_PRESENTVSYNC * vsync) );

		scale = { (float)resolution.x / FULL_SPACE.x, (float) resolution.y / FULL_SPACE.y };
		SDL_RenderSetScale(gRenderer, scale.x, scale.y);
	}

	SDL_Renderer* getGRenderer() {
		return gRenderer;
	}

	bool getShowFPS() {
		return showFPS;
	}

	void addAniToStore(const char* location, string assignmentName, AnimationType newType, int width, int frameSkip) {
		Animation* temp = new Animation(gRenderer, location, width, frameSkip);

		if (localStore[assignmentName] == NULL) {
			AnimationAssignment* aniAss = new AnimationAssignment();
			aniAss->addAnimation(newType, temp);
			localStore[assignmentName] = aniAss;
			err::logMessage("New aniAss made:" + assignmentName);
		}
		else {
			localStore[assignmentName]->addAnimation(newType, temp);
		}
	}

	void addAssToStore(char* assignmentName, AnimationAssignment* newAss) {
		if (localStore[assignmentName] == NULL) {
			localStore[assignmentName] = newAss;
		}
		else {
			err::logMessage(assignmentName);
		}
	}

	AnimationAssignment* getFromStore(string assignmentName) {

		if (localStore[assignmentName] != NULL) {
			return localStore[assignmentName];
		}
		else {
			err::logMessage("An animation assignment was requested from a store when it doesn't exist");
			return NULL;
		}

	}

	void renderFrame() {
		SDL_RenderPresent(gRenderer);
	}
};

#endif