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
		backgroundManager = new BackgroundManager();
		backgroundManager->initialiseBackgroundManager(graphicsState->getGRenderer(), currentSettings);
		backgroundManager->forceBackgroundTable();

		opener = new ButtonManager(textRenderer, gold, sans36);
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
		opener->update( getInput(), currentSettings );
		opener->unlock();

		SharedEnityList<ListedEntity> list;
		ListedEntity* meme = new ListedEntity();

		list.pushObject(meme);
		cout << list.size();
		meme->setFlag(false);
		list.cleanDeathFlags();
		cout << list.size();

		if (currentSettings->buttonFlag == titleToMenu) {
			currentSettings->buttonFlag.store(none);
			currentSettings->level = 1;
			currentSettings->nextStage = inGame;
			currentSettings->endLevel = true;
		}
	}

	void renderCycle() {
		backgroundManager->lock();
		backgroundManager->readyBackgroundTable();

		BackRender temp;
		SDL_Rect tempRect;
		while (backgroundManager->remainingBackgroundTable()) {
			temp = backgroundManager->getABackground();
			tempRect = temp.dest;
			SDL_RenderCopy(graphicsState->getGRenderer(), temp.texture, NULL, &tempRect);
		}
		backgroundManager->unlock();

		opener->render();
	}
};

#endif