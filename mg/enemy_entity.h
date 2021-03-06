/*PLayer class, with character specification
To be used only in _level.h*/
#ifndef __ENEMY_ENTITY_H__
#define __ENEMY_ENTITY_H__

#include <algorithm>
#include <fstream>
#include <string> 
#include <iostream>

#include "str_kit.h"
#include "cus_structs.h"
#include "box_entity.h"
#include "listed_entities.h"

#include "bullet_enemy.h"

/*Standard class for an enemy. All enemies have a spawning cycle and a MovementSet, which contains spawn time positions, and death time
Requires MovementControl and Animation set be specified for proper execution.
*/
class EnemyEntity : public MovementCommander {
protected:
	int internalCycle = -1;
	int spawningCycle;
	int deathCycle;

	int health = 1;
	int maxHealth;

	//Total powerup table to be dropped on death
	PowerUpTable powerUpTable;
	//Flipped false when powewrups got
	bool powerUpDroppable = true;

	string bmtName = "";
	int bmtDelay = 0;
	BulletMaster* bulletMaster = NULL;

public:
	EnemyEntity(int spawningCycle, string animationName, float hitbox, int health,
		CUS_Point startingPosition, CUS_Polar startingVelocity) {
		this->spawningCycle = spawningCycle;

		setAnimationSet( getFromStore(animationName) );
		
		this->hitBox = hitbox;
		this->maxHealth = health;
		this->health = this->maxHealth;

		this->position = startingPosition;
		setStartingPosition(startingPosition);
		setStartingVelocity(startingVelocity);
		updateBox();
	}

	~EnemyEntity() {
		delete bulletMaster;
	}

	//Returns bullets spawned
	vector<shared_ptr<Bullet>> update(CUS_Point playerPosition, map<string, BulletMasterTemplate*>* bmtList, SharedEntityList<BulletSpawner>* spawnerMasterList) {
		internalCycle++;
		itCurrentFrame();
		updateMovement(playerPosition, NULL);
		updateBox();
		if (health <= 0)
			flag = false;
		if (deathCycle <= internalCycle)
			flag = false;
		//Check bullets
		if (bmtName != "") {
			if (bmtDelay == internalCycle) {
				bulletMaster = new BulletMaster( (*bmtList)[bmtName], position );
				bulletMaster->addToMasterBulletSpawnerList(spawnerMasterList);
			}
		}
		vector<shared_ptr<Bullet>> returnList;
		if (bulletMaster) {
			returnList = bulletMaster->update(playerPosition, position);
		}
		return returnList;
	}

	BulletMaster* getBMT() {
		return bulletMaster;
	}

	int getSpawningCycle() {
		return spawningCycle;
	}

	void takeDamage(int damage) {
		this->health -= damage;
	}

	int getMaxHealth() {
		return maxHealth;
	}

	int getCurrentHealth() {
		return health;
	}

	void setDeathCycle(int deathCycle) {
		this->deathCycle = deathCycle;
	}

	PowerUpTable getPowerUpTable() {
		if (powerUpDroppable) {
			powerUpDroppable = false;
			return powerUpTable;
		}
		else {
			//Return empty powerUpTable, since powerup has been dropped already
			PowerUpTable temp;
			return temp;
		}
	}

	void addBMT(string bmtName, int bmtDelay) {
		this->bmtName = bmtName;
		this->bmtDelay = bmtDelay;
	}
};

static void pullEnemies(vector<EnemyEntity*>* enemyVec, LevelSettings* levelSettings) {
	EnemyEntity* toPull = NULL;

	string filePath = "campaigns\\" + levelSettings->campaign + "\\" + to_string(levelSettings->level) + "\\" + "enemy_table.txt";

	int lineNo = 0;
	ifstream inFile;
	inFile.open(filePath.c_str());
	string line;

	//TODO: try catch ex
	while (getline(inFile, line)) {
		lineNo++;
		auto lineVec = str_kit::splitOnToken(line, ' ');
		if ((line[0] == '/' && line[1] == '/')) {
			pass;
		}
		else if (line == "") {
			pass;
		}
		else if (lineVec.size() == 0) {
			pass;
		}
		else if (lineVec.size() == 9 && lineVec[0] == "MOVE") {
			MovementCommand currentUpdate(
			stoi(lineVec[2]),
			stoi(lineVec[3]),
			stoi(lineVec[4]),
			stoi(lineVec[3]) ? -1 * stof(lineVec[5]) + 180 : stof(lineVec[5]),
			stoi(lineVec[6]),
			stoi(lineVec[7]),
			stof(lineVec[8])
			);

			if (currentUpdate.relativeToPlayer)
				currentUpdate.angle -= 180;

			if (toPull) {
				toPull->addMovementCommand(stoi(lineVec[1]), currentUpdate);
			}
			else {
				err::logMessage("The file: " + filePath + " decalres a MovementCommand, before an EnemyEntity was created at line: " + to_string(lineNo));
			}
		}
		else if (lineVec.size() == 9 && lineVec[0] == "ENEMY") {
			if (toPull)
				err::logMessage("New enemy spawned with the previous not having a death cycle at line: " + to_string(lineNo) + " in file " + filePath);

			toPull = new EnemyEntity(
				stoi(lineVec[1]),
				lineVec[2],
				stof(lineVec[3]),
				stoi(lineVec[4]),
				{ stof(lineVec[5]), stof(lineVec[6]) },
				{ stof(lineVec[7]), stof(lineVec[8]) }
			);
		}
		else if (lineVec.size() == 2 && lineVec[0] == "DEATH") {
			if (!toPull)
				err::logMessage("ERROR: death cycle with no previous enemy at line: " + to_string(lineNo) + " in file " + filePath);
			else {
				toPull->setDeathCycle(stoi(lineVec[1]));
				enemyVec->push_back(toPull);
				toPull = NULL;
			}
		}
		else if (lineVec[0] == "BULLETMASTER") {
			if (!toPull)
				err::logMessage("ERROR: bulletmaster with no previous enemy at line: " + to_string(lineNo) + " in file " + filePath);
			else {
				if (lineVec.size() == 3) {
					toPull->addBMT(lineVec[2], stoi(lineVec[1]));
				}
			}
		}
		else {
			err::logMessage("An incorrect amount of parameters was defined in file " + filePath + " at " + to_string(lineNo));
		}
	}

	if (toPull) {
		err::logMessage("line 202 EnemyNotPushed");
	}
}

#endif