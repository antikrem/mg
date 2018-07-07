#ifndef __GRAPHICS_STATE_H_INCLUDED__
#define __GRAPHICS_STATE_H_INCLUDED__

#include "SDL.h"
#include "dict.h"

class GraphicsState {
private:
	SDL_Window* gWindow;
	SDL_Renderer* gRenderer;

	SDL_Point resolution;
	bool fullscreen;
	bool vsync;
	bool hardwareAccelerated;

	
public:
	GraphicsState() {
		Dictionary graphicsSettings("config//gfx_settings.ini");
		resolution.x = graphicsSettings.byIntKey("window.w");
		resolution.y = graphicsSettings.byIntKey("window.h");
		fullscreen = graphicsSettings.byIntKey("fullscreen");
		vsync = graphicsSettings.byIntKey("fullscreen");
		hardwareAccelerated = graphicsSettings.byIntKey("hardware_accelerated");

		err::logMessage(to_string(resolution.x));
		err::logMessage(to_string(resolution.y));
		err::logMessage(to_string(fullscreen));
		err::logMessage(to_string(vsync));
		
		gWindow = SDL_CreateWindow("2tick(+1)-in_dev",
			SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
			resolution.x, resolution.y, fullscreen);
			
		gRenderer = SDL_CreateRenderer( gWindow, -1, (SDL_RENDERER_ACCELERATED * hardwareAccelerated) | (SDL_RENDERER_PRESENTVSYNC * vsync) );
	}

	SDL_Point getResolution() {
		return resolution;
	}

	bool getFullScreen() {
		return fullscreen;
	}
};

#endif