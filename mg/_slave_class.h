#ifndef __SLAVE_CLASS_H_INCLUDED__
#define __SLAVE_CLASS_H_INCLUDED__

#include <atomic>
#include <chrono>
#include <vector>

#include "sdl.h"
#include "level_enum.h"
#include "error.h"
#include "input.h"
#include "graphics_state.h"
#include "listed_entities.h"
#include "text_entity.h"

using namespace std;

/*A SlaveInstance is a abstract object representing a section that can be controlled, like a level or menu
It can be overrided to change what the instance does*/
class SlaveInstance {
protected:
	//current master cycle
	int cycle = 0;

	//Graphics environment
	GraphicsState* graphicsState;
	//Text environment
	TextRenderer* textRenderer = NULL;
	//Level state information
	LevelSettings* currentSettings = NULL;

	//Thread control variables
	atomic<bool> initialised = false;
	atomic<bool> endSlave = false;
	atomic<bool> computerActive = true;
	atomic<bool> rendererActive = true;

	//FPS draw variables
	float fps = 0;
	vector<float> FPSVector;
	atomic<bool> pushFPS = false;
	TextContainer* fpsContainer = NULL;

	//thread safe input interface, recall Input is trivially defined
	atomic<Input> safeInput;

	//Safely get current input
	Input getInput();

//OVERWRITTING LOGIC AND RENDERING FUNCTIONS//
	/*Runs once before gameloop begins, runs async to master loop
	Any heap memory allocated must be cleared in memFree()
	Needs to be overwritten*/
	virtual void initialise();

	/*Clears memory of instance
	Must clean up initialise() to avoid a leak
	Needs to be overwritten*/
	virtual void memFree();

	/*Logic to be conducted 300 times a second. Handled by computer()
	Needs to be overwritten*/
	virtual void computeCycle();

	/*Logic to be conducted with no upper limit per second. Handled by renderer()
	Needs to be overwritten*/
	virtual void renderCycle();

//DRAW FUNCTIONS
	void drawBoxEntity(RenderInformation drawTarget, int shift, float postScale = 0) {
		SDL_Rect copyReference = drawTarget.renderSize;

		if (postScale != 0) {
			copyReference.x = (int)( (copyReference.w * postScale - copyReference.w) / 2 );
			copyReference.y = (int)((copyReference.h * postScale - copyReference.h) / 2);
			copyReference.w = (int)(copyReference.w * postScale);
			copyReference.h = (int)(copyReference.h * postScale);
		}

		copyReference.x += shift;
		copyReference.x += 220;
		copyReference.y += 10;
		if (drawTarget.alpha > 250) {
			SDL_RenderCopyEx(graphicsState->getGRenderer(), drawTarget.currentFrame, NULL, &copyReference, (double)drawTarget.angle, NULL, SDL_FLIP_NONE);
		}
		else if (drawTarget.alpha > 0) {
			SDL_SetTextureAlphaMod(drawTarget.currentFrame, (int)drawTarget.alpha);
			SDL_RenderCopyEx(graphicsState->getGRenderer(), drawTarget.currentFrame, NULL, &copyReference, (double)drawTarget.angle, NULL, SDL_FLIP_NONE);
			SDL_SetTextureAlphaMod(drawTarget.currentFrame, 255);
		}
		else {
			1;
		}
	}

public:
	/*Kills thread, memory is retained, delete when readyToFree() is true*/
	void killThreads();

	/*If true, safe to delete*/
	bool readyToFree();

	/*This function will be called as a seperate thread and then disjoint, and handles game logic (e.g.) 300 tiems
	a second.
	Ends when endSlave is true (when killThreads() is envoked), and flip computerActive to false*/
	void computer();

	/*This function will be called as a seperate thread and then disjoint, and handles graphics (e.g.) at
	an unlocked clock speed.
	Ends when endSlave is true (when killThreads() is envoked), and flip rendererActive to false*/
	void renderer();

	/*Initialises values required for rendering and other neccessary things*/
	void initialiseInstance(GraphicsState* graphicsState, TextRenderer* textRenderer, LevelSettings* currentSettings);

	void setCurrentInput(Input newInput);

};
#endif