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

	BulletTemplate(string animationName) {
		this->animationName = animationName;
	}

	void addMovementCommand(int cycle, MovementCommand movementCommand) {
		movementList[cycle] = movementCommand;
	}
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
	BulletTemplate* bulletTemplate = NULL;
	int inbetweenDelay;

	BulletSpawnerTemplate(CUS_Point initialPosition, CUS_Point initialVelocity, int inbetweenDelay) {
		this->initialPosition = initialPosition;
		this->initialVelocity = initialVelocity;
		this->inbetweenDelay = inbetweenDelay;
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
	int delayCounter = 0;
	int inbetweenDelay = 0;
	BulletTemplate* bulletTemplate = NULL;

public:

	BulletSpawner(BulletSpawnerTemplate* bulletSpawnerTemplate) {
		internalCounter = -(bulletSpawnerTemplate->initialDelay);
		position = bulletSpawnerTemplate->initialPosition;
		velocity = bulletSpawnerTemplate->initialVelocity;
		bulletTemplate = bulletSpawnerTemplate->bulletTemplate;
		inbetweenDelay = bulletSpawnerTemplate->inbetweenDelay;

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
			exitLocations[i] = normaliseAngle(exitLocations[i] + rotationSpeed);
		}

		vector<Bullet*> returnList;

		delayCounter++;
		if (delayCounter >= inbetweenDelay) {
			cout << "retlist: caught" << endl;
			delayCounter = 0;
			CUS_Polar offset;
			for (auto angle : exitLocations) {
				offset = { displacement,angle };
				returnList.push_back(new Bullet(bulletTemplate, position + toPoint(offset), angle));
			}
			
		}
		cout << "retlist: " << returnList.size() << endl;
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

	vector<shared_ptr<Bullet>> update(CUS_Point playerPosition, CUS_Point enemyPosition) {
		vector<shared_ptr<Bullet>> newBullets;
		
		cout << "Spawner size: " << bulletSpawners.size() << endl;
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
		else if (lineVec.size() == 2 && lineVec[0] == "BMT") {
			if (bmtToPull) {
				(*bmtList)[bmtToPull->name] = bmtToPull;
			}
			mode = bmt;
			bmtToPull = new BulletMasterTemplate(lineVec[1]);
		}
		else if (lineVec[0] == "SPAWNER") {
			if (lineVec.size() == 6) {

				mode = bst;
				if (bmtToPull) {
					bstToPull = new BulletSpawnerTemplate({ stof(lineVec[1]), stof(lineVec[2]) }, { stof(lineVec[3]), stof(lineVec[4]) }, stoi(lineVec[5]));
					if (btToPull) {
						bstToPull->addBulletTemplate(btToPull);
					}
					bmtToPull->addBulletSpawnerTemplate(bstToPull);
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
		else if (lineVec.size() == 2 && lineVec[0] == "BULLET") {
			mode = bt;
			btToPull = new BulletTemplate(lineVec[1]); //Animation name
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