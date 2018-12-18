#pragma once
#include "text_entity.h"
#include "input.h"
#include <windows.h>

#define CYCLESTEXTDISPLAY 50
#define SLIDEEND 15
#define SLIDESTART -240
#define PROFILEDISPLACEMENT 16
#define TEXTUPDATERATE 10
#define SKIPUPDATERATE 10
#define PAUSEWAITPERCHAR (float)10

class Dialogue {
private:
	int dialogueCycle = 0;
	
	SDL_Renderer * RENDERER;
	TextRenderer* TEXT_RENDERER;

	int noPauseWait = 0;

	bool skip = true;

	SDL_Texture* playerOverlay = NULL;
	SDL_Texture* enemyOverlay = NULL;

	SDL_Rect overlayRect = { 235, SLIDESTART, 930, 240 };
	SDL_Rect playerProfile = { 251, SLIDESTART + PROFILEDISPLACEMENT, 170, 208 };

	const CUS_Point playerTextPos = { 448, 31 };
	const CUS_Point enemyTextPos = { 266, 31 };
	SDL_Rect textRect;

	map<string, SDL_Texture*> characterProfiles;

	//For last profile
	string lastSide;
	string lastProfile;

	string textLine = "";
	vector<string> commands;
	vector<string> currentLine;

	mutex lock;

	//For rendering text
	SDL_Texture* textTex = NULL;
	
	void updateText() {
		SDL_Surface* textSurf = TTF_RenderText_Blended_Wrapped((*TEXT_RENDERER).fonts[sans28], textLine.c_str(), (*TEXT_RENDERER).colors[white], 686);
		textTex = SDL_CreateTextureFromSurface(TEXT_RENDERER->gRenderer, textSurf);
		if (
			textSurf)
			textRect = { 0, 0, textSurf->w, textSurf->h };
		SDL_FreeSurface(textSurf);
	}

public:
	Dialogue(TextRenderer* TEXT_RENDERER, LevelSettings* levelSettings) {
		this->RENDERER = TEXT_RENDERER->gRenderer;
		this->TEXT_RENDERER = TEXT_RENDERER;
		SDL_Surface* playerOverlaySurf = SDL_LoadBMP("assets\\UI\\dialogue.bmp");
		if (!playerOverlaySurf)
			err::logMessage("ERROR: assets\\UI\\dialogue.bmp is missing, reinstall/verify game");
		playerOverlay = SDL_CreateTextureFromSurface(RENDERER, playerOverlaySurf);
		if (!playerOverlay)
			err::logMessage("ERROR: player dialogue texture could not be loaded to VRAM, reinstall graphics drivers");

		SDL_Surface* otherOverlaySurf = SDL_LoadBMP("assets\\UI\\dialogue1.bmp");
		if (!otherOverlaySurf)
			err::logMessage("ERROR: assets\\UI\\dialogue1.bmp is missing, reinstall/verify game");
		enemyOverlay = SDL_CreateTextureFromSurface(RENDERER, otherOverlaySurf);
		if (!enemyOverlay)
			err::logMessage("ERROR: enemy dialogue texture could not be loaded to VRAM, reinstall graphics drivers");

		HANDLE fileHandle;
		WIN32_FIND_DATA data;
		string path = "assets\\UI\\in_game\\character_profiles\\";
		string searchPath = path + "*.bmp";
		fileHandle = FindFirstFile(searchPath.c_str(), &data);
		if (fileHandle != INVALID_HANDLE_VALUE) {
			do {
				string file = data.cFileName;
				string fileContents = path+file;
				file = file.substr(0, file.size() - 4);
				
				SDL_Surface *surf = SDL_LoadBMP(fileContents.c_str());
				if (!surf)
					err::logMessage("Warning: Missing or invalid file: " + fileContents + ", profile skipped");
				else {
					if not( 0.8 < (surf->w / surf->h) && (surf->w / surf->h) < 0.83 )
						err::logMessage("Warning: profile at " + fileContents + " is of deviant aspect ratio. Recommended aspect ratio is a scale of 170:208");
					SDL_Texture* tex = SDL_CreateTextureFromSurface(RENDERER, surf);
					SDL_FreeSurface(surf);
					if (!tex)
						err::logMessage("Warning: profile at " + fileContents + " was not loaded on VRAM, reinstall graphics drivers");
					else
						characterProfiles[file] = tex;
				}
			} while (FindNextFile(fileHandle, &data));
			FindClose(fileHandle);
			updateText();
		}
		
		string filePath = "campaigns\\" + levelSettings->campaign + "\\" + to_string(levelSettings->level) + "\\" + "dialogue_table.txt";

		int lineNo = 0;
		ifstream inFile;
		inFile.open(filePath.c_str());
		if (!inFile)
			err::logMessage("The file at " + filePath + " was not able to be opened");

		string line;



		while (getline(inFile, line)) {
			lineNo++;
			if not(line[0] == '/' && line[1] == '/')
				commands.push_back(line);
		}
		commands.push_back( "1000000000;k;Last;1;You really don't need to see this" );
		currentLine = str_kit::splitOnToken(commands[0], ';');
	}

	~Dialogue() {
		for (auto i : characterProfiles) {
			SDL_DestroyTexture(i.second);
		}
	}

	/*Returns true if its time to stop iterating the cycles
	Else returns false
	commands are stored as (int)cycle;;("p"/"e")side(string)character;("0"/"1")pause;(string)message*/
	bool updateDialogue(int cycle, Input input) {
		dialogueCycle++;
		//Reset skip button
		if (skip == false && !input.shoot) {
			skip = true;
		}
		if (cycle >= stoi(currentLine[0])) {
			//Wait for slide to be in place
			if (overlayRect.y < SLIDEEND) {
				overlayRect.y += 4;
				playerProfile.y = overlayRect.y + PROFILEDISPLACEMENT;
			}
			//When slide in place
			else {
				//wait for text to fully render
				if (textLine.size() != currentLine[4].size()) {
					//but only render on every so other cycle
					if (dialogueCycle%TEXTUPDATERATE == 0) {
						string in(1, currentLine[4][textLine.size()]);
						textLine.append(in);
						updateText();
					}
					//unless it is a pause type message box, in which case proper input skips
					if (currentLine[3] == "1" && skip && input.shoot) {
						skip = false;
						textLine = currentLine[4];
						updateText();
					}
				}
				//If text is rendered
				else {
					//If it is pause type 
					if (currentLine[3] == "1") {
						if (skip && input.shoot) {
							commands.erase(commands.begin());
							skip = false;
							currentLine = str_kit::splitOnToken(commands[0], ';');
							textLine = "";
							updateText();
						}
					}
					//Else if its not pause type
					else if (currentLine[3] == "0") {
						noPauseWait++;
						if ((float)noPauseWait > PAUSEWAITPERCHAR * currentLine[4].size()) {
							noPauseWait = 0;
							commands.erase(commands.begin());
							currentLine = str_kit::splitOnToken(commands[0], ';');
							textLine = "";
							updateText();
						}
					}
				}

			}
		}
		else if (cycle < stoi(currentLine[0])) {
			if (overlayRect.y > SLIDESTART) {
				overlayRect.y -= 4;
				playerProfile.y = overlayRect.y + PROFILEDISPLACEMENT;
			}
		}

		if (currentLine[3] == "1" && cycle == stoi(currentLine[0])) {
			return true;
		}
		else {
			return false;
		}
	}

	void drawDialogue() {
		lock.lock();
		if (overlayRect.y != SLIDESTART) {
			if (currentLine[1] == "p") {
				SDL_RenderCopy(RENDERER, playerOverlay, NULL, &overlayRect);
				SDL_RenderCopy(RENDERER, characterProfiles[currentLine[2]], NULL, &playerProfile);
				lastSide = "p";
				lastProfile = currentLine[2];
				textRect.x = (int) playerTextPos.x;
				textRect.y = (int) playerTextPos.y;
			}
			else if (currentLine[1] == "e") {
				SDL_RenderCopy(RENDERER, enemyOverlay, NULL, &overlayRect);
				playerProfile.x += 728;
				SDL_RenderCopy(RENDERER, characterProfiles[currentLine[2]], NULL, &playerProfile);
				playerProfile.x -= 728;
				lastSide = "e";
				lastProfile = currentLine[2];
				textRect.x = (int) enemyTextPos.x;
				textRect.y = (int) enemyTextPos.y;
			}
			else if (currentLine[1] == "k") {
				if (lastSide == "p") {
					SDL_RenderCopy(RENDERER, playerOverlay, NULL, &overlayRect);
					SDL_RenderCopy(RENDERER, characterProfiles[lastProfile], NULL, &playerProfile);
				}
				else if (lastSide == "e") {
					SDL_RenderCopy(RENDERER, enemyOverlay, NULL, &overlayRect);
					playerProfile.x += 728;
					SDL_RenderCopy(RENDERER, characterProfiles[lastProfile], NULL, &playerProfile);
					playerProfile.x -= 728;
				}
			}
			if not (overlayRect.y < SLIDEEND) {
				SDL_RenderCopy(RENDERER, textTex, NULL, &textRect);
			}
			
		}
		lock.unlock();
	}
};