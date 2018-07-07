#include "animation.h"

void Animation::resetRect() {
	srcrect.y = 0;
	srcrect.x = 0;
	srcrect.w = 0;
	srcrect.h = 0;
}

void Animation::addTextureToMap(SDL_Renderer* RENDERER, SDL_Surface* src, int widthOfFrame, int heightOfFrame, int numberOfFrame, float scale) {
	SDL_Surface *newSurfaceGenerated =
		SDL_CreateRGBSurface(0, widthOfFrame, heightOfFrame, 32, 0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000);
	//err::logNULL(newSurfaceGenerated, "On loading texture, temp surface failed to be created.");

	srcrect.y = 0;
	srcrect.x = numberOfFrame * widthOfFrame;
	srcrect.w = widthOfFrame;
	srcrect.h = heightOfFrame;

	//	Implement error check for final version
	SDL_BlitSurface(src, &srcrect, newSurfaceGenerated, NULL);
	SDL_Texture* finalTex = SDL_CreateTextureFromSurface(RENDERER, newSurfaceGenerated);
	texture_map[numberOfFrame] = finalTex;
	SDL_FreeSurface(newSurfaceGenerated);
	resetRect();
}

Animation::Animation(SDL_Renderer* RENDERER, const char* bmp_location, int widthOfFrame, int frameSkip_param, float scale) {

	resetRect();
	SDL_Surface *temporarySurfaceStorage = SDL_LoadBMP(bmp_location);

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
	for (it = texture_map.begin(); it != texture_map.end(); it++) {
		SDL_DestroyTexture(it->second);
	}
}

SDL_Texture* Animation::getFrame(int numberOfFrame) {
	return texture_map[numberOfFrame];
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