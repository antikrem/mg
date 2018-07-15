/*Level Class.
To be used with _controller.cpp*/
#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "_interactives.h"
#include "_slave_class.h"

class Level : public SlaveInstance {
private:
	/*Base counter, iterates on non-still cycles*/
	int counter = 0;

	BackgroundManager* backgroundManager = NULL;
	ButtonManager* opener = NULL;

	void initialise() {
		//Prep background manager
		backgroundManager = new BackgroundManager();
		backgroundManager->initialiseBackgroundManager(graphicsState->getGRenderer(), currentSettings);
		backgroundManager->forceBackgroundTable();
	}

	void memFree() {
		delete backgroundManager;
	}

	void computeCycle() {
	}

	void renderCycle() {
	}
};

#endif