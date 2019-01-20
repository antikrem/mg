/*Classess to run menus.
To be used with _controller.cpp*/
#ifndef __MENU_H__
#define __MENU_H__

#include "background.h"
#include "_slave_class.h"
#include "_interactives.h"

void but1(LevelSettings* currentSettings) {
	err::logMessage("but 1 pressed");
	currentSettings->buttonFlag = titleToMenu;
}

void but2(LevelSettings* currentSettings) {
	err::logMessage("but 2 pressed");
	cout << currentSettings->lives << endl;
}

class TitleMenu : public SlaveInstance {
private:
	BackgroundManager* backgroundManager = NULL;
	ButtonManager* opener = NULL;

	void initialise() {
		para::setDistanceFromPlane((float)1000);

		backgroundManager = new BackgroundManager(graphicsState->getGRenderer(), levelSettingsCurrent);

		opener = new ButtonManager(textGlobalMaster, goldFontColor, sansFontStyle, 36 );
		opener->addButton("MEME", { 100, 200, 200, 36 }, but1 );
		opener->addButton("DREAM", { 100, 500, 200, 36 }, but2);
		opener->activate(150);
	}

	void memFree() {
		delete backgroundManager;
		delete opener;
	}

	void computeCycle() {
		opener->lock();
		opener->update( getInput(), levelSettingsCurrent );
		opener->unlock();

		if (levelSettingsCurrent->buttonFlag == titleToMenu) {
			levelSettingsCurrent->buttonFlag.store(none);
			levelSettingsCurrent->level = 1;
			levelSettingsCurrent->nextStage = inGame;
			levelSettingsCurrent->endLevel = true;
		}
	}

	void renderCycle() {
		backgroundManager->lock();
		backgroundManager->drawBackground(0);
		backgroundManager->unlock();

		opener->render();
	}
};

#endif