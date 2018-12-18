#ifndef __BULLET_ENEMY_H__
#define __BULLET_ENEMY_H__

#include "movement_commander.h"
#include "powerup.h"

///Templates
class BulletTemplate {
public:
	string animationName;
	float hitbox;
	map <int, MovementCommand> movementList;
	CUS_Polar initialVelocity = { 0, 0 };
	bool relativeToSpawner = false;

};

class BulletSpawnerTemplate {
public:
	///Movement Control
	CUS_Point initialPosition;
	CUS_Point initialVelocity;
	map <int, MovementCommand> movementList;

	///Spawn position
	//Displacement from spawn controler
	float displacement = 0;
	//Angle from spawn Template to spawn
	vector<float> exitLocations;
	//rotation per cycle
	float rotationSpeed = 0;

	///Bullet Control
	int initialDelay = 0;
	//Bullet that is spawned by this spawner
	BulletTemplate* bulletTemplate;
	int inbetweenDelay;

	BulletSpawnerTemplate(CUS_Point initialPosition, CUS_Point initialVelocity) {
		this->initialPosition = initialPosition;
		this->initialVelocity = initialVelocity;
		this->bulletTemplate = bulletTemplate;
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
		temp.angle += bulletTemplate->relativeToSpawner ? exitAngle : 0;
		this->velocity = toPoint(temp);
		this->position = position;

		setAnimationSet( getFromStore(bulletTemplate->animationName) );

		for (auto commands : bulletTemplate->movementList) {
			addMovementCommand(commands.first, commands.second);
		}

		updateBox();
	}

	void update(CUS_Point playerPosition) {
		updateMovement(playerPosition, NULL);
	}
};

//Bullet Master Actual
class BulletSpawner : public MovementCommander {
private:
	int internalCounter = 0;

	float displacement;
	vector<float> exitLocations;
	float rotationSpeed = 0;
	int inbetweenDelay;
	BulletTemplate* bulletTemplate = NULL;

public:

	BulletSpawner(BulletSpawnerTemplate* bulletSpawnerTemplate) {
		internalCounter = -(bulletSpawnerTemplate->initialDelay);
		position = bulletSpawnerTemplate->initialPosition;
		velocity = bulletSpawnerTemplate->initialVelocity;
		bulletTemplate = bulletSpawnerTemplate->bulletTemplate;

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
		updateMovement(playerPosition, &master);

		for (unsigned int i = 0; i < exitLocations.size(); i++) {
			exitLocations[i] = normaliseAngle(exitLocations[i]+ rotationSpeed);
		}

		vector<Bullet*> returnList;

		if not(internalCounter%inbetweenDelay) {
			CUS_Polar offset;
			for (auto angle : exitLocations) {
				offset = { displacement,angle };
				returnList.push_back(new Bullet(bulletTemplate, position + toPoint(offset), angle));
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

	BulletMaster(BulletMasterTemplate* bulletMasterTemplate) {
		this->name = bulletMasterTemplate->name;

		for (auto spawnerTemplate : bulletMasterTemplate->bulletSpawnerTemplates) {
			bulletSpawners.push_back(new BulletSpawner(spawnerTemplate));
		}
	}

	vector<Bullet*> update(CUS_Point playerPosition, CUS_Point enemyPosition) {
		vector<Bullet*> newBullets;
		
		for (auto spawner : bulletSpawners) {
			auto bullets = spawner->update(playerPosition, enemyPosition);
			if (bullets.size()) {
				for (auto bullet : bullets) {
					bulletList.pushObject(bullet);
					newBullets.push_back(bullet);
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

static void pullBMT(map<string, BulletMasterTemplate*>* bmtVec, LevelSettings* levelSettings) {
	BTModes mode = nobt;
	BulletMasterTemplate* bmtToPull = NULL;
	BulletSpawnerTemplate* bstToPull = NULL;

	string filePath = "campaigns\\" + levelSettings->campaign + "\\" + to_string(levelSettings->level) + "\\" + "bmt_table.txt";

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
		else if (lineVec.size() == 2 && lineVec[0] == "BMT") {
			if (bmtToPull) {
				(*bmtVec)[bmtToPull->name] = bmtToPull;
			}
			mode = bmt;
			bmtToPull = new BulletMasterTemplate(lineVec[1]);
		}
		else if (lineVec.size() == 5 && lineVec[0] == "SPAWNER") {
			mode = bst;
			if (bmtToPull) {
				bstToPull = new BulletSpawnerTemplate({ stof(lineVec[1]), stof(lineVec[2]) }, { stof(lineVec[3]), stof(lineVec[4]) });
				bmtToPull->addBulletSpawnerTemplate(bstToPull);
			}
		}
		else if (lineVec[0] == "EXIT" && mode == bst) {
			if (bstToPull) {
				for (unsigned int i = 1; i < lineVec.size(); i++) {
					bstToPull->addExitLocation(stof(lineVec[i]));
				}
			}
		}
		else {
			err::logMessage("An incorrect amount of parameters was defined in file " + filePath + " at " + to_string(lineNo));
		}
	}

	if (!bmtToPull || !bstToPull) {
		err::logMessage("line 202 NOTPUSHED");
	}
}

#endif