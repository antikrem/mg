#include "error.h"
#include "_menu.h"
#include "_level.h"

#include <iostream>
#include <thread>

/*Returns current key pressess based on keyBind Values*/
Input catchInput(KeyBind* keyBind) {
	Input current_state;
	const Uint8* keystate;

	keystate = SDL_GetKeyboardState(NULL);

	current_state.left = keystate[keyBind->leftCode];
	current_state.right = keystate[keyBind->rightCode];
	current_state.up = keystate[keyBind->upCode];
	current_state.down = keystate[keyBind->downCode];
	current_state.shoot = keystate[keyBind->shootCode];
	current_state.special = keystate[keyBind->specialCode];
	current_state.dash = keystate[keyBind->dashCode];
	current_state.shift = keystate[keyBind->shiftCode];
	current_state.enter = keystate[keyBind->enterCode];
	current_state.esc = keystate[keyBind->escCode];

	return current_state;
}

int main(int argc, char* args[]) {

	SDL_Init(SDL_INIT_EVERYTHING);
	TTF_Init();
	
	//If true, game is in play, else end
	bool gameState = true;

	GraphicsState* graphicsState = new GraphicsState();
	TextRenderer* textRenderer = new TextRenderer(graphicsState);

	err::primeErrorFile();
	
	SlaveInstance* currentState = NULL;
	LevelSettings* levelSettings = new LevelSettings;

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
				currentState = new TitleMenu; break;
			}

			currentState->initialiseInstance(graphicsState, textRenderer, levelSettings);
			levelSettings->currentStage = levelSettings->nextStage;

			thread renderThread(&SlaveInstance::computer, currentState);
			renderThread.detach();

			thread computeThread(&SlaveInstance::renderer, currentState);
			computeThread.detach();
		}

		if (currentState != NULL) {
			currentState->setCurrentInput( catchInput(keyBind) );
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
	
	err::endLog();

	SDL_Quit();
	TTF_Quit();

	return 0;
}