#ifndef __SLAVE_CLASS_H_INCLUDED__
#define __SLAVE_CLASS_H_INCLUDED__

#define NUMBER_OF_CONSOLE_MESSAGES 5

#define FULLSIZE 0

#include <atomic>
#include <chrono>
#include <vector>

#include "constants.h"
#include "sdl.h"
#include "level_enum.h"
#include "error.h"
#include "input.h"
#include "listed_entities.h"
#include "text_entity.h"
#include "parallax.h"
#include "sound.h"

using namespace std;

/*A SlaveInstance is a abstract object representing a section that can be controlled, like a level or menu
It can be overrided to change what the instance does*/
class SlaveInstance {
protected:
	//current master cycle
	int cycle = 0;
	//Seperate from cycle, controls timings for static events
	int counter = -1;
	//If true, the counter will not increase
	bool stuckCounter = false;

	//Master command
	map<int, vector<string>> commandList;

	//Graphics environment
	GraphicsState* graphicsState;
	//Text environment
	TextRenderer* textRenderer = NULL;
	//Enhanced text environment
	TextGlobalMaster* textGlobalMaster = NULL;

	LevelSettings* levelSettingsCurrent = NULL;

	//Thread control variables
	atomic<bool> initialised = false;
	atomic<bool> endSlave = false;
	atomic<bool> endParticler = false;
	atomic<bool> computerActive = true;
	atomic<bool> rendererActive = true;
	atomic<bool> particleActive = false;

	//FPS report draw variables
	float fps = 0;
	vector<int> FPSVector;
	atomic<bool> pushFPS = false;
	TextMaster* fpsTextMaster = NULL;

	//Cycle report draw variables
	atomic<int> cycleDrawCounter = 0;
	atomic<int> cycleLoadDrawCounter = 0;
	atomic<int> cyclesPerDraw = 0;

	//particle report draw variables
	atomic<float> particleLoad = 0;
	atomic<int> particleMasterCount = 0;
	atomic<int> particleSlaveCount = 0;
	atomic<int> particleGroupCount = 0;

	//Weather report draw variables
	atomic<int> weatherToReport = 0;
	atomic<int> numberOfWeatherClips[3] = { 0,0,0 }; //0->lowest, 2->highest

	//Box entity information varaibles
	int boxCountRunningTotal = 0;
	atomic<int> boxCountCurrent = 0;

	//Particle manager
	ParticleManager* particleMaster = NULL;
	//Total wind for particle master
	atomic<CUS_Point> totalWindSum;

	//Console Variables
	atomic<bool> inConsole = false;
	int consoleCounter = 0;
	int eraseCounter = 20;
	char nextIn = (char)0;
	string consoleBuffer[NUMBER_OF_CONSOLE_MESSAGES];
	string currentConsoleLine;
	TextMaster* consoleTextMaster = NULL;

	/*Updates console view, feed bool to push history*/
	void updateConsoleView(bool push);

	/*A function that computes the current cycles input into console update*/
	void consoleUpdate();

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

	/*Level specific commands that are tried after general commands
	Needs to be over written*/
	virtual void levelProcessCommand(string command, bool fromMaster);

	/*Takes a command and updates the gamestate*/
	void processCommand(string command, bool fromMaster);

	/*Checks command list,
	computes if there are commands this cycle*/
	void computeFromCommandList();

	/*Loads commands from file,
	Adds timed commands to list, and executes the else */
	void loadCommandList();

//DRAW FUNCTIONS
	/*darkness is brightest at zero, and completly black at 255
	return 0 (no draw), 1 (simple draw), 2 (lighting draw)*/
	int drawBoxEntity(RenderInformation drawTarget, int shift, int darkness, float postScale) {
		boxCountRunningTotal++;
		SDL_Rect copyReference = drawTarget.renderSize;

		if not(drawTarget.inView)
			return 0;

		if (postScale != 0) {
			copyReference.w = (int)((float)copyReference.w * postScale);
			copyReference.h = (int)((float)copyReference.h * postScale);
			copyReference.x = (int)( (float)copyReference.x  - ((float)copyReference.w / 2) );
			copyReference.y = (int)( (float)copyReference.y  - ((float)copyReference.h / 2) );
		}

		copyReference.x += shift;
		copyReference.x += 220;
		if (drawTarget.alpha > 250) {
			SDL_RenderCopyEx(graphicsState->getGRenderer(), drawTarget.currentFrame, NULL, &copyReference, (double)drawTarget.angle, NULL, SDL_FLIP_NONE);
		}
		else if (drawTarget.alpha > 0) {
			SDL_SetTextureAlphaMod(drawTarget.currentFrame, (int)drawTarget.alpha);
			SDL_RenderCopyEx(graphicsState->getGRenderer(), drawTarget.currentFrame, NULL, &copyReference, (double)drawTarget.angle, NULL, SDL_FLIP_NONE);
			SDL_SetTextureAlphaMod(drawTarget.currentFrame, 255);
		}
		else { //If there isn't anything to draw, leave early so a shadow isn't drawn
			return 0;
		}

		//Add shadow
		if (darkness > 0) {
			if (drawTarget.alpha > 250) {
				SDL_SetTextureAlphaMod(drawTarget.shadowFrame, darkness);
				SDL_RenderCopyEx(graphicsState->getGRenderer(), drawTarget.shadowFrame, NULL, &copyReference, (double)drawTarget.angle, NULL, SDL_FLIP_NONE);
			}
			else if (drawTarget.alpha > 0) {
				SDL_SetTextureAlphaMod( drawTarget.shadowFrame, (int)((((float)drawTarget.alpha) / 255) * (float)darkness) );
				SDL_RenderCopyEx(graphicsState->getGRenderer(), drawTarget.shadowFrame, NULL, &copyReference, (double)drawTarget.angle, NULL, SDL_FLIP_NONE);
			}
		}
		//Or add light
		else if (darkness < 0) {
			if (drawTarget.alpha > 250) {
				SDL_SetTextureAlphaMod(drawTarget.lightFrame, -1*darkness);
				SDL_RenderCopyEx(graphicsState->getGRenderer(), drawTarget.lightFrame, NULL, &copyReference, (double)drawTarget.angle, NULL, SDL_FLIP_NONE);
			}
			else if (drawTarget.alpha > 0) {
				SDL_SetTextureAlphaMod(drawTarget.lightFrame, -1 * (int)((((float)drawTarget.alpha) / 255) * (float)darkness));
				SDL_RenderCopyEx(graphicsState->getGRenderer(), drawTarget.lightFrame, NULL, &copyReference, (double)drawTarget.angle, NULL, SDL_FLIP_NONE);
			}
		}
		else {
			return 1;
		}
		return 2;
	}

public:
	/*Kills thread, memory is retained, delete when readyToFree() is true*/
	void killThreads();

	/*If true, safe to delete*/
	bool readyToFree();

	/*Called as a seperate thread and then disjoint, and handles game logic (e.g.) 300 tiems
	a second.
	Ends when endSlave is true (when killThreads() is envoked), and flips computerActive to false*/
	void computer();

	/*Called as a seperate thread and then disjoint, and handles graphics (e.g.) at
	an unlocked clock speed.
	Ends when endSlave is true (when killThreads() is envoked), and flips rendererActive to false*/
	void renderer();

	/*Optional call by computer thread, that is called by computer.
	Logic runs 150 times a second updating particles asyncronous of compute*/
	void particler();

	/*Initialises values required for rendering and other neccessary things*/
	void initialiseInstance(GraphicsState* graphicsState, TextRenderer* textRenderer, LevelSettings* currentSettings, TextGlobalMaster* txtGlobalMaster);

	void setCurrentInput(Input newInput);

	/*get a pointer to console character input*/
	char* getConsoleCharInput() {
		return &nextIn;
	}

};
#endif