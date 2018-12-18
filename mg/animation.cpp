#include "animation.h"

#include <iostream>

void Animation::resetRect() {
	srcrect.y = 0;
	srcrect.x = 0;
	srcrect.w = 0;
	srcrect.h = 0;
}

void Animation::addTextureToMap(SDL_Renderer* RENDERER, SDL_Surface* src, int widthOfFrame, int heightOfFrame, int numberOfFrame, float scale) {
	SDL_Surface *newSurfaceGenerated =
		SDL_CreateRGBSurface(0, widthOfFrame, heightOfFrame, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);

	srcrect.y = 0;
	srcrect.x = numberOfFrame * widthOfFrame;
	srcrect.w = widthOfFrame;
	srcrect.h = heightOfFrame;

	//TODO: Implement error check for final version
	SDL_BlitSurface(src, &srcrect, newSurfaceGenerated, NULL);
	SDL_Texture* finalTex = SDL_CreateTextureFromSurface(RENDERER, newSurfaceGenerated);
	textureMap[numberOfFrame] = finalTex;

	SDL_PixelFormat *fmt = newSurfaceGenerated->format;
	if (fmt->BitsPerPixel != 32)
		err::logMessage("Format is borked");

	Uint32 *pixel = (Uint32 *) newSurfaceGenerated->pixels;
	Uint8 r, g, b, a;

	//Generates dark map
	SDL_LockSurface(newSurfaceGenerated);
	for (int y = 0; y<newSurfaceGenerated->h; y++) {
		for (int x = 0; x < newSurfaceGenerated->w; x++) {
			pixel = (Uint32 *) newSurfaceGenerated->pixels;
			pixel += ((y*newSurfaceGenerated->w) + x);
			SDL_GetRGBA(*pixel, fmt, &r, &g, &b, &a);
			*pixel = SDL_MapRGBA(fmt, 0, 0, 0, a);

		}
	}
	SDL_UnlockSurface(newSurfaceGenerated);

	finalTex = SDL_CreateTextureFromSurface(RENDERER, newSurfaceGenerated);
	textureShadowMap[numberOfFrame] = finalTex;

	//Generates light map
	SDL_LockSurface(newSurfaceGenerated);
	for (int y = 0; y<newSurfaceGenerated->h; y++) {
		for (int x = 0; x < newSurfaceGenerated->w; x++) {
			pixel = (Uint32 *)newSurfaceGenerated->pixels;
			pixel += ((y*newSurfaceGenerated->w) + x);
			SDL_GetRGBA(*pixel, fmt, &r, &g, &b, &a);
			*pixel = SDL_MapRGBA(fmt, 255, 255, 255, a);

		}
	}
	SDL_UnlockSurface(newSurfaceGenerated);

	finalTex = SDL_CreateTextureFromSurface(RENDERER, newSurfaceGenerated);
	textureLightMap[numberOfFrame] = finalTex;

	SDL_FreeSurface(newSurfaceGenerated);
	resetRect();
}

Animation::Animation(SDL_Renderer* RENDERER, const char* bmp_location, int widthOfFrame, int frameSkip_param, float scale) {
	resetRect();
	SDL_Surface *temporarySurfaceStorage = IMG_Load(bmp_location);

	if (temporarySurfaceStorage == NULL) {
		err::logMessage("ERROR: A file is missing, reinstall or verify game files. Missing file is:");
		err::logMessage(bmp_location);
	}
	if (widthOfFrame > temporarySurfaceStorage->w) {
		err::logMessage("ERROR: Asset metadata is corrupted, reinstall or verify game files");
	}

	height = temporarySurfaceStorage->h;
	frames = (int)(temporarySurfaceStorage->w / widthOfFrame);
	frameSkip = frameSkip_param;
	width = widthOfFrame;

	for (int i = 0; i < frames; i++) {
		addTextureToMap(RENDERER, temporarySurfaceStorage, widthOfFrame, height, i, scale);
	}

	SDL_FreeSurface(temporarySurfaceStorage);
}

Animation::Animation(SDL_Renderer* RENDERER, SDL_Surface* surface, int widthOfFrame, int frameSkip_param, float scale) {

	resetRect();

	height = surface->h;
	frames = (int)(surface->w / widthOfFrame);
	frameSkip = frameSkip_param;
	width = widthOfFrame;

	for (int i = 0; i < frames; i++) {
		addTextureToMap(RENDERER, surface, widthOfFrame, height, i, scale);
	}

	SDL_FreeSurface(surface);
}

Animation::~Animation() {
	mt_tex::iterator it;
	for (it = textureMap.begin(); it != textureMap.end(); it++) {
		SDL_DestroyTexture(it->second);
	}

	for (it = textureShadowMap.begin(); it != textureShadowMap.end(); it++) {
		SDL_DestroyTexture(it->second);
	}
}

SDL_Texture* Animation::getFrame(int numberOfFrame) {
	return textureMap[numberOfFrame];
}

SDL_Texture* Animation::getShadowFrame(int numberOfFrame) {
	return textureShadowMap[numberOfFrame];
}

SDL_Texture* Animation::getLightFrame(int numberOfFrame) {
	return textureLightMap[numberOfFrame];
}

int Animation::getTotalFrames() {
	return frames;
}

int Animation::getSkip() {
	return frameSkip;
}

int Animation::getWidth() {
	return width;
}

int Animation::getHeight() {
	return height;
}