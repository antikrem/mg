#ifndef __BULLET_ENEMY_H__
#define __BULLET_ENEMY_H__

#include "movement_commander.h"
#include "powerup.h"

#define BULLETBOUNDARYTODELETE { -100, - 100, WORK_SPACE_X+200, WORK_SPACE_Y+200 }

///Templates
class BulletTemplate {
public:
	string animationName;
	float hitbox;
	map <int, MovementCommand> movementList;
	CUS_Polar initialVelocity = { 0, 0 };
	//Relative is false by default
	vector<int> flipRelativeTrue;
	vector<int> flipRelativeFalse;

	BulletTemplate(string animationName, CUS_Polar initialVelocity) {
		this->animationName = animationName;
		this->initialVelocity = initialVelocity;
	}

	void addMovementCommand(int cycle, MovementCommand movementCommand) {
		movementList[cycle] = movementCommand;
	}
};

class BulletSpawnerTemplate {
public:
	///Movement Control
	CUS_Point initialPosition;
	CUS_Polar initialVelocity;
	map <int, MovementCommand> movementList;

	///Spawn position
	//Displacement from spawn controler
	float displacement = 0;
	//Angle from spawn Template to spawn bullets facing
	vector<float> exitLocations;
	//rotation per cycle
	float rotationSpeed = 0;

	///Bullet Control
	int initialDelay = 0;
	//Bullet that is spawned by this spawner
	
	//Bullets come out whenever spray pattern is equal to current counter
	//When counter is larger than spray timer, 
	vector<int> sprayTimer;
	//When the last spray of spray timer occurs inBetweenDelay is the delay until next volly
	int inBetweenTimer = 0;
	//If -1, sprays will go until master is dead, otherwise it will only do this many volleys
	int rounds = -1;

	BulletTemplate* bulletTemplate = NULL;

	BulletSpawnerTemplate(CUS_Point initialPosition, CUS_Polar initialVelocity) {
		this->initialPosition = initialPosition;
		this->initialVelocity = initialVelocity;
	}

	void addSprayTimer( vector<int> sprayTimer ) {
		this->sprayTimer.insert(this->sprayTimer.end(), sprayTimer.begin(), sprayTimer.end());
	}

	void setRounds(int rounds) {
		this->rounds = rounds;
	}

	void setInitialDelay(int initialDelay) {
		this->initialDelay = initialDelay;
	}

	void setInBetweenTimer(int delay) {
		this->inBetweenTimer = delay;
	}

	void addExitLocation(float location) {
		exitLocations.push_back(location);
	}

	void addBulletTemplate(BulletTemplate* bulletTemplate) {
		this->bulletTemplate = bulletTemplate;
	}

	void addMovementCommand(int cycle, MovementCommand movementCommand) {
		movementList[cycle] = movementCommand;
	}
};

class BulletMasterTemplate {
public:
	vector<BulletSpawnerTemplate*> bulletSpawnerTemplates;
	PowerUpTable powerUpTable;
	string name;

	BulletMasterTemplate(string name) {
		this->name = name;
	}

	~BulletMasterTemplate() {
		for (auto bulletSpawnerTemplate : bulletSpawnerTemplates) {
			delete bulletSpawnerTemplate;
			bulletSpawnerTemplate = NULL;
		}
			
	}

	void addBulletSpawnerTemplate(BulletSpawnerTemplate* bulletSpawnerTemplate) {
		bulletSpawnerTemplates.push_back(bulletSpawnerTemplate);
	}
};

class Bullet : public MovementCommander {
private:
	int hitbox = 1;
public:
	Bullet(BulletTemplate* bulletTemplate, CUS_Point position, float exitAngle) {
		auto temp = bulletTemplate->initialVelocity;
		temp.angle += exitAngle;
		angleState = velocityAngle;
		setStartingVelocity(temp);
		setStartingPosition(position);

		setAnimationSet( getFromStore(bulletTemplate->animationName) );

		for (auto commands : bulletTemplate->movementList) {
			addMovementCommand(commands.first, commands.second);
		}

		updateBox();
	}

	CUS_Point update(CUS_Point playerPosition) {
		auto ret =  updateMovement(playerPosition, NULL);
		updateBox();
		if (!checkPointInRect(position, BULLETBOUNDARYTODELETE))
			flag = false;
		return ret;
	}

};

enum BulletSpawnerMode {
	initialDelayMode,
	sprayMode,
	inBetweenTimerMode,
	sprayOverMode,
};

//Bullet Spawner Actual
class BulletSpawner : public MovementCommander {
private:
	int internalCounter = 0;
	int switchTimer = 0;

	float displacement;
	vector<float> exitLocations;
	float rotationSpeed = 0;
	BulletTemplate* bulletTemplate = NULL;

	int switchCounter = 0;
	BulletSpawnerMode mode = initialDelayMode;

	vector<int> sprayTimer;
	int initialDelay = 0;
	int lengthOfSpray = 0; 
	int inBetweenTimer = 0;
	int currentRound = 0;
	int rounds = -1;


public:

	BulletSpawner(BulletSpawnerTemplate* bulletSpawnerTemplate, CUS_Point masterPosition) {
		addFirstMaster( masterPosition);
		position = bulletSpawnerTemplate->initialPosition + lastMaster;
		velocity = bulletSpawnerTemplate->initialVelocity;
		bulletTemplate = bulletSpawnerTemplate->bulletTemplate;
		inBetweenTimer = bulletSpawnerTemplate->inBetweenTimer;
		sprayTimer.insert(sprayTimer.begin(), bulletSpawnerTemplate->sprayTimer.begin(), bulletSpawnerTemplate->sprayTimer.end());
		rounds = bulletSpawnerTemplate->rounds;

		for (auto i : sprayTimer) {
			if (i > lengthOfSpray)
				lengthOfSpray = i;
		}

		for (auto entry : bulletSpawnerTemplate->movementList) {
			addMovementCommand(entry.first, entry.second);
		}

		for (auto entry : bulletSpawnerTemplate->exitLocations) {
			exitLocations.push_back(entry);
		}
	}

	/*Returns either empty vector, 
	or vector of new bullets ready to be listed*/
	vector<Bullet*> update(CUS_Point playerPosition, CUS_Point master) {
		internalCounter++;
		switchCounter++;

		updateMovement(playerPosition, &master);

		for (unsigned int i = 0; i < exitLocations.size(); i++) {
			exitLocations[i] = normaliseAngle(exitLocations[i] + rotationSpeed);
		}

		vector<Bullet*> returnList;

		if (mode == initialDelayMode) {
			if (switchCounter >= initialDelay) {
				mode = sprayMode;
				switchCounter = -1;
			}
		} 
		else if (mode == sprayMode) {
			if (count(sprayTimer.begin(), sprayTimer.end(), switchCounter)) {
				CUS_Polar offset;
				for (auto angle : exitLocations) {
					offset = { displacement,angle };
					returnList.push_back(new Bullet(bulletTemplate, position + toPoint(offset), angle));
				}
			}
			if (switchCounter == lengthOfSpray) {
				mode = inBetweenTimerMode;
				currentRound++;
				switchCounter = -1;
			}

		}
		else if (mode == inBetweenTimerMode) {
			if (switchCounter >= inBetweenTimer) {
				mode = sprayMode;
				switchCounter = -1;
			}
			if ((currentRound >= rounds) && (rounds != -1)) {
				mode = sprayOverMode;
			}
		}
		
		return returnList;
	}

};

class BulletMaster {
private:
	string name;
	vector<BulletSpawner*> bulletSpawners;

	SharedEntityList<Bullet> bulletList;
public:

	BulletMaster(BulletMasterTemplate* bulletMasterTemplate, CUS_Point enemyPosition) {
		this->name = bulletMasterTemplate->name;

		for (auto spawnerTemplate : bulletMasterTemplate->bulletSpawnerTemplates) {
			bulletSpawners.push_back(new BulletSpawner(spawnerTemplate, enemyPosition));
		}
	}

	vector<shared_ptr<Bullet>> update(CUS_Point playerPosition, CUS_Point enemyPosition) {
		vector<shared_ptr<Bullet>> newBullets;
		
		for (auto spawner : bulletSpawners) {
			auto bullets = spawner->update(playerPosition, enemyPosition);
			if (bullets.size()) {
				for (auto bullet : bullets) {
					newBullets.push_back( bulletList.pushObject(bullet) );
				}
			}
		}

		return newBullets;
	}

};

enum BTModes {
	nobt,
	bmt,
	bst,
	bt
};

static void pullBMT(map<string, BulletMasterTemplate*>* bmtList, LevelSettings* levelSettings) {
	BTModes mode = nobt;
	BulletMasterTemplate* bmtToPull = NULL;
	BulletSpawnerTemplate* bstToPull = NULL;
	BulletTemplate* btToPull = NULL;

	string filePath = "campaigns\\" + levelSettings->campaign + "\\" + to_string(levelSettings->level) + "\\" + "bullet_table.txt";

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
		else if (lineVec.size() == 2 && lineVec[0] == "BULLETMASTER") {
			if (bmtToPull) {
				(*bmtList)[bmtToPull->name] = bmtToPull;
				bstToPull = NULL;
				btToPull = NULL;
			}
			mode = bmt;
			bmtToPull = new BulletMasterTemplate(lineVec[1]);
		}
		else if (lineVec[0] == "SPAWNER") {
			if (lineVec.size() == 5) {

				mode = bst;
				if (bmtToPull) {
					bstToPull = new BulletSpawnerTemplate({ stof(lineVec[1]), stof(lineVec[2]) }, { stof(lineVec[3]), stof(lineVec[4]) });
					if (btToPull) {
						bstToPull->addBulletTemplate(btToPull);
					}
					bmtToPull->addBulletSpawnerTemplate(bstToPull);
				}

			}
		}
		else if (lineVec[0] == "VOLLEYTIMER") {
			if (mode == bst) {
				vector<int> sprayTimer;
				for (int i = 1; i < (int)lineVec.size(); i++) {
					sprayTimer.push_back(stoi(lineVec[i]));
				}
				if (bstToPull) {
					bstToPull->addSprayTimer(sprayTimer);
				}

			}
		}
		else if (lineVec[0] == "BETWEENVOLLEYPAUSE") {
			if (mode == bst) {
				if (lineVec.size() == 2) {
					bstToPull->setInBetweenTimer(stoi(lineVec[1]));
				}
			}
		}
		else if (lineVec[0] == "ROUNDS") {
			if (mode == bst) {
				if (lineVec.size() == 2) {
					bstToPull->setRounds(stoi(lineVec[1]));
				}
			}
		}
		else if (lineVec[0] == "EXIT") {
			if (bstToPull) {
				if (mode == bst) {
					for (unsigned int i = 1; i < lineVec.size(); i++) {
						bstToPull->addExitLocation(stof(lineVec[i]));
					}
				}
			}
		}
		else if (lineVec[0] == "BULLET") {
			if (lineVec.size() == 4) {
				mode = bt;
				btToPull = new BulletTemplate(lineVec[1], { stof(lineVec[2]),  stof(lineVec[3]) }); //Animation name
			} 
			else {
				cout << "BULLET size check TODO";
			}
			
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

			if (mode == bst) {
				if (bstToPull) {
					bstToPull->addMovementCommand(stoi(lineVec[1]), currentUpdate);
				}
			}
			else if (mode == bt) {
				if (btToPull) {
					btToPull->addMovementCommand(stoi(lineVec[1]), currentUpdate);
				}
			}
			else {
				err::logMessage("The file: " + filePath + " decalres a MovementCommand, with no bullet template or bullet spawner template above: " + to_string(lineNo));
			}
		}
		else {
			err::logMessage("An incorrect amount of parameters was defined in file " + filePath + " at " + to_string(lineNo));
		}
	}

	if (bmtToPull) {
		(*bmtList)[bmtToPull->name] = bmtToPull;
	}
}

#endif