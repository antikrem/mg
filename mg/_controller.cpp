#include "error.h"
#include "_menu.h"
#include "_level.h"

#include <iostream>
#include <thread>

/*Returns current key pressess based on keyBind Values*/
Input catchInput(KeyBind* keyBind, SDL_Event* evt, char* nextIn) {
	Input currentInputState;
	const Uint8* keystate;

	keystate = SDL_GetKeyboardState(NULL);

	currentInputState.left = keystate[keyBind->leftCode];
	currentInputState.right = keystate[keyBind->rightCode];
	currentInputState.up = keystate[keyBind->upCode];
	currentInputState.down = keystate[keyBind->downCode];
	currentInputState.shoot = keystate[keyBind->shootCode];
	currentInputState.special = keystate[keyBind->specialCode];
	currentInputState.dash = keystate[keyBind->dashCode];
	currentInputState.shift = keystate[keyBind->shiftCode];
	currentInputState.enter = keystate[keyBind->enterCode];
	currentInputState.esc = keystate[keyBind->escCode];
	currentInputState.console = keystate[keyBind->consoleCode];

	//A key has been pressed and the party begins
	if (evt->type == SDL_KEYDOWN) {
		SDL_Keysym key = evt->key.keysym;
		//Input character buffer is clear
		if (*nextIn == (char)0) {
			//Catch type keys and mod with shift
			if ((key.sym >= SDLK_a && key.sym <= SDLK_z) || (key.sym >= SDLK_0 && key.sym <= SDLK_9)) {
				if (key.mod & KMOD_LSHIFT || key.mod & KMOD_RSHIFT) {
					*nextIn = (char)key.sym - 32;
				}
				else {
					*nextIn = (char)key.sym;
				}
			}
			//Take these keys direct, they are special af
			else if (key.sym == SDLK_SPACE || key.sym == SDLK_BACKSPACE || key.sym == SDLK_RETURN) {
				*nextIn = (char)key.sym;
			}

		}
	}


	return currentInputState;
}

int main(int argc, char* args[]) {
	//Prime error log sub system
	err::primeErrorFile();

	//Initialise
	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
	sound_ini();

	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
		err::logMessage("Error: SDL_Image failed to load, check .dlls or reinstall");
	}

	//If true, game is in play, else end
	bool gameState = true;

	GraphicsState* graphicsState = new GraphicsState();
	TextRenderer* textRenderer = new TextRenderer(graphicsState);
	loadStore(graphicsState->getGRenderer());
	
	SlaveInstance* currentState = NULL;
	LevelSettings* levelSettings = new LevelSettings;

	loadSoundAndMusic(NULL);

	//Current key settings
	KeyBind* keyBind = new KeyBind;
	//Update from file
	updateScanCodes(keyBind);

	//Stores last event
	SDL_Event evt;

	while (gameState) {
		//Catch last event, store at evt
		SDL_PollEvent(&evt);

		//If there is no level being played, make one
		if (currentState == NULL) {
			switch (levelSettings->nextStage) {
			case (titleMenu):
				currentState = new TitleMenu; break;
			case (inGame):
				currentState = new Level; break;
			}

			currentState->initialiseInstance(graphicsState, textRenderer, levelSettings);
			levelSettings->currentStage = levelSettings->nextStage;

			thread renderThread(&SlaveInstance::computer, currentState);
			renderThread.detach();

			thread computeThread(&SlaveInstance::renderer, currentState);
			computeThread.detach();
		}

		if (currentState != NULL) {
			currentState->setCurrentInput( catchInput(keyBind, &evt, currentState->getConsoleCharInput()) );
		}

		//If end level flag is true, attempt to end the level
		if (levelSettings->endLevel) {
			levelSettings->endLevel = false;
			if (currentState != NULL) {
				currentState->killThreads();
				while (!currentState->readyToFree()) {
					cout << "waiting" << endl;
				}
				delete currentState;
				currentState = NULL;
			}
		}

		//End level, delete level if one exists
		if ( (evt.type == SDL_QUIT) || (levelSettings->endGame) ) {
			gameState = false;
			levelSettings->endLevel = false;
			if (currentState != NULL) {
				currentState->killThreads();
				while (!currentState->readyToFree()) {
					cout << "waiting" << endl;
				}
				delete currentState;
				currentState = NULL;
			}
		}

	}

	delete keyBind;
	delete levelSettings;

	clearAnimaionStore();
	
	err::endLog();

//	sound_end();
	SDL_Quit();
	TTF_Quit();

	return 0;
}