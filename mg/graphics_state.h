#ifndef __GRAPHICS_STATE_H_INCLUDED__
#define __GRAPHICS_STATE_H_INCLUDED__

#include "SDL.h"
#include "dict.h"
#include "animation_set.h"
#include "cus_structs.h"

const SDL_Point FULL_SPACE = { 1920 , 1080 };

enum GraphicsSettings {
	low,
	medium,
	high,
	ultra
};

class GraphicsState {
private:
	SDL_Window* gWindow;
	SDL_Renderer* gRenderer;

	SDL_Point resolution;
	CUS_Point scale;
	bool fullscreen = 1;
	bool vsync = 0;
	bool hardwareAccelerated = 1;
	bool showFPS = 0;
	int frameLimit = 0;

	GraphicsSettings weatherVolume = high;

	//Setting related to the particle system
	//scales distance for particle effect distance, smaller values will result in further particles 
	//being ignored in calculations
	float particleDistanceAffectScale = 1;

	GraphicsSettings intToGraph(int integer) {
		if (integer < 0)
			return low;
		else if (integer > 3)
			return ultra;
		else
			return (GraphicsSettings) integer;
	}
	
public:
	GraphicsState() {
		Dictionary graphicsSettings("config//gfx_settings.ini");
		if (graphicsSettings.checkKey("window.w"))
			resolution.x = graphicsSettings.byIntKey("window.w");
		if (graphicsSettings.checkKey("window.h"))
			resolution.y = graphicsSettings.byIntKey("window.h");
		if (graphicsSettings.checkKey("fullscreen"))
			fullscreen = graphicsSettings.byIntKey("fullscreen");
		if (graphicsSettings.checkKey("framelimit"))
			frameLimit = graphicsSettings.byIntKey("framelimit");
		if (graphicsSettings.checkKey("vsync"))
			vsync = graphicsSettings.byIntKey("vsync");
		if (graphicsSettings.checkKey("hardware_accelerated"))
			hardwareAccelerated = graphicsSettings.byIntKey("hardware_accelerated");

		if (graphicsSettings.checkKey("weather_volume"))
			weatherVolume = intToGraph(graphicsSettings.byIntKey("weather_volume"));

		if (graphicsSettings.checkKey("showFPS"))
			showFPS = graphicsSettings.byIntKey("showFPS");

		err::logMessage("Resolution: " + to_string(resolution.x) + " by " + to_string(resolution.y) );
		err::logMessage("Fullscreen: " + to_string(fullscreen));
		err::logMessage("vsync: " + to_string(vsync));
		
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

	int getFrameLimit() {
		return frameLimit;
	}

	void renderFrame() {
		SDL_RenderPresent(gRenderer);
	}

	GraphicsSettings getWeatherVolume() {
		return weatherVolume;
	}

	float getParticleDistanceAffectScale() {
		return particleDistanceAffectScale;
	}
};

//Cleanly pull a texture from img, returns null on error
static SDL_Texture* pullTextureFromPath(string path, GraphicsState* gState) {
	SDL_Surface* surf = IMG_Load(path.c_str());
	if (!surf) {
		err::logMessage("A file is corrupted or missing, check " +path +", or reinstall");
		return NULL;
	}
	auto text = SDL_CreateTextureFromSurface(gState->getGRenderer(), surf);
	SDL_FreeSurface(surf);
	return text;
}

#endif