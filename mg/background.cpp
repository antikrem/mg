#pragma once
#include <map>
#include "sdl.h"
#include "error.h"
#include "dict.h"
#include "global_constants.h"
#include "background.h"

#include <iostream>

using namespace std;


int BackgroundManager::getFreeBackgroundSpace() {
	for (int i = 0; i < MAX_BACKGROUND_TABLE; i++) {
		if (backgroundTable[i].texture == NULL) {
			return i;
		}
	}
	err::logMessage("Make MAX_BACKGROUND_TABLE bigger");
	return 0;
}

void BackgroundManager::updateBackground() {
	if (currentBackground == NULL) {
		currentBackground = backgroundSchedule[0];
		if (currentBackground == NULL) {
			err::logMessage("No background at backgroundSchedule[0], be sure to declare");
		}
	}

	if (backgroundSchedule[internalCounter] != NULL) {
		currentBackground = backgroundSchedule[internalCounter];
	}

	bool addNewBackground = true;

	if (speedTable.count(internalCounter) == 1) {
		scrollSpeed = speedTable[internalCounter];
	}

	for (int i = 0; i < MAX_BACKGROUND_TABLE; i++) {
		if (backgroundTable[i].texture != NULL) {
			backgroundTable[i].dest_y += scrollSpeed;
			backgroundTable[i].dest.y = (int)backgroundTable[i].dest_y;
			if (backgroundTable[i].dest.y < 0) {
				addNewBackground = false;
			}

			//deletes backgrounds out of range
			int w, h;
			SDL_QueryTexture(backgroundTable[i].texture, NULL, NULL, &w, &h);

			if (backgroundTable[i].dest.y - 5 > WORK_SPACE_Y) {
				backgroundTable[i].texture = NULL;
				backgroundTable[i].dest_y = 0;
				backgroundTable[i].dest = { 0, 0, 0, 0, };
			}
		}
	}

	if (addNewBackground == true) {
		int w, h;
		SDL_QueryTexture(currentBackground, NULL, NULL, &w, &h);
		SDL_Rect newDest = { 0, -h, w, h };

		BackRender newElement = { currentBackground, -(float)h , newDest, false };

		backgroundTable[getFreeBackgroundSpace()] = newElement;
	}
}

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

void BackgroundManager::addScrollSpeed(int cycles, float speed) {
	if (cycles < 0) {
		err::logMessage("A scroll was added on a negative cycle");
	}
	else {
		speedTable[cycles] = speed;
	}
}

void BackgroundManager::addShiftCoeff(int cycles, float shift) {
	if (cycles < 0) {
		err::logMessage("A shift was added on a negative cycle");
	}
	else {
		shiftTable[cycles] = shift;
	}
}

BackgroundManager::BackgroundManager() {
	for (int i = 0; i < MAX_BACKGROUND_TABLE; i++) {
		backgroundTable[i].texture = NULL;
		backgroundTable[i].dest = { 0, 0, 0, 0 };
		backgroundTable[i].dest_y = 0;
	}
	speedTable[0] = (float)1;
}

BackgroundManager::~BackgroundManager() {
	for (auto const& element : backgroundSchedule) {
		SDL_DestroyTexture(element.second);
	}
}

void BackgroundManager::initialiseBackgroundManager(SDL_Renderer* RENDERER, LevelSettings levelSet) {
	int level = levelSet.level;

	string filePath = "levels\\";
	filePath.append(to_string(level));
	filePath.append("\\backgrounds\\update_table.txt");

	ifstream inFile;
	inFile.open(filePath.c_str());

	string line;
	while (getline(inFile, line)) {
		if (line[0] == 's') {
			addScrollSpeed(
				stoi(line.substr(1, line.find(" "))),
				stof(line.substr(line.find(" ") + 1, line.length()))
			);
		}
		else if (line[0] == 'c') {
			addShiftCoeff(
				stoi(line.substr(1, line.find(" "))),
				stof(line.substr(line.find(" ") + 1, line.length()))
			);
		}
		else if (line[0] == '/') {
			;
		}
	}
	inFile.close();

	filePath = "levels\\";
	filePath.append(to_string(level));
	filePath.append("\\backgrounds\\");

	string filePathTemp = filePath;
	filePathTemp.append("background_table.txt");

	inFile.open(filePathTemp);

	int cycle;
	SDL_Surface* newSurf;
	SDL_Texture* newTex;
	string bmpPos;

	err::logMessage("Loading backgrounds:");
	while (getline(inFile, line)) {
		cycle = stoi(line.substr(0, line.find(" ")));

		bmpPos = filePath;
		bmpPos.append(line.substr(line.find(" ") + 1, line.length()));
		bmpPos.append(".bmp");

		err::logMessage(to_string(cycle) + " " + bmpPos);
	
		newSurf = SDL_LoadBMP(bmpPos.c_str());
		newTex = SDL_CreateTextureFromSurface(RENDERER, newSurf);
		SDL_FreeSurface(newSurf);

		addBackground(cycle, newTex);
	}
}

void BackgroundManager::primeBackgroundTable() {
	internalCounter++;
	updateBackground();
}

//TODO: 1080 background updates per fucking second
void BackgroundManager::forceBackgroundTable() {
	for (int i = 0; i < 1080; i++) {
		updateBackground();
	}
}

void BackgroundManager::readyBackgroundTable() {
	for (int i = 0; i < MAX_BACKGROUND_TABLE; i++) {
		backgroundTable[i].renderedFlag = false;
	}
}

bool BackgroundManager::remainingBackgroundTable() {
	bool cont = false;
	for (int i = 0; i < MAX_BACKGROUND_TABLE; i++) {
		if (backgroundTable[i].texture != NULL) {
			if (backgroundTable[i].renderedFlag == false) {
				return true;
			}
		}
	}
	return false;
}

float BackgroundManager::getShift() {
	if (shiftTable.count(internalCounter) == 1) {
		currentShift = shiftTable[internalCounter];
	}
	return currentShift;
}

BackRender BackgroundManager::getABackground() {
	for (int i = 0; i < MAX_BACKGROUND_TABLE; i++) {
		if (backgroundTable[i].texture != NULL) {
			if (backgroundTable[i].renderedFlag == false) {
				backgroundTable[i].renderedFlag = true;
				return backgroundTable[i];
			}
		}
	}
	err::logMessage("getABackGround returned nothing when remainingBackgroundTable() == TRUE");
	return backgroundTable[0];
}
