/*Level Class.
To be used with _controller.cpp*/
#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "_interactives.h"
#include "_slave_class.h"

#include "light_master.h"

#include "weather.h"

#include "entity_player.h"
#include "enemy_entity.h"

#include "background.h"
#include "background_objects.h"

#include "text_dialogue.h"

#include "sound.h"

#include <locale> 

#define SHIFTVELOCITY 4.5F


class Level : public SlaveInstance {
private:
	//pause only when equal to zero
	int pauseCounter = 0;

	//Used to avoid unsticking counter with pauses
	bool alreadyPause = false;

	//Playermovement on plane, scaled for lower levels
	float planeSpeed = 0.5F ;
	float planeAcceleration = 0;
	int planeAccelerationCycles = 0;

	//Speed of wind, used for particles and weather
	CUS_Polar baseWindSpeed = { 0.1f, 90 };
	CUS_Polar burstWindSpeed = { 0, 0 };
	CUS_Polar windSpeed = { 0, 0 };
	//list of possible bursts
	vector<CUS_Polar> windBursts;
	//Points to current burst if not equal to -1
	int currentBurst = -1;
	/*If no currentBurst and value is zero, sets up burst random chance possibility, else value is a cooldown
	If there is a current burst, value is remaining time, a value of zero means its time to stop the burst*/
	int burstDuration = 0;
	//likelihood of a burst occuring
	float burstChance = F(0.0005);
	//speed that a normalisation occurs at
	float burstNormalisation = 0;
	//shift velocity is an average of 5 cycles of shift
	float currentShiftVelocity = 0;
	float cumulativeShift = 0;

	//Shift variables
	float trueShift = 0;
	atomic<int> shift = 0;

	//Lightmaster control instance
	LightMaster *lightMaster = NULL;

	//BackgroundManager for this slave instance
	BackgroundManager* backgroundManager = NULL;

	//Dialogue manager
	Dialogue* dialogue = NULL;

	//Background object generator for this slave instance
	mutex backgroundObjLock;
	BackgroundObjectManager* backgroundObjectManager = NULL;

	//Player ent will be assigned based on current settings;
	PlayerEntity* player = NULL;
	//Used for calculating shift of non-center points
	CUS_Point playerTralingPosition;

	//Player Bullet list
	SharedEntityList<PlayerBullet> playerBullets;

	//Weather Master
	mutex weatherMasterLock;
	WeatherEffectManager* weatherEffectManager = NULL;

	//AnonEnts list
	SharedEntityList<AnonEnt> anonEnts;

	//BMT pool
	map<string, BulletMasterTemplate*> BulletMasterTemplatePool;

	//EnemyEntity list
	vector<EnemyEntity*> upcomingEnemyList;
	SharedEntityList<EnemyEntity> enemyEntities;

	//Total bullet list
	SharedEntityList<Bullet> totalBulletList;

	//Powerup list
	SharedEntityList<PowerUp> powerUps;

	//Text container for floating text
	TextMaster* floatTextMaster;

	//Text container for UI elements 
	TextMaster* uiTextMaster;

	//Overlay Texture
	SDL_Texture* overlay = NULL;

	//Circle texture for hitbox rendering
	SDL_Texture* hitBoxSizeTexture = NULL;
	//If true render hitbox
	atomic<bool> hitBoxSizeRender = false;

private:
	//Draws hitbox for individual listed entity
	void drawHitBox(ListedEntity* ent) {
		if (hitBoxSizeRender) {
			SDL_Rect hitRect = { (int)(ent->getPosition().x - ent->getHitBox() + 220 + shift), (int)(ent->getPosition().y - ent->getHitBox()), 2*(int)(ent->getHitBox()), 2*(int)(ent->getHitBox()) };
			SDL_RenderCopy(graphicsState->getGRenderer(), hitBoxSizeTexture, NULL, &hitRect);
		}
	}

	void levelProcessCommand(string command, bool fromMaster) {
		auto lineVec = str_kit::splitOnToken(command, ' ');

		if (lineVec[0] == "PLANE") {
			if (lineVec.size() == 2) {
				if (lineVec[1] == "INFO") {
					err::logConsoleMessage("PLANE SPEED current: " + to_string(planeSpeed));
					err::logConsoleMessage("PLANE ACCLERATION current: " + to_string(planeAcceleration) + " for cycles: " + to_string(planeAccelerationCycles));
				}
			}
			if (lineVec.size() == 3) {
				if (lineVec[1] == "SPEED") {
					planeSpeed = stof(lineVec[2]);
					err::logConsoleMessage("PLANE SPEED new: " + to_string(planeSpeed));
				}
			}
			else if (lineVec.size() == 4) {
				if (lineVec[1] == "ACCELERATION") {
					planeAcceleration = stof(lineVec[2]);
					planeAccelerationCycles = stoi(lineVec[3]);
					err::logConsoleMessage("PLANE ACCLERATION new: " + to_string(planeAcceleration) + " for cycles: " + to_string(planeAccelerationCycles));
				}
			}
		}
		else if (lineVec[0] == "WIND") {
			if (lineVec.size() == 2) {
				if (lineVec[1] == "CLEAR") {
					int size = windBursts.size();
					windBursts.clear();
					err::logConsoleMessage("WIND bursts was clear, number cleared: " + to_string(size));
				}
				else if (lineVec[1] == "SIZE") {
					err::logConsoleMessage("Number of WIND bursts is: " + to_string(windBursts.size()));
				}
				else {
					if (!fromMaster)
						err::logConsoleMessage("WIND called with 1 invalid parameter: " + lineVec[1]);
					else
						err::logMessage("WIND called by master with 1 invalid parameter: " + lineVec[1]);

				}
				
			}
			else if (lineVec.size() == 4) {
				if (lineVec[1] == "BURST") {
					try {
						windBursts.push_back({ stof(lineVec[2]), stof(lineVec[3]) });
						if (!fromMaster)
							err::logConsoleMessage("Current windburst count is: " + to_string(windBursts.size()));
					}
					catch (const std::exception & ex) {
						ex.what();
						if (fromMaster)
							err::logMessage("WIND BURST was called by master as " + command + " which has an invalid parameter at line " + to_string(counter));
						else
							err::logConsoleMessage("Requested value was not two numbers");
					}
				}
				else if (lineVec[1] == "BASE") {
					try {
						baseWindSpeed = { stof(lineVec[2]), stof(lineVec[3]) };
						if (!fromMaster)
							err::logConsoleMessage("Current base winspeed count is: " + to_string(baseWindSpeed.magnitude) + " in direction: " + to_string(baseWindSpeed.angle));
					}
					catch (const std::exception & ex) {
						ex.what();
						if (fromMaster)
							err::logMessage("WIND BASE was called by master as " + command + " which has an invalid parameter at line " + to_string(counter));
						else
							err::logConsoleMessage("Requested value was not two numbers");
					}
				}
				
			}
			else {
				if (!fromMaster)
					err::logConsoleMessage("Expected command either 1 or 2 parameters, got: " + to_string(lineVec.size() - 1));
				else
					err::logMessage("WIND was called by master on cycle " + to_string(counter) + " with invalid number of parameters " + to_string(lineVec.size() - 1) + " expected 1/2 parameter");
			}
		}
		else if (lineVec[0] == "HITBOX") {
			if not(hitBoxSizeTexture) {
				SDL_Surface* hitBoxSurf = IMG_Load("assets//UI//hitbox.png");
				hitBoxSizeTexture = SDL_CreateTextureFromSurface(graphicsState->getGRenderer(), hitBoxSurf);
				if (!hitBoxSizeTexture) {
					err::logMessage("Hitbox file corrupted or missing, check assets//UI//hitbox.png, or reinstall");
				}
				SDL_FreeSurface(hitBoxSurf);
			}
			hitBoxSizeRender = !hitBoxSizeRender;
		}
		else if (lineVec[0] == "GETPLAYERPOSITION") {
			err::logConsoleMessage("Player at: " + to_string(player->getPosition().x) + " " + to_string(player->getPosition().y));
		}
		else if (lineVec[0] == "WEATHER") {
			if (lineVec.size() != 2) {
				if (!fromMaster)
					err::logConsoleMessage("Expected command + 1 parameter, got: " + to_string(lineVec.size() - 1));
				else
					err::logMessage("WEATHER was called by master on cycle " + to_string(counter) + " with invalid number of lines " + to_string(lineVec.size() - 1) + " expected 1 parameter");
			}
			else if (lineVec[1] == "STOP") {
				if (weatherEffectManager) {
					weatherEffectManager->stopWeather();
					if (fromMaster)
						err::logConsoleMessage("Master has stopped the weather");
					else
						err::logConsoleMessage("Weather has been stopped");
				}
			}
			else if (lineVec[1] == "RAIN") {
				if (weatherEffectManager) {
					weatherEffectManager->startRain();
					if (fromMaster)
						err::logConsoleMessage("Master has started rain");
					else
						err::logConsoleMessage("Rain sequence has started");
				}
			}
			else {
				if (fromMaster)
					err::logConsoleMessage("Invalid parameter for STOP at cycle: " + to_string(counter));
				else
					err::logConsoleMessage("Invalid parameter for STOP");
			}
		}
		else if (lineVec[0] == "LIGHTING") {
			if (lineVec.size() != 2) {
				if (!fromMaster)
					err::logConsoleMessage("Expected command + 1 parameter, got: " + to_string(lineVec.size() - 1));
				else
					err::logMessage("LIGHTING was called by master on cycle " + to_string(counter) + " with invalid number of lines " + to_string(lineVec.size() - 1) + " expected 1 parameter");
			}
			else if (lineVec[1] == "LIGHTNING") {
				if (lightMaster) {
					lightMaster->lightningStrike();
					if (fromMaster)
						err::logConsoleMessage("Master dropped Lightning");
					else
						err::logConsoleMessage("Lightning spawned correctly");
				}
			}
			else {
				if (fromMaster)
					err::logConsoleMessage("Invalid parameter for LIGHTING at cycle: " + to_string(counter));
				else
					err::logConsoleMessage("Invalid parameter for LIGHTING");
			}
		}
		else {
			if (fromMaster)
				err::logMessage("INVALID COMMAND: [" + command + "] failed to parse in master: " + to_string(counter));
			err::logConsoleMessage("Command: [" + command + "] was invalid");
		}
	}

	void initialise() {
		//Load local store
		loadStoreLocal(graphicsState->getGRenderer(), levelSettingsCurrent, false);

		dialogue = new Dialogue(textRenderer, levelSettingsCurrent);

		lightMaster = new LightMaster(graphicsState);

		//Create background
		backgroundManager = new BackgroundManager(graphicsState->getGRenderer(), levelSettingsCurrent);

		//Create background object manager
		backgroundObjectManager = new BackgroundObjectManager(levelSettingsCurrent);

		//Create weather effects manager
		weatherEffectManager = new WeatherEffectManager(graphicsState, noweather);

		//Create Player Ent
		if (levelSettingsCurrent->currentCharacter == 0) 
			player = new Sorceress();
		particleMaster->addForceApplier(player->particulate(true) );

		//Initialise float text container
		floatTextMaster = new TextMaster(textGlobalMaster);
		uiTextMaster = new TextMaster(textGlobalMaster);
		uiTextMaster->addTextEnt("power", "00.00%", { 1409, 595 }, 56, sansFontStyle, whiteFontColor, midMid, 0, true);
		uiTextMaster->addTextEnt("rage", "00.00%", { 1699, 595 }, 56, sansFontStyle, whiteFontColor, midMid, 0, true);

		//Load overlay
		SDL_Surface* overlaySurf = IMG_Load("assets//UI//overlay.png");
		overlay = SDL_CreateTextureFromSurface(graphicsState->getGRenderer(), overlaySurf);
		if (!overlay) {
			err::logMessage("Overlay file corrupted or missing, check assets//UI//overlay.bmp, or reinstall");
		}
		SDL_FreeSurface(overlaySurf);

		//Load bullets
		pullBMT(&BulletMasterTemplatePool, levelSettingsCurrent);

		//Load enemies
		vector<EnemyEntity*> tempEnemyList;
		pullEnemies(&tempEnemyList, levelSettingsCurrent);
		for (auto enemy : tempEnemyList)
			upcomingEnemyList.push_back(enemy);

		particleMaster->spawnParticle({ 500,500 }, { 0,0 }, particle_gold);
		particleMaster->spawnParticle({ 600,400 }, { 0,0 }, particle_gold);
		particleMaster->spawnParticle({ 700,300 }, { 0,0 }, particle_gold);
		particleMaster->spawnParticle({ 800,200 }, { 0,0 }, particle_gold);
		particleMaster->spawnParticle({ 900,100 }, { 0,0 }, particle_gold);

		//load music
		loadSoundAndMusic(levelSettingsCurrent);

		//Load command list
		loadCommandList();

		err::logLevelStart(levelSettingsCurrent);
	}

	void memFree() {
		stopMusic(0);
		stopAllSoundChannels();
		freeLocalSoundAndMusic();

		delete backgroundManager;
		backgroundManager = NULL;

		delete player;
		player = NULL;

		delete backgroundObjectManager;
		backgroundObjectManager = NULL;

		delete weatherEffectManager;
		weatherEffectManager = NULL;

		delete dialogue;
		dialogue = NULL;

		clearLocalAnimaionStore();
		SDL_DestroyTexture(overlay);

		if (hitBoxSizeTexture)
			SDL_DestroyTexture(hitBoxSizeTexture);
	}

	void computeCycle() {
		//Check command list
		computeFromCommandList();

		//Early declarations
		CUS_Point windToStore;
		enemyEntities.lock();
		auto it = upcomingEnemyList.begin();
		enemyEntities.unlock();
		vector<shared_ptr<Bullet>> pullBack;

		if (pauseCounter > 0)
			pauseCounter--;
		if (levelSettingsCurrent->buttonFlag == pause)
			goto PAUSELABEL;
		
		//Shift update
		if (getInput().shift) {
			pass;
		}
		else if ( trueShift + SHIFTVELOCITY < para::getShift(player->getPosition()) ) {
			trueShift += SHIFTVELOCITY;
			cumulativeShift += SHIFTVELOCITY;
		}
		else if ( trueShift - SHIFTVELOCITY > para::getShift(player->getPosition()) ) {
			trueShift -= SHIFTVELOCITY;
			cumulativeShift -= SHIFTVELOCITY;
		}
		shift = (int)trueShift;

		//Calculate windchange from shift velocity
		if (cycle % 5 == 0) {
			currentShiftVelocity = cumulativeShift / 5;
			cumulativeShift = 0;
		}

		//Manage windspeed
		if (planeAccelerationCycles > 0) {
			planeAccelerationCycles--;
			planeSpeed += planeAcceleration;
		}
		windSpeed = baseWindSpeed + burstWindSpeed;
		windToStore = toPoint(windSpeed);
		windToStore.y += planeSpeed;
		if (totalWindSumForceStill) {
			totalWindSum.store({ 0,0 });
			windSpeed = { 0,0 };
		}
		else {
			totalWindSum.store(windToStore);
		}
		
		burstDuration--;
		//If there is no burst,
		if (currentBurst < 0) {
			//Normalise windspeed (start moving towards 0)
			if (burstWindSpeed.magnitude != 0) {
				burstWindSpeed.magnitude -= burstNormalisation;
				//If windspeed is low enough, just set it to zero
				if (burstWindSpeed.magnitude < 0.1) {
					burstWindSpeed.magnitude = 0;
				}
			}
			//If the burst cooldown is over
			if (burstDuration < 0) {
				if (burstChance >= random::randomFloat() && windBursts.size()) {
					currentBurst = random::randomInt(0, windBursts.size() - 1);
					burstNormalisation = 0.01f * windBursts[currentBurst].magnitude;
					burstDuration = 300;
				}
			}
			
		}
		//If there is a current burst
		if (currentBurst >= 0) {
			burstWindSpeed.angle = windBursts[currentBurst].angle;
			if (burstWindSpeed.magnitude < windBursts[currentBurst].magnitude) {
				burstWindSpeed.magnitude += 0.1f * windBursts[currentBurst].magnitude;
			}
			if (burstDuration < 0) {
				currentBurst = -1;
				burstDuration = (int) (random::randomFloat(0.7f, 1.4f) * 3000);
			}
		}

		//Update Lighting Engine
		lightMaster->lock();
		lightMaster->update();
		lightMaster->unlock();
		
		//Update Background manager
		backgroundManager->lock();
		backgroundManager->updateBackground(planeSpeed);
		backgroundManager->unlock();

		//Update Background Object Manager
		backgroundObjectManager->lock();
		backgroundObjectManager->update(planeSpeed, counter);
		backgroundObjectManager->unlock();

		//Update weather
		weatherMasterLock.lock();
		weatherEffectManager->update(windSpeed, lightMaster, currentShiftVelocity);
		for (int i = 0; i < 3; i++)
			numberOfWeatherClips[i] = weatherEffectManager->getEffectsCountByIndex(i);
		weatherToReport = weatherEffectManager->getWeatherType();
		weatherMasterLock.unlock();

		//update player
		player->lock();
		player->playerUpdate(getInput(), shift);
		if (player->getRageResetFlag()) {
			floatTextMaster->addTextEnt("", "RAGE RESET", player->getPosition(), 22, sansFontStyle, whiteFontColor, midMid, 0,
				true, { (float) 2.1, (float)180 }, { (float) 0.012, (float)0 }, 400, 255, -(float)1.108);
			powerUps.lock();
			for (auto i : powerUps.getEntList()) {
				if (i->getUpgradeType() == life) {
					i->setOnRails();
				}
			}
			powerUps.unlock();
		}
		player->unlock();

		//Pull enemies
		it = upcomingEnemyList.begin();
		while (it != upcomingEnemyList.end()) {
			if ((*it)->getSpawningCycle() == counter) {
				enemyEntities.pushObject((*it));
				particleMaster->addForceApplier((*it)->particulate(true));
				it = upcomingEnemyList.erase(it);
			}
			else {
				it++;
			}
		}
		
		//Pull bullets from player
		player->lock();
		while (player->tableResidual()) {
			playerBullets.pushObject( player->getBullet(player->tableResidualPosition()) );
		}
		player->unlock();

		//Pull anon ents from player
		player->lock();
		while (player->toAnonEntsToGet()) {
			anonEnts.pushObject(player->getAnonEnt());
		}
		player->unlock();

		//update enemies
		enemyEntities.lock();
		for (auto i : enemyEntities.getEntList()) {
			pullBack = i->update(player->getPosition(), &BulletMasterTemplatePool);
			//If bullets were pulled, chuck it on the master list
			if (pullBack.size()) {
				for (auto j : pullBack) {
					totalBulletList.pushObject(j);
				}
			}
		}
		enemyEntities.unlock();
		enemyEntities.cleanDeathFlags();
		
		totalBulletList.lock();
		for (auto i : totalBulletList.getEntList()) {
			auto check = i->update(player->getPosition());
			//Do a quick quick check first for collision
			if (quickCheckTwoPointsDistance(i->getPosition(), player->getPosition(), i->getHitBox() + player->getHitBox())) {
				if (i->getPosition().getDistanceToPoint(player->getPosition()) < (i->getHitBox() + player->getHitBox())) {
					player->registerHit();
				}
			}
			
		}
		totalBulletList.unlock();
		totalBulletList.pushToRenderBuffer(shift);

		//update power ups
		powerUps.lock();
		for (auto i : powerUps.getEntList()) {
			if ( i->updatePosition(player->getPosition()) ) {
				CUS_Point tempPowerPos = i->getPosition();
				CUS_Point tempRandPowerPos = { 70 * random::randomFloat() * random::sign(), 70 * random::randomFloat() * random::sign() };
				floatTextMaster->addTextEnt("", to_string(i->getPoints()), tempPowerPos + tempRandPowerPos, 22, sansFontStyle, whiteFontColor, midMid, 0,
					true, { (float) 2.1, (float)180 }, { (float) 0.012, (float)0 }, 400, 255, -(float)1.108);
			}
		}
		powerUps.unlock();
		powerUps.cleanDeathFlags();
		powerUps.pushToRenderBuffer(shift);

		//update player bullet list
		playerBullets.lock();
		for (auto i : playerBullets.getEntList()) {
			i->update();
		}
		playerBullets.unlock();
		playerBullets.cleanDeathFlags();
		playerBullets.pushToRenderBuffer(shift);

		//Compute interactions between enemies and player bullets
		playerBullets.lock();
		enemyEntities.lock();
		for (auto i : enemyEntities.getEntList()) {
			for (auto j : playerBullets.getEntList()) {
				if (i->getPosition().getDistanceToPoint(j->getPosition()) <	(i->getHitBox() + j->getHitBox()) ) {
					i->takeDamage(j->getDamage());
					j->setFlag(false);
					
					//If this bullet kills
					if (i->getCurrentHealth() <= 0) {
						PowerUpTable temporaryTable = i->getPowerUpTable();
						temporaryTable.life++;
						while (temporaryTable.life-- > 0) {
							powerUps.pushObject(new PowerUp(life, i->getPosition() + toPoint({ random::randomFloat(0, 10), random::randomFloat(0, 359) }), 10));
						}
						while (temporaryTable.points--> 0) {
							powerUps.pushObject(new PowerUp(life, i->getPosition() + toPoint({ random::randomFloat(0, 10), random::randomFloat(0, 359) }), 10));
						}
						while (temporaryTable.power--> 0) {
							powerUps.pushObject(new PowerUp(life, i->getPosition() + toPoint({ random::randomFloat(0, 10), random::randomFloat(0, 359) }), 10));
						}
						while (temporaryTable.spell--> 0) {
							powerUps.pushObject(new PowerUp(life, i->getPosition() + toPoint({ random::randomFloat(0, 10), random::randomFloat(0, 359) }), 10));
						}
					}
					
				}
			}
		}
		playerBullets.unlock();
		enemyEntities.unlock();

		//Update dialogues
		stuckCounter = dialogue->updateDialogue(counter, getInput());

		//update float text
		floatTextMaster->updateAllTextEnts();

		//update ui text
		uiTextMaster->updateTextByKey("power", str_kit::convertToScoreString(player->getPower(), false));
		uiTextMaster->updateTextByKey("rage", str_kit::convertToScoreString(player->getRage(), false));

		//Clear dead anonEnts from anonEnt master list
		anonEnts.cleanDeathFlags();

		//Clear dead bullets from master bullet list
		totalBulletList.cleanDeathFlags();

		PAUSELABEL:
		//Pause game on escape press
		if (getInput().enter && levelSettingsCurrent->buttonFlag == pause) {
			levelSettingsCurrent->buttonFlag.store(none);
			levelSettingsCurrent->level = 0;
			levelSettingsCurrent->nextStage = titleMenu;
			levelSettingsCurrent->endLevel = true;
		}
		else if (getInput().esc && !pauseCounter) {
			pauseCounter = 130;
			if (levelSettingsCurrent->buttonFlag == pause) {
				if (!alreadyPause)
					stuckCounter = false;
				levelSettingsCurrent->buttonFlag = none;
			}
			else if (levelSettingsCurrent->buttonFlag == none) {
				if (stuckCounter) 
					alreadyPause = true;
				else
					stuckCounter = true;
				levelSettingsCurrent->buttonFlag = pause;
			}
		}
	}

	void renderCycle() {
		//Draw background
		backgroundManager->lock();
		backgroundManager->drawBackground(shift);
		backgroundManager->unlock();

		lightMaster->lock();
		lightMaster->drawLightLevel(0);
		lightMaster->unlock();

		//Draw weather back
		weatherMasterLock.lock();
		weatherEffectManager->drawBackWeather(lightMaster, shift, lightMaster->getObjectRenderBrightness(0));
		weatherMasterLock.unlock();

		//Draw background objects under player
		backgroundObjectManager->lock();
		for (auto i : backgroundObjectManager->getSharedList()->getEntList()) {
			if (i->getDepth() > 0) {
				auto temp = i->renderCopy(shift);
				temp.renderSize.x += (int)(player->getPosition().x - i->getPosition().x);
				boxCountRunningTotal += drawBoxEntity(temp, para::getShift( player->getPosition(), i->getPosition(), i->getDepth() ), FULLNORMAL,
					para::getSizeScaler( i->getDepth() ) );
			} 
		}
		backgroundObjectManager->unlock();

		//Draw anon ents under player bullets
		anonEnts.lock();
		for (auto i : anonEnts.getEntList()) {
			if (!i->getAbovePlayerBullets()) {
				boxCountRunningTotal += drawBoxEntity(i->renderCopy(shift), shift, lightMaster->getObjectRenderBrightness(0), FULLSIZE);
			}
		}
		anonEnts.unlock();

		lightMaster->lock();
		lightMaster->drawLightLevel(1);
		lightMaster->unlock();

		//Draw weather middle
		weatherMasterLock.lock();
		weatherEffectManager->drawMiddleWeather(lightMaster, shift, lightMaster->getObjectRenderBrightness(1));
		weatherMasterLock.unlock();

		//Draw player bullets
		boxCountRunningTotal += playerBullets.render(graphicsState, shift, lightMaster->getObjectRenderBrightness(1), FULLSIZE);

		//Draw powerups
		boxCountRunningTotal += powerUps.render(graphicsState, shift, lightMaster->getObjectRenderBrightness(1), FULLSIZE);

		//Draw anon ents above player bullets
		anonEnts.lock();
		for (auto i : anonEnts.getEntList()) {
			if (i->getAbovePlayerBullets()) {
				boxCountRunningTotal += drawBoxEntity(i->renderCopy(shift), shift, lightMaster->getObjectRenderBrightness(1), FULLSIZE);
			}
		}
		anonEnts.unlock();

		//Draw player
		player->lock();
		if (player->getTargetable()) {
			boxCountRunningTotal += drawBoxEntity(player->renderCopy(shift), shift, lightMaster->getObjectRenderBrightness(1), FULLSIZE);
		}
		else if ((cycle %10) <5 ) {
			boxCountRunningTotal += drawBoxEntity(player->renderCopy(shift), shift, lightMaster->getObjectRenderBrightness(1), FULLSIZE);
		}
		
		player->unlock();

		//Draw enemy bullets
		boxCountRunningTotal += totalBulletList.render(graphicsState, shift, lightMaster->getObjectRenderBrightness(1), FULLSIZE);
		
		//Draw Enemies
		enemyEntities.lock();
		for (auto i : enemyEntities.getEntList()) {
			boxCountRunningTotal += drawBoxEntity(i->renderCopy(shift), shift, lightMaster->getObjectRenderBrightness(1), FULLSIZE);
		}
		enemyEntities.unlock();

		lightMaster->lock();
		lightMaster->drawLightLevel(2);
		lightMaster->unlock();

		//Draw particles
		particleMaster->renderParticles(shift);

		//Draw float text
		floatTextMaster->renderText(shift + 220);

		//Draw hitboxes
		drawHitBox(player);
		playerBullets.renderHitBoxes(hitBoxSizeRender, graphicsState, hitBoxSizeTexture, shift);
		totalBulletList.renderHitBoxes(hitBoxSizeRender, graphicsState, hitBoxSizeTexture, shift);
		enemyEntities.renderHitBoxes(hitBoxSizeRender, graphicsState, hitBoxSizeTexture, shift);

		//Draw weather above
		weatherMasterLock.lock();
		weatherEffectManager->drawAboveWeather(lightMaster, shift, lightMaster->getObjectRenderBrightness(2));
		weatherMasterLock.unlock();

		lightMaster->lock();
		lightMaster->drawLightLevel(3);
		lightMaster->unlock();

		//Draw overlay
		SDL_RenderCopy(graphicsState->getGRenderer(), overlay, NULL, NULL);

		//draw ui text
		uiTextMaster->renderText(0);

		//Draw dialogue
		dialogue->drawDialogue();
	}
};

#endif