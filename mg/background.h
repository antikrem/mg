#ifndef __BACKGROUND_H__
#define __BACKGROUND_H__

#include "SDL.h"
#include "thread_safe.h"
#include <map>

//total backgrounds able to be rendered at a time
const int MAX_BACKGROUND_TABLE = 6;

/*A storage structure containing a texture and render information that represents one tile of background.
Has a flag that is flipped true when the surface ahs been rendered this draw cycle*/
struct BackRender {
	SDL_Texture* texture;
	float dest_y;
	SDL_Rect dest;
	bool renderedFlag;
};

/*Stores and manages BackRenders being drawn.
Loads background tiles based on clock*/
class BackgroundManager : public ThreadSafe {
private:
	int internalCounter = 0;

	map<int, SDL_Texture*> backgroundSchedule;
	BackRender backgroundTable[MAX_BACKGROUND_TABLE];
	SDL_Texture* currentBackground = NULL;

	float scrollSpeed = (float)1;
	map<int, float> speedTable;
	map<int, float> shiftTable;
	float currentShift = (float)3;

	int getFreeBackgroundSpace();

	void updateBackground();

	void addBackground(int cycles, SDL_Texture* newText);

	void addScrollSpeed(int cycles, float speed);

	void addShiftCoeff(int cycles, float shift);

public:
	BackgroundManager();

	~BackgroundManager();

	void initialiseBackgroundManager(SDL_Renderer* RENDERER, LevelSettings* levelSet);

	void primeBackgroundTable();

	void forceBackgroundTable();

	void readyBackgroundTable();

	bool remainingBackgroundTable();

	float getShift();

	BackRender getABackground();
};

#endif