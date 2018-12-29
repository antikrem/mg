/*Level Class.
To be used with _controller.cpp*/
#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "_interactives.h"
#include "_slave_class.h"

#include "weather.h"

#include "entity_player.h"
#include "enemy_entity.h"

#include "background.h"
#include "background_objects.h"

#include "text_dialogue.h"

#include <locale> 

#define SHIFTVELOCITY 4.5F

 int getShift(CUS_Point pos) {
	return (int)(-1 * ((float)(pos.x) / (float)WORK_SPACE_X)*(WORK_SPACE_X - RENDERED_X));
}

class Level : public SlaveInstance {
private:
	//pause only when equal to zero
	int pauseCounter = 0;

	//Playermovement on plane, scaled for lower levels
	float planeSpeed = 0.5F ;

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

	//Shift variables
	float trueShift = 0;
	atomic<int> shift = 0;

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

	//Powerup list
	SharedEntityList<PowerUp> powerUps;

	//Text container for floating text
	mutex floatTextLock;
	TextContainer* floatText;

	//Overlay Texture
	SDL_Texture* overlay;

private:
	void processCommand(string command) {
		auto lineVec = str_kit::splitOnToken(command, ' ');
		if (lineVec[0] == "FOV") {
			para::setFOV(stof(lineVec[1]));
		}
		else if (lineVec[0] == "DFP") {
			para::setDistanceFromPlane(stof(lineVec[1]));
		}
		else if (lineVec[0] == "WIND") {
			windBursts.push_back({ stof(lineVec[1]), stof(lineVec[2]) });;
		}
		else if (lineVec[0] == "WEATHER") {
			if (lineVec[1] == "STOP") {
				if (weatherEffectManager) {
					weatherEffectManager->stopWeather();
				}
			}
		}
	}

	void initialise() {
		//Load command list
		loadCommandList();

		//Load local store
		loadStoreLocal(graphicsState->getGRenderer(), levelSettingsCurrent, false);

		dialogue = new Dialogue(textRenderer, levelSettingsCurrent);

		//Create background
		backgroundManager = new BackgroundManager(graphicsState->getGRenderer(), levelSettingsCurrent);

		//Create background object manager
		backgroundObjectManager = new BackgroundObjectManager(levelSettingsCurrent);

		//Create weather effects manager
		weatherEffectManager = new WeatherEffectManager(graphicsState, rain);

		//Create Player Ent
		if (levelSettingsCurrent->currentCharacter == 0) 
			player = new Sorceress();
		particleMaster->addForceApplier(player->particulate(true) );

		//Initialise float text container
		floatText = new TextContainer(textRenderer);

		//Load overlay
		SDL_Surface* overlaySurf = SDL_LoadBMP("assets//UI//overlay.bmp");
		overlay = SDL_CreateTextureFromSurface(graphicsState->getGRenderer(), overlaySurf);
		if (!overlay) {
			err::logMessage("Overlay file corrupted or missing, check assets//UI//overlay.bmp, or reinstall");
		}
		SDL_FreeSurface(overlaySurf);

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
	}

	void memFree() {
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
	}

	void computeCycle() {
		//Check command list
		computeFromCommandList();

		CUS_Point windToStore;
		enemyEntities.lock();
		auto it = upcomingEnemyList.begin();
		enemyEntities.unlock();

		if (pauseCounter > 0)
			pauseCounter--;
		if (levelSettingsCurrent->buttonFlag == pause)
			goto PAUSELABEL;
		
		//Shift update
		if (getInput().shift) {
			pass;
		}
		else if ( trueShift + SHIFTVELOCITY < getShift(player->getPosition()) ) {
			trueShift += SHIFTVELOCITY;
		}
		else if ( trueShift - SHIFTVELOCITY > getShift(player->getPosition()) ) {
			trueShift -= SHIFTVELOCITY;
		}
		shift = (int)trueShift;

		//Manage windspeed
		windSpeed = baseWindSpeed + burstWindSpeed;
		windToStore = toPoint(windSpeed);
		windToStore.y += planeSpeed;
		totalWindSum.store(windToStore);
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
		weatherEffectManager->update(planeSpeed, windSpeed);
		for (int i = 0; i < 3; i++)
			numberOfWeatherClips[i] = weatherEffectManager->getEffectsCountByIndex(i);
		weatherToReport = weatherEffectManager->getWeatherType();
		weatherMasterLock.unlock();

		//update player
		player->lock();
		player->playerUpdate(getInput(), shift);
		player->unlock();

		//Pull enemies
		enemyEntities.lock();
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
		enemyEntities.unlock();
		
		//Pull bullets from player
		player->lock();
		playerBullets.lock();
		while (player->tableResidual()) {
			playerBullets.pushObject( player->getBullet(player->tableResidualPosition()) );
		}
		playerBullets.unlock();
		player->unlock();

		//Pull anon ents from player
		player->lock();
		anonEnts.lock();
		while (player->toPushResidual()) {
			anonEnts.pushObject(player->getAnonEnt());
		}
		anonEnts.unlock();
		player->unlock();

		//update enemies
		enemyEntities.lock();
		for (auto i : enemyEntities.getEntList()) {
			i->update(player->getPosition());
		}
		enemyEntities.cleanDeathFlags();
		enemyEntities.unlock();

		//update power ups
		powerUps.lock();
		for (auto i : powerUps.getEntList()) {
			;
			if ( i->updatePosition(player->getPosition()) ) {
				CUS_Point tempPowerPos = i->getPosition();
				CUS_Point tempRandPowerPos = { 70 * random::randomFloat() * random::sign(), 70 * random::randomFloat() * random::sign() };
				CUS_Point shiftPowerup = { 220, 10 };
				floatText->newTextEnt(random::randomString(), to_string(i->getPoints()), tempPowerPos + tempRandPowerPos + shiftPowerup, sans22, white, midMid, true,
					{ (float) 2.1, (float)180 }, { (float) 0.012, (float)0 }, 400, 255, -(float)1.135);
			}
		}
		powerUps.cleanDeathFlags();
		powerUps.unlock();

		//update player bullet list
		playerBullets.lock();
		for (auto i : playerBullets.getEntList()) {
			i->update();
		}
		playerBullets.cleanDeathFlags();
		playerBullets.unlock();

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
						powerUps.lock();
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
						powerUps.unlock();
					}
					
				}
			}
		}
		playerBullets.unlock();
		enemyEntities.unlock();



		//Update dialogues
		stuckCounter = dialogue->updateDialogue(counter, getInput());

		//update float text
		floatText->lock();
		floatText->updateAllTextEnts();
		floatText->unlock();

		//Clear dead anonEnts
		anonEnts.lock();
		anonEnts.cleanDeathFlags();
		anonEnts.unlock();

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
				levelSettingsCurrent->buttonFlag = none;
			}
			else if (levelSettingsCurrent->buttonFlag == none) {
				levelSettingsCurrent->buttonFlag = pause;
			}
		}
	}

	void renderCycle() {
		//Draw background
		backgroundManager->lock();
		backgroundManager->drawBackground(shift);
		backgroundManager->unlock();

		//Draw weather back
		weatherMasterLock.lock();
		weatherEffectManager->drawBackWeather(shift);
		weatherMasterLock.unlock();

		//Draw background objects under player
		backgroundObjectManager->lock();
		for (auto i : backgroundObjectManager->getSharedList()->getEntList()) {
			if (i->getDepth() > 0) {
				auto temp = i->renderCopy();
				temp.renderSize.x += (int)(player->getPosition().x - i->getPosition().x);
				drawBoxEntity(temp, para::getShift( player->getPosition(), i->getPosition(), i->getDepth() ), FULLNORMAL,
					para::getSizeScaler( i->getDepth() ) );
			} 
		}
		backgroundObjectManager->unlock();

		//Draw weather middle
		weatherMasterLock.lock();
		weatherEffectManager->drawMiddleWeather(shift);
		weatherMasterLock.unlock();

		//Draw anon ents under player bullets
		anonEnts.lock();
		for (auto i : anonEnts.getEntList()) {
			drawBoxEntity(i->renderCopy(), shift, FULLNORMAL, FULLSIZE);
		}
		anonEnts.unlock();

		//Draw player bullets
		playerBullets.lock();
		for (auto i : playerBullets.getEntList()) {
			drawBoxEntity(i->renderCopy(), shift, FULLNORMAL, FULLSIZE);
		}
		playerBullets.unlock();

		//Draw powerups
		powerUps.lock();
		for (auto i : powerUps.getEntList()) {
			drawBoxEntity(i->renderCopy(), shift, FULLNORMAL, FULLSIZE);
		}
		powerUps.unlock();

		//Draw Enemies
		enemyEntities.lock();
		for (auto i : enemyEntities.getEntList()) {
			drawBoxEntity(i->renderCopy(), shift, FULLNORMAL, FULLSIZE);
		}
		enemyEntities.unlock();

		//Draw player
		player->lock();
		drawBoxEntity(player->renderCopy(), shift, FULLNORMAL, FULLSIZE);
		player->unlock();

		//Draw particles
		particleMaster->renderParticles(shift);

		//Draw float text
		floatText->lock();
		floatText->renderText(shift);
		floatText->unlock();

		//Draw weather above
		weatherMasterLock.lock();
		weatherEffectManager->drawAboveWeather(shift);
		weatherMasterLock.unlock();

		//Draw overlay
		SDL_RenderCopy(graphicsState->getGRenderer(), overlay, NULL, NULL);

		//Draw dialogue
		dialogue->drawDialogue();
	}
};

#endif