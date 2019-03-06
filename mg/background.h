/*simple background interface,*/
#ifndef __BACKGROUND_H__
#define __BACKGROUND_H__

#include "SDL.h"
#include "thread_safe.h"
#include "cus_structs.h"
#include <map>
#include <vector>

/*A storage structure containing a texture and render information that represents one tile of background.*/
struct BackRender {
	SDL_Texture* texture;
	float dest_y;
	SDL_Rect dest;
};

/*Stores and manages BackRenders being drawn.
Loads background tiles based on clock*/
class BackgroundManager : public ThreadSafe {
private:
	int internalCounter = 0;
	SDL_Renderer* renderer = NULL;

	//Future background Schedules
	map<int, SDL_Texture*> backgroundSchedule;
	//Only one can be loaded at a time, and it gets pushed on at first opertunity
	map<int, SDL_Texture*> backgroundTransitionSchedule;

	SDL_Texture* transitionBackground = NULL;
	SDL_Texture* currentBackground = NULL;

	std::vector<BackRender> backgroundList;

	map<int, float> depthTable;
	float currentDepth = (float)3;

	void addBackground(int cycles, SDL_Texture* newText);

	void addTransitionalBackground(int cycles, SDL_Texture* newText);

public:
	BackgroundManager(SDL_Renderer* RENDERER, LevelSettings* levelSet);

	~BackgroundManager();

	/*Cycles through, forcing the new background to completly overwrite the new background*/
	void forceBackgroundTable();

	void updateBackground(float windspeed);

	void drawBackground(int shift);
};

#endif