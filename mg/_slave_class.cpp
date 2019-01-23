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

void SlaveInstance::levelProcessCommand(string command, bool fromMaster) {
	cout << command << endl;
}

void SlaveInstance::processCommand(string command, bool fromMaster) {
	auto lineVec = str_kit::splitOnToken(command, ' ');
	if (fromMaster)
		err::logConsoleMessage("Master has called: [" + command + "]");
	if (lineVec[0] == "QUIT") {
		levelSettingsCurrent->endGame = true;
	}
	else if (lineVec[0] == "FOV") {
		if (lineVec.size() != 2) {
			if (fromMaster)
				err::logMessage("FOV was called by master on cycle " + to_string(counter) + " with invalid number of lines " + to_string(lineVec.size() - 1) + " expected 1 parameter");
			else
				err::logConsoleMessage("Expected FOV + 1 parameter, got: " + to_string(lineVec.size() - 1));
		}
		else {
			try {
				para::setFOV(stof(lineVec[1]));
				err::logConsoleMessage("FOV set to: " + to_string(para::getFOV()));
				err::logConsoleMessage("Distance to plane set to: " + to_string(para::getDistanceFromPlane()));
			}
			catch (const std::exception & ex) {
				ex.what();
				if (fromMaster)
					err::logMessage("FOV was called by master as " + command + " which has an invalid parameter at line " + to_string(counter));
				else
					err::logConsoleMessage("Requested value was not a number");
			}
		}
	}

	else if (lineVec[0] == "DFP") {
		if (lineVec.size() != 2) {
			if (fromMaster)
				err::logMessage("DFP was called by master on cycle " + to_string(counter) + " with invalid number of lines " + to_string(lineVec.size() - 1) + " expected 1 parameter");
			else
				err::logConsoleMessage("Expected DFP + 1 parameter, got: " + to_string(lineVec.size() - 1));
		}
		else {
			try {
				para::setDistanceFromPlane(stof(lineVec[1]));
				err::logConsoleMessage("FOV set to: " + to_string(para::getFOV()));
				err::logConsoleMessage("Distance from plane set to: " + to_string(para::getDistanceFromPlane()));
			}
			catch (const std::exception & ex) {
				ex.what();
				if (fromMaster)
					err::logMessage("DFP was called by master as " + command + " which has an invalid parameter at line " + to_string(counter));
				else
					err::logConsoleMessage("Requested value was not a number");
			}
		}
	}
	else if (lineVec[0] == "MUSIC") {
		if (lineVec.size() != 3) {
			if (!fromMaster)
				err::logConsoleMessage("Expected command + 2 parameters, got: " + to_string(lineVec.size() - 1));
			else {
				err::logMessage("MUSIC was called by master on cycle " + to_string(counter) + " with invalid number of lines " + to_string(lineVec.size() - 1) + " expected 1 parameter");
				err::logConsoleMessage("MUSIC was called by master on cycle " + to_string(counter) + " with invalid number of lines " + to_string(lineVec.size() - 1) + " expected 1 parameter");
			}

		}
		else if (lineVec[1] == "PLAY") {

			if (checkMusic(lineVec[2])) {
				playMusic(lineVec[2]);
				err::logConsoleMessage("Playing Track: " + lineVec[2]);
			}
			else {
				if (fromMaster) {
					err::logConsoleMessage("Master tried to play music that isn't in the library: " + lineVec[2]);
					err::logMessage("Master tried to play music that isn't in the library: " + lineVec[2]);
				}
				else
					err::logConsoleMessage("Invalid parameter for MUSIC PLAY");
			}
		}
		else {
			if (!fromMaster)
				err::logConsoleMessage("MUSIC was not given a proper command: " + lineVec[1]);
			else {
				err::logMessage("MUSIC was not given a proper command: " + lineVec[1] + " on cyle " + to_string(counter));
				err::logConsoleMessage("MUSIC was not given a proper command: " + lineVec[1]);
			}

		}
	}
	else if (lineVec[0] == "BACKGROUND") {
		pass;
	}
	else {
		levelProcessCommand(command, fromMaster);
	}
	
}

void SlaveInstance::computeFromCommandList() {
	if (commandList.count(counter)) {
		for (auto command : commandList[counter]) {
			processCommand(command, true);
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
			processCommand(line, true);
		}
	}
}

void SlaveInstance::updateConsoleView(bool push) {
	string newLine;
	newLine.append(": ");
	if (!push) {
		newLine.append(currentConsoleLine);
	}
	else {
		for (int i = 0; i < NUMBER_OF_CONSOLE_MESSAGES-1; i++) {
			consoleBuffer[i].clear();
			consoleBuffer[i].append(consoleBuffer[i + 1]);
		}
		consoleBuffer[NUMBER_OF_CONSOLE_MESSAGES - 1].clear();
		consoleBuffer[NUMBER_OF_CONSOLE_MESSAGES-1].append(currentConsoleLine);
	}

	consoleTextMaster->updateTextByKey("current", newLine);
	for (int i = 0; i < NUMBER_OF_CONSOLE_MESSAGES; i++) {
		consoleTextMaster->updateTextByKey(to_string(i), consoleBuffer[i]);
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
			if ((int)freshInput == 8) {
				if (currentConsoleLine.size() > 0 && eraseCounter < 0) {
					eraseCounter = 50;
					currentConsoleLine = currentConsoleLine.substr(0, currentConsoleLine.size() - 1);
					updateConsoleView(false);
				}
			}
			//Check return, process console line
			else if (freshInput == SDLK_RETURN) {
				if (currentConsoleLine.size() > 0) {
					processCommand(currentConsoleLine, false);
					updateConsoleView(true);
					currentConsoleLine.clear();
				}
			}
			//Check spaces
			else if (freshInput == SDLK_SPACE) {
				currentConsoleLine.append(" ");
				updateConsoleView(false);
			}
			//This only leaves characters to add
			else {
				currentConsoleLine.append(string(1, freshInput));
				updateConsoleView(false);
			}
			
		}
		//Not in console so input ignored
		else {
			nextIn = (char)0;
		}
	}

	if (inConsole) {
		if (err::queryConsoleMessageBuffer()) {
			for (int i = 0; i < NUMBER_OF_CONSOLE_MESSAGES - 1; i++) {
				consoleBuffer[i].clear();
				consoleBuffer[i].append(consoleBuffer[i + 1]);
			}
			consoleBuffer[NUMBER_OF_CONSOLE_MESSAGES - 1].clear();
			consoleBuffer[NUMBER_OF_CONSOLE_MESSAGES - 1].append(err::pullConsoleMessage());
			updateConsoleView(false);
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

		cycle++;
		err::setLoggingCycle(cycle);

		//Handle console
		consoleCounter--;
		if (getInput().console && consoleCounter < 0) {
			inConsole = !inConsole;
			consoleCounter = 100;
		}
		consoleUpdate();
		eraseCounter--;

		if (!stuckCounter)
			counter++;
		computeCycle();

		if not(cycle % 3)
			pushFPS = true;

		currentCyclePoint = chrono::high_resolution_clock::now();
		cycleLoadDrawCounter += (unsigned int)(chrono::duration_cast<std::chrono::microseconds>(currentCyclePoint - startCyclePoint)).count();

		unsigned int i;
		do {
			currentCyclePoint = chrono::high_resolution_clock::now();
			i = (unsigned int)(chrono::duration_cast<std::chrono::microseconds>(currentCyclePoint - startCyclePoint)).count();
		} while (i < 3333);
		cycleDrawCounter += (int)i;
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
	delete fpsTextMaster;
	delete consoleTextMaster;
	
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

			//draw fps shit before its time to calculate time
			//handle fps counter, every 1000 cycles
			if (graphicsState->getShowFPS()) {
				if (FPSVector.size() >= 20) {
					int temp = 0;
					for (int i : FPSVector) 
						temp += i;

					float average = ((float)temp / FPSVector.size());
					average = F(1e6) / average;
						
					fpsTextMaster->updateTextByKey("fps", "fps: " + to_string((int)average));
					fpsTextMaster->updateTextByKey("dropped", "drop: " + str_kit::convertToScoreString((3333 * cyclesPerDraw * 100) / ((float)cycleDrawCounter), false) + " (" + str_kit::convertToScoreString((cycleLoadDrawCounter * 100) / ((float)3333 * cyclesPerDraw), false) +")");
					fpsTextMaster->updateTextByKey("boxes", "boxes: " + to_string(boxCountCurrent));
					fpsTextMaster->updateTextByKey("particle", "p_master: " + to_string(particleMasterCount) + " (in " + to_string(particleGroupCount) + ") with " + to_string(particleSlaveCount) + " at " + to_string(particleLoad) + "%");
					fpsTextMaster->updateTextByKey("cycle", "cycle: " + to_string(counter) + (stuckCounter ? " STUCK" : " FREE"));
					fpsTextMaster->updateTextByKey("weather", "weather: " + weather2String(weatherToReport) + " | B: " + to_string(numberOfWeatherClips[0]) + " | M: " + to_string(numberOfWeatherClips[1]) + " | A: " + to_string(numberOfWeatherClips[2]));
					cycleDrawCounter = 0;
					cycleLoadDrawCounter = 0;
					cyclesPerDraw = 0;
					FPSVector.clear();
				}
			}
			else {
				fpsTextMaster->updateTextByKey("dropped", " ");
				fpsTextMaster->updateTextByKey("fps", " ");
				fpsTextMaster->updateTextByKey("boxes", " ");
				fpsTextMaster->updateTextByKey("particle", " ");
				fpsTextMaster->updateTextByKey("cycle", " ");
				fpsTextMaster->updateTextByKey("weather", " ");
			}

			fpsTextMaster->renderText(0);

			if (inConsole) {
				consoleTextMaster->renderText(0);
			}

			currentCyclePoint = chrono::high_resolution_clock::now();

			if (graphicsState->getShowFPS()) {
				if (pushFPS) {
					FPSVector.push_back( (int)((chrono::duration_cast<std::chrono::microseconds>(currentCyclePoint - startCyclePoint)).count()) );
					pushFPS = false;
				}
			}
			
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
	if (!rendererActive) {
		delete particleMaster;
	}
}

void SlaveInstance::initialiseInstance(GraphicsState* graphicsState, TextRenderer* textRenderer, LevelSettings* currentSettings, TextGlobalMaster* textGlobalMaster) {
	this->textRenderer = textRenderer;
	this->textGlobalMaster = textGlobalMaster;
	this->graphicsState = graphicsState;
	this->levelSettingsCurrent = currentSettings;
	this->particleMaster = new ParticleManager(graphicsState);

	totalWindSum.store({ 0,0 });

	fpsTextMaster = new TextMaster(textGlobalMaster);
	fpsTextMaster->addTextEnt("dropped", " ", { (float)10, (float)904 }, 28, sansFontStyle, whiteFontColor, topLeft);
	fpsTextMaster->addTextEnt("fps", " ", { (float)10, (float)932 }, 28, sansFontStyle, whiteFontColor, topLeft);
	fpsTextMaster->addTextEnt("boxes", " ", { (float)10, (float)960 }, 28, sansFontStyle, whiteFontColor, topLeft);
	fpsTextMaster->addTextEnt("particle", " ", { (float)10, (float)988 }, 28, sansFontStyle, whiteFontColor, topLeft);
	fpsTextMaster->addTextEnt("cycle", " ", { (float)10, (float)1016 }, 28, sansFontStyle, whiteFontColor, topLeft);
	fpsTextMaster->addTextEnt("weather", " ", { (float)10, (float)1044 }, 28, sansFontStyle, whiteFontColor, topLeft);


	consoleTextMaster = new TextMaster(textGlobalMaster);
	for (int i = 0; i < NUMBER_OF_CONSOLE_MESSAGES; i++) {
		consoleTextMaster->addTextEnt(to_string(i), " ", { (float)1170, (float)5 + 35 * i }, 28, sansFontStyle, redFontColor, topRight);
	}
	consoleTextMaster->addTextEnt("current", ": ", { (float)640, (float)NUMBER_OF_CONSOLE_MESSAGES * 35 + 5 }, 28, sansFontStyle, goldFontColor, topLeft);

}

void SlaveInstance::setCurrentInput(Input newInput) {
	safeInput.store(newInput);
}

