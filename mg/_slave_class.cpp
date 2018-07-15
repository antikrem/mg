#include "_slave_class.h"

#include <atomic>
#include <chrono>
#include <vector>

#include "sdl.h"
#include "level_enum.h"
#include "error.h"
#include "input.h"
#include "graphics_state.h"
#include "text_entity.h"


Input SlaveInstance::getInput() {
	return(safeInput.load());
}

void SlaveInstance::initialise() {
	err::logMessage("initialise() not overwritten");
}

void SlaveInstance::memFree() {
	err::logMessage("memFree() not overwritten");
}

void SlaveInstance::computeCycle() {
	err::logMessage("computeCycle() not overwritten");
}

void SlaveInstance::renderCycle() {
	err::logMessage("renderCycle() not overwritten");
}

void SlaveInstance::killThreads() {
	endSlave = true;
}

bool SlaveInstance::readyToFree() {
	return (!rendererActive && !computerActive);
}

void SlaveInstance::computer() {
	initialise();
	fpsContainer = new TextContainer(textRenderer);
	if (graphicsState->getShowFPS()) {
		fpsContainer->newTextEnt("fps", "FPS", { 5, 5 }, sans28, gold, topLeft, true);
	}
	else {
		fpsContainer->newTextEnt("fps", " ", { 5, 5 }, sans28, gold, topLeft, true);
	}
	initialised = true;

	chrono::high_resolution_clock::time_point startCyclePoint;
	chrono::high_resolution_clock::time_point currentCyclePoint;
	int cycle = 0;
	while (!endSlave) {
		startCyclePoint = chrono::high_resolution_clock::now();

		cycle++;
		err::setLoggingCycle(cycle);
		computeCycle();

		pushFPS = true;

		unsigned int i;
		do {
			currentCyclePoint = chrono::high_resolution_clock::now();
			i = (unsigned int)(chrono::duration_cast<std::chrono::microseconds>(currentCyclePoint - startCyclePoint)).count();
		} while (i < 3333);
	}

	err::setLoggingCycle(0);

	//Wait for renderer to end before freeing memory
	while (rendererActive) {
		cout << "waiting2" << endl;
	}
	memFree();
	delete fpsContainer;
	fpsContainer = NULL;
	computerActive = false;
}

void SlaveInstance::renderer() {
	chrono::high_resolution_clock::time_point startCyclePoint;
	chrono::high_resolution_clock::time_point currentCyclePoint;
	while (!endSlave) {
		if (initialised) {
			startCyclePoint = chrono::high_resolution_clock::now();

			renderCycle();

			currentCyclePoint = chrono::high_resolution_clock::now();

			//handle fps counter, every 1000 cycles
			if (graphicsState->getShowFPS()) {
				if (pushFPS) {
					fps = (float) 1e9 / ((chrono::duration_cast<std::chrono::nanoseconds>(currentCyclePoint - startCyclePoint)).count());
					pushFPS = false;
					FPSVector.push_back(fps);

					if (FPSVector.size() >= 50) {
						float average = 0;

						for (float i : FPSVector) {
							average += i;
						}
						average = (average / FPSVector.size());
						fpsContainer->updateTextByKey("fps", to_string((int)average));
						FPSVector.clear();
					}
				}
			}
			else {
				fpsContainer->updateTextByKey("fps", " ");
			}

			fpsContainer->renderText(0);
			graphicsState->renderFrame();
		}
	}
	rendererActive = false;
}

void SlaveInstance::initialiseInstance(GraphicsState* graphicsState, TextRenderer* textRenderer, LevelSettings* currentSettings) {
	this->textRenderer = textRenderer;
	this->graphicsState = graphicsState;
	this->currentSettings = currentSettings;
}

void SlaveInstance::setCurrentInput(Input newInput) {
	safeInput.store(newInput);
}