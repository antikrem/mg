/*Classess to run menus.
To be used with _controller.cpp*/
#ifndef __MENU_H__
#define __MENU_H__

#include "background.h"
#include "_slave_class.h"
#include "_interactives.h"
#include "weather.h"

void but1(LevelSettings* currentSettings) {
	err::logMessage("but 1 pressed");
	currentSettings->buttonFlag = titleToMenu;
}

void but2(LevelSettings* currentSettings) {
	err::logMessage("but 2 pressed");
	cout << currentSettings->lives << endl;
}

//Title menu goes through stages
//First stage is the opener, which is a scroll through level with text shown

enum MenuStages {
	openerMenuStage,
	titleMenuStage
};

class TitleMenu : public SlaveInstance {
private:
	BackgroundManager* backgroundManager = NULL;
	ButtonManager* opener = NULL;

	//Lightmaster control instance
	LightMaster *lightMaster = NULL;

	//Weather Master
	mutex weatherMasterLock;
	WeatherEffectManager* weatherEffectManager = NULL;

	atomic<MenuStages> currentStage = openerMenuStage;

	void initialise() {
		para::setDistanceFromPlane((float)5000);

		backgroundManager = new BackgroundManager(graphicsState->getGRenderer(), levelSettingsCurrent);

		opener = new ButtonManager(textGlobalMaster, goldFontColor, sansFontStyle, 36 );
		opener->addButton("MEME", { 100, 200, 200, 36 }, but1 );
		opener->addButton("DREAM", { 100, 500, 200, 36 }, but2);
		opener->activate(150);

		//Create Light master
		lightMaster = new LightMaster(graphicsState);

		//Create weather effects manager
		weatherEffectManager = new WeatherEffectManager(graphicsState, noweather);
		weatherEffectManager->startRain();

		float windspeed = 0;
	}

	void memFree() {
		delete backgroundManager;
		delete opener;
	}

	void computeCycle() {

		//Update weather
		weatherMasterLock.lock();
		CUS_Point wind = { 0.0f, 5.0f };
		weatherEffectManager->update(toPolar(wind), lightMaster, 0);
		for (int i = 0; i < 3; i++)
			numberOfWeatherClips[i] = weatherEffectManager->getEffectsCountByIndex(i);
		weatherToReport = weatherEffectManager->getWeatherType();
		weatherMasterLock.unlock();

		if (currentStage == openerMenuStage) {
			backgroundManager->updateBackground(0.1f);
			if (counter == 200) {
				weatherEffectManager->stopWeather();
				currentStage = titleMenuStage;
			}
		}


		if (currentStage == titleMenuStage) {
			opener->lock();
			opener->update(getInput(), levelSettingsCurrent);
			opener->unlock();

			if (levelSettingsCurrent->buttonFlag == titleToMenu) {
				levelSettingsCurrent->buttonFlag.store(none);
				levelSettingsCurrent->level = 1;
				levelSettingsCurrent->nextStage = inGame;
				levelSettingsCurrent->endLevel = true;
			}
		}

		
	}

	void renderCycle() {
		backgroundManager->lock();
		backgroundManager->drawBackground(0);
		backgroundManager->unlock();

		
		lightMaster->lock();
		lightMaster->drawLightLevel(0);
		lightMaster->unlock();

		weatherMasterLock.lock();
		weatherEffectManager->drawBackWeather(lightMaster, 0, lightMaster->getObjectRenderBrightness(0));
		weatherMasterLock.unlock();

		lightMaster->lock();
		lightMaster->drawLightLevel(1);
		lightMaster->unlock();

		weatherMasterLock.lock();
		weatherEffectManager->drawMiddleWeather(lightMaster, 0, lightMaster->getObjectRenderBrightness(1));
		weatherMasterLock.unlock();

		lightMaster->lock();
		lightMaster->drawLightLevel(2);
		lightMaster->unlock();

		weatherMasterLock.lock();
		weatherEffectManager->drawAboveWeather(lightMaster, 0, lightMaster->getObjectRenderBrightness(2));
		weatherMasterLock.unlock();

		lightMaster->lock();
		lightMaster->drawLightLevel(3);
		lightMaster->unlock();

		

		if (currentStage == titleMenuStage) {
			opener->render();
		}
	}
};

#endif