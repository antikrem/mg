#include "_slave_class.h"

#include <atomic>
#include <chrono>
#include <vector>
#include <thread>

#include "sdl.h"
#include "level_enum.h"
#include "error.h"
#include "input.h"
#include "graphics_state.h"
#include "text_entity.h"
#include "str_kit.h"

/*Returns a string for WeatherType, for debug*/
static string weather2String(int wt) {
	switch (wt) {
	case 0:
		return "none";
	case 1:
		return "rain";
	}
	return "oop"; //TODO
}

Input SlaveInstance::getInput() {
	auto press = safeInput.load();
	if (inConsole) {
		press.up = false;
		press.down = false;
		press.left = false;
		press.right = false;
		press.shift = false;
		press.dash = false;
		press.shoot = false;
		press.special = false;
	}
	return(press);
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

void SlaveInstance::processCommand(string command) {
	cout << command << endl;
}

void SlaveInstance::computeFromCommandList() {
	if (commandList.count(counter)) {
		for (auto command : commandList[counter]) {
			processCommand(command);
		}
		commandList.erase(counter);
	}
}

void SlaveInstance::loadCommandList() {
	string filePath = "campaigns\\" + levelSettingsCurrent->campaign + "\\" + to_string(levelSettingsCurrent->level) + "\\" + "level_master.txt";

	int lineNo = 0;
	ifstream inFile;
	inFile.open(filePath.c_str());
	string line;

	locale loc;

	//TODO: try catch ex
	while (getline(inFile, line)) {
		lineNo++;
		auto lineVec = str_kit::splitOnToken(line, ' ');
		if (line == "") {
			pass;
		}
		else if ((line[0] == '/' && line[1] == '/')) {
			pass;
		}
		else if (lineVec.size() == 0) {
			pass;
		}
		else if (str_kit::isADigit(lineVec[0]) && (lineVec.size() > 1)) {
			string reconstruction;
			for (unsigned int i = 1; i < lineVec.size(); i++)
				reconstruction.append(lineVec[i] + " ");
			reconstruction.pop_back();
			commandList[stoi(lineVec[0])].push_back(reconstruction);
		}
		else {
			processCommand(line);
		}
	}
}

void SlaveInstance::consoleUpdate() {
	if (nextIn != (char)0) {
		//In console so input is processed
		if (inConsole) {
			//Input has been released and needs to be added to current console update
			char freshInput = nextIn;
			nextIn = (char)0;
			//First check backspaces
			if ((int)freshInput == 8 && currentConsoleLine.size() > 0) {
				cout << "erase" << endl;
				currentConsoleLine = currentConsoleLine.substr(0, currentConsoleLine.size() - 1);
			}
			//Check return, process console line
			else if (freshInput == SDLK_RETURN) {
				if (currentConsoleLine.size() > 0) {
					processCommand(currentConsoleLine);
					currentConsoleLine.clear();
				}
			}
			//Check spaces
			else if (freshInput == SDLK_SPACE) {
				currentConsoleLine.append(" ");
			}
			//This only leaves characters to add
			else {
				currentConsoleLine.append(string(1, freshInput));
			}

		}
		//Not in console so input ignores
		else {
			nextIn = (char)0;
		}
	}

}

void SlaveInstance::killThreads() {
	endSlave = true;
}

bool SlaveInstance::readyToFree() {
	return (!rendererActive && !computerActive && !particleActive);
}

void SlaveInstance::computer() {
	initialise();
	initialised = true;

	thread particleThread(&SlaveInstance::particler, this);
	particleThread.detach();

	chrono::high_resolution_clock::time_point startCyclePoint;
	chrono::high_resolution_clock::time_point currentCyclePoint;
	while (!endSlave) {
		startCyclePoint = chrono::high_resolution_clock::now();
		
		fpsContainer->updateAllTextEnts();

		cycle++;
		err::setLoggingCycle(cycle);

		//Handle console
		consoleCounter--;
		if (getInput().console && consoleCounter < 0) {
			inConsole = !inConsole;
			consoleCounter = 100;
		}
		consoleUpdate();

		if (!stuckCounter)
			counter++;
		computeCycle();

		if not(cycle % 3)
			pushFPS = true;

		unsigned int i;
		do {
			currentCyclePoint = chrono::high_resolution_clock::now();
			i = (unsigned int)(chrono::duration_cast<std::chrono::microseconds>(currentCyclePoint - startCyclePoint)).count();
		} while (i < 3333);
		cycleDrawCounter += (((int)i - 3333) >= 0) ? (i - 3333) : 0;
		cyclesPerDraw++;
	}

	err::setLoggingCycle(0);

	endParticler = true;
	
	//Wait for particler to end before freeing memory
	while (particleActive) {
		cout << "waitingForParticle" << endl;
	}
	
	//Wait for renderer to end before freeing memory
	while (rendererActive) {
		cout << "waitingForRenderer" << endl;
	}
	memFree();
	delete fpsContainer;
	
	computerActive = false;
}

void SlaveInstance::renderer() {
	chrono::high_resolution_clock::time_point startCyclePoint;
	chrono::high_resolution_clock::time_point currentCyclePoint;
	while (!endSlave) {
		if (initialised) {
			startCyclePoint = chrono::high_resolution_clock::now();
			boxCountRunningTotal = 0;

			renderCycle();

			boxCountCurrent = boxCountRunningTotal;

			//Wait if the framerate is limited
			if (graphicsState->getFrameLimit()) {
				unsigned int i;
				int frameLimit = (int)(F(1e6) / graphicsState->getFrameLimit());
				do {
					currentCyclePoint = chrono::high_resolution_clock::now();
					i = (unsigned int)(chrono::duration_cast<std::chrono::microseconds>(currentCyclePoint - startCyclePoint)).count();
				} while (i < (unsigned int)frameLimit);
			}

			currentCyclePoint = chrono::high_resolution_clock::now();
			
			//handle fps counter, every 1000 cycles
			if (graphicsState->getShowFPS()) {
				if (pushFPS) {
					fps = (float) 1e9 / ((chrono::duration_cast<std::chrono::nanoseconds>(currentCyclePoint - startCyclePoint)).count());
					FPSVector.push_back(fps);
					pushFPS = false;
					if (FPSVector.size() >= 20) {
						float average = 0;
						for (float i : FPSVector) 
							average += i;

						average = (average / FPSVector.size());
						
						fpsContainer->updateTextByKey("fps", "fps: " + to_string((int)average));
						fpsContainer->updateTextByKey("dropped", "drop: " + to_string((float)cycleDrawCounter/(3333* cyclesPerDraw)) + "%");
						fpsContainer->updateTextByKey("boxes", "boxes: " + to_string(boxCountCurrent));
						fpsContainer->updateTextByKey("particle", "p_master: " + to_string(particleMasterCount) + " (in " + to_string(particleGroupCount) + ") with " + to_string(particleSlaveCount) + " at "+ to_string(particleLoad) + "%");
						fpsContainer->updateTextByKey("cycle", "cycle: " + to_string(counter) + (stuckCounter ? " STUCK" : " FREE"));
						fpsContainer->updateTextByKey("weather", "weather: " + weather2String(weatherToReport) + " | B: " + to_string(numberOfWeatherClips[0]) + " | M: " + to_string(numberOfWeatherClips[1]) + " | A: " + to_string(numberOfWeatherClips[2]));
						cycleDrawCounter = 0;
						cyclesPerDraw = 0;
						FPSVector.clear();
					}
				}
			}
			else {
				fpsContainer->updateTextByKey("fps", " ");
				fpsContainer->updateTextByKey("dropped", " ");
				fpsContainer->updateTextByKey("boxes", " ");
				fpsContainer->updateTextByKey("particle", " ");
				fpsContainer->updateTextByKey("cycle", " ");
				fpsContainer->updateTextByKey("weather", " ");
			}
			fpsContainer->renderText(0);
		}
		graphicsState->renderFrame();
	}
	rendererActive = false;
}

void SlaveInstance::particler() {
	chrono::high_resolution_clock::time_point startCyclePoint;
	chrono::high_resolution_clock::time_point currentCyclePoint;
	
	int cycle= 0;
	particleActive = true;
	while (!endParticler) {
		if (particleMaster) {
			startCyclePoint = chrono::high_resolution_clock::now();
			cycle++;
			particleMaster->grandParticleUpdate(startCyclePoint, totalWindSum.load());
			particleSlaveCount = particleMaster->getParticleCount();
			particleMasterCount = particleMaster->getApplierCount();
			particleGroupCount = particleMaster->getNumberOfGroups();

			currentCyclePoint = chrono::high_resolution_clock::now();
			unsigned int i = (unsigned int)(chrono::duration_cast<std::chrono::nanoseconds>(currentCyclePoint - startCyclePoint)).count();
			particleLoad = (float)i / 6666666;
			do {
				currentCyclePoint = chrono::high_resolution_clock::now();
				i = (unsigned int)(chrono::duration_cast<std::chrono::microseconds>(currentCyclePoint - startCyclePoint)).count();
			} while (i < 6666);
		}
	}

	particleActive = false;
}

void SlaveInstance::initialiseInstance(GraphicsState* graphicsState, TextRenderer* textRenderer, LevelSettings* currentSettings) {
	this->textRenderer = textRenderer;
	this->graphicsState = graphicsState;
	this->levelSettingsCurrent = currentSettings;
	this->particleMaster = new ParticleManager(graphicsState);

	totalWindSum.store({ 0,0 });

	fpsContainer = new TextContainer(textRenderer);
	fpsContainer->newTextEnt("dropped", " ", { (float)5, (float)5 }, sans28, white, topLeft, false,
		{ 0, 0 }, { 0, 0 }, 2147480000, (float)254, 0);
	fpsContainer->newTextEnt("fps", " ", { (float)5, (float)33 }, sans28, white, topLeft, false,
		{ 0, 0 }, { 0, 0 }, 2147480000, (float)254, 0);
	fpsContainer->newTextEnt("boxes", " ", { (float)5, (float)61 }, sans28, white, topLeft, false,
		{ 0, 0 }, { 0, 0 }, 2147480000, (float)254, 0);
	fpsContainer->newTextEnt("particle", " ", { (float)5, (float)89 }, sans28, white, topLeft, false,
		{ 0, 0 }, { 0, 0 }, 2147480000, (float)254, 0);
	fpsContainer->newTextEnt("cycle", " ", { (float)5, (float)117 }, sans28, white, topLeft, false,
		{ 0, 0 }, { 0, 0 }, 2147480000, (float)254, 0);
	fpsContainer->newTextEnt("weather", " ", { (float)5, (float)145 }, sans28, white, topLeft, false,
		{ 0, 0 }, { 0, 0 }, 2147480000, (float)254, 0);
	
}

void SlaveInstance::setCurrentInput(Input newInput) {
	safeInput.store(newInput);
}

