#include <map>
#include "sdl.h"
#include "error.h"
#include "dict.h"
#include "constants.h"
#include "background.h"
#include "sdl_image.h"

#include "parallax.h"
#include "str_kit.h"

#include <iostream>

using namespace std;

void BackgroundManager::addBackground(int cycles, SDL_Texture* newText) {
	if (cycles < 0) {
		err::logMessage("A background was added on a negative cycle");
	}
	else {
		if (newText == NULL) {
			err::logMessage("Background added was NULL");
		}
		backgroundSchedule[cycles] = newText;
	}
}

void BackgroundManager::addTransitionalBackground(int cycles, SDL_Texture* newText) {
	if (cycles < 0) {
		err::logMessage("A background was added on a negative cycle");
	}
	else {
		if (newText == NULL) {
			err::logMessage("Background added was NULL");
		}
		backgroundSchedule[cycles] = newText;
	}
}

BackgroundManager::BackgroundManager(SDL_Renderer* RENDERER, LevelSettings* levelSet) {
	this->renderer = RENDERER;

	string basePath = "campaigns\\";
	basePath.append(levelSet->campaign);
	basePath.append("\\");
	basePath.append(to_string(levelSet->level));
	basePath.append("\\");
	string masterPath = basePath;
	masterPath.append("level_master.txt");
	basePath.append("\\backgrounds\\");

	ifstream inFile;
	inFile.open(masterPath.c_str());

	string line;
	string backgroundImageLocation;
	int lineNo = 0;

	while (getline(inFile, line)) {
		lineNo++;
		auto lineVec = str_kit::splitOnToken(line, ' ');

		if ((line[0] == '/' && line[1] == '/')) {
			pass;
		}
		else if (line == "") {
			pass;
		}
		else if (lineVec.size() == 0) {
			pass;
		}
		else if (lineVec[0] == "BACKGROUND") {
			backgroundImageLocation = basePath;
			backgroundImageLocation.append(lineVec[2]);
			SDL_Surface *temporarySurfaceStorage = IMG_Load(backgroundImageLocation.c_str());
			SDL_Texture *finalTexture = SDL_CreateTextureFromSurface(renderer, temporarySurfaceStorage);
			SDL_FreeSurface(temporarySurfaceStorage);
			backgroundSchedule[stoi(lineVec[1])] = finalTexture;
			depthTable[stoi(lineVec[1])] = stof(lineVec[3]);

			if (lineVec.size() == 5) {
				backgroundImageLocation = basePath;
				backgroundImageLocation.append(lineVec[4]);
				SDL_Surface *temporarySurfaceStorage = IMG_Load(backgroundImageLocation.c_str());
				SDL_Texture *finalTexture = SDL_CreateTextureFromSurface(renderer, temporarySurfaceStorage);
				SDL_FreeSurface(temporarySurfaceStorage);
				backgroundTransitionSchedule[stoi(lineVec[1])] = finalTexture;
			}

		}
	}
	inFile.close();

	if (backgroundSchedule.count(0))
		currentBackground = backgroundSchedule[0];
	else {
		err::logMessage("No background schedule for cycle 0");
		currentBackground = NULL;
	}
		

	if (backgroundTransitionSchedule.count(0))
		transitionBackground = backgroundTransitionSchedule[0];
	currentDepth = depthTable[0];

	int totalScroll = 1080;
	BackRender newbackground;
	while (totalScroll > 0) {
		int w, h;
		if (transitionBackground) {
			SDL_QueryTexture(transitionBackground, NULL, NULL, &w, &h);
			newbackground.texture = transitionBackground;
			transitionBackground = NULL;
		}
		else {
			SDL_QueryTexture(currentBackground, NULL, NULL, &w, &h);
			newbackground.texture = currentBackground;
		}

		totalScroll -= h;
		
		newbackground.dest.w = w;
		newbackground.dest.h = h;
		newbackground.dest.x = 0;
		newbackground.dest_y = F(totalScroll);
		newbackground.dest.y = (int)newbackground.dest_y;
		
		backgroundList.push_back(newbackground);
	}
	
}

BackgroundManager::~BackgroundManager() {
	for (auto element : backgroundSchedule) {
		SDL_DestroyTexture(element.second);
		element.second = NULL;
	}

	for (auto element : backgroundTransitionSchedule) {
		SDL_DestroyTexture(element.second);
		element.second = NULL;
	}
}

//TODO: 1080 background updates per fucking second
void BackgroundManager::forceBackgroundTable() {
	for (int i = 0; i < 1080; i++) {
		;
	}
}

void BackgroundManager::updateBackground(float windspeed) {
	internalCounter++;
	if (currentBackground == NULL) {
		currentBackground = backgroundSchedule[0];
		if (currentBackground == NULL) {
			err::logMessage("No background at backgroundSchedule[0], be sure to declare");
		}
	}
	
	//update future tiles if time to switch
	if (backgroundSchedule.count(internalCounter)) {
		currentBackground = backgroundSchedule[internalCounter];
	}
	if (backgroundTransitionSchedule.count(internalCounter)) {
		transitionBackground = backgroundTransitionSchedule[internalCounter];
	}

	//update list
	for (unsigned int i = 0; i < backgroundList.size(); i++) {
		backgroundList[i].dest_y += windspeed;
		backgroundList[i].dest.y = (int)backgroundList[i].dest_y;
	}

	//remove background tiles that pass out
	while (backgroundList[0].dest_y > 1080) {
		backgroundList.erase(backgroundList.begin());
	}

	//add new tiles
	while (backgroundList[backgroundList.size()-1].dest_y > 0) {
		BackRender newbackground;

		newbackground.texture = currentBackground;
		if (transitionBackground) {
			newbackground.texture = transitionBackground;
			transitionBackground = NULL;
		}
		int w, h;
		SDL_QueryTexture(newbackground.texture, NULL, NULL, &w, &h);
		newbackground.dest_y = (backgroundList[backgroundList.size() - 1].dest_y - F(h));
		newbackground.dest.y = (int)newbackground.dest_y;
		newbackground.dest.w = w;
		newbackground.dest.h = h;
		newbackground.dest.x = 0;
		backgroundList.push_back(newbackground);
	}

}

void BackgroundManager::drawBackground(int shift) {
	float newShift = para::getSizeScaler(currentDepth)* F(shift);

	SDL_Rect temp;
	for (auto backRender : backgroundList) {
		temp.h = backRender.dest.h;
		temp.w = backRender.dest.w;
		temp.x = backRender.dest.x;
		temp.y = backRender.dest.y;
		temp.x += (int)newShift;

		SDL_RenderCopy(renderer, backRender.texture, NULL, &temp);
	}
}