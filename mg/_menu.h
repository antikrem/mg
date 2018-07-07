#ifndef __MENU_H_INCLUDED__
#define __MENU_H_INCLUDED__

#include <atomic>
#include <chrono>

#include "sdl.h"
#include "level_enum.h"
#include "error.h"
#include "input.h"
#include "graphics_state.h"
using namespace std;

/*A SlaveInstance is a abstract object representing a section that can be controlled, like a level or menu
It can be overrided to change what the instance does*/
class SlaveInstance {
private:
	//Graphics environment
	GraphicsState* graphicsState;
	//Level state information
	LevelSettings* currentSettings = NULL;

	//Thread control variables
	atomic<bool> endSlave = false;
	atomic<bool> computerActive = true;
	atomic<bool> rendererActive = true;

	//thread safe input interface, recall Input is trivially defined
	atomic<Input> safeInput;

	//Safely get current input
	Input getInput() {
		return(safeInput.load());
	}

//LOGIC AND RENDERING FUNCTIONS//
	
	/*Logic to be conducted 300 times a second. Handled by computer()
	Needs to be over written*/
	virtual void computeCycle() {
	//	err::logMessage("computeCycle() not overwritten");
	}

	/*Logic to be conducted with no upper limit per second. Handled by renderer()
	Needs to be over written*/
	virtual void renderCycle() {
	//	err::logMessage("renderCycle() not overwritten");
	}

public:
	/*Kills thread, memory is retained, delete when readyToFree() is true*/
	void killThreads() {
		endSlave = true;
	}

	/*If true, safe to delete*/
	bool readyToFree() {
		return (!rendererActive && !computerActive);
	}

	/*This function will be called as a seperate thread and then disjoint, and handles game logic (e.g.) 300 tiems
	a second.
	Ends when endSlave is true (when killThreads() is envoked), and flip computerActive to false*/
	void computer() {
		chrono::high_resolution_clock::time_point startCyclePoint;
		chrono::high_resolution_clock::time_point currentCyclePoint;
		int cycle = 0;
		while (!endSlave) {
			startCyclePoint = chrono::high_resolution_clock::now();

			computeCycle();

			unsigned int i;
			do {
				currentCyclePoint = chrono::high_resolution_clock::now();
				i = (unsigned int) (chrono::duration_cast<std::chrono::microseconds>(currentCyclePoint - startCyclePoint)).count();
				if (i >= 3333) {
			//		err::logMessage(to_string(i));
				}
			} while ( i < 3333 );
		}

		computerActive = false;
	}

	/*This function will be called as a seperate thread and then disjoint, and handles graphics (e.g.) at
	an unlocked clock speed.
	Ends when endSlave is true (when killThreads() is envoked), and flip rendererActive to false*/
	void renderer() {
		while (!endSlave) {
			renderCycle();
		}

		rendererActive = false;
	}

	/*Initialises values required for rendering and other neccessary things*/
	void initialiseInstance(GraphicsState* graphicsState, LevelSettings* currentSettings) {
		this->graphicsState = graphicsState;
		this->currentSettings = currentSettings;
	}

	void setCurrentInput(Input newInput) {
		safeInput.store(newInput);
	}

};
#endif