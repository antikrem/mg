#include "box_entity.h"
#include "listed_entities.h"
#include "level_enum.h"
#include "parallax.h"

#include <vector>

/*Template for BackgroundObjects,
stored in BackgroundObjectManager
*/
struct BackgroundObjectTemplate {
	float maxSpawnDepth;
	float minSpawnDepth;
	int firstCycle;
	int lastCycle;
	float chance = 0;
	AnimationAssignment* animationAssignment = NULL;
	SDL_Rect spawnbox;
};

class BackgroundObject :  public ListedEntity {
private:
	SDL_Rect boundary;
	//Negative depths are entities infront of the player
	float depth;
	CUS_Point baseVelocity;

	void outOfBounds() {
		if (!checkPointInRect(position, boundary))
			flag = false;
	}

public:
	BackgroundObject(BackgroundObjectTemplate* objTemplate) {
		this->depth = random::randomFloat(objTemplate->minSpawnDepth, objTemplate->maxSpawnDepth);

		CUS_Point position;
		position.x = random::randomFloat(objTemplate->spawnbox.x - para::getExcessWidth(depth) - objTemplate->animationAssignment->getAnimation(idle)->getWidth(), 
			objTemplate->spawnbox.x + objTemplate->spawnbox.w + para::getExcessWidth(depth) + objTemplate->animationAssignment->getAnimation(idle)->getWidth());
		position.y = random::randomFloat(objTemplate->spawnbox.y - objTemplate->animationAssignment->getAnimation(idle)->getHeight(),
			objTemplate->spawnbox.y + objTemplate->spawnbox.h);

		this->position = position;

		boundary.x = (int)(-para::getExcessWidth(depth) - objTemplate->animationAssignment->getAnimation(idle)->getWidth() - 50);
		boundary.y = -100 - objTemplate->animationAssignment->getAnimation(idle)->getHeight();
		boundary.w = (int)(2*para::getExcessWidth(depth) + 2 * objTemplate->animationAssignment->getAnimation(idle)->getWidth() + WORK_SPACE_X + 100);
		boundary.h = WORK_SPACE_Y + 200 + 2 * objTemplate->animationAssignment->getAnimation(idle)->getHeight();

		velocity = { 0,0 }; 
		baseVelocity = { 0,0 };

		if (depth < 0 && fabs(depth) > para::getDistanceFromPlane()) {
			this->position.x = 2000;
			this->position.y = 2000;
			flag = false;
		}

		list = depth;

		setAnimationSet(objTemplate->animationAssignment);
		updateBox();
	}

	float getDepth() {
		return depth;
	}

	void update(float velocity) {
		itCurrentFrame();

		if (this->baseVelocity.y != velocity) {
			baseVelocity = { 0, velocity };
			this->velocity.y = velocity * para::getSizeScaler(depth);
		}

		position += this->velocity;
		outOfBounds();
		updateBox();
	}
};

class BackgroundObjectManager {
private:
	vector<BackgroundObjectTemplate> templateList;
	mutex mut;
	SharedEntityList<BackgroundObject> backgroundObjects;
public:
	BackgroundObjectManager(LevelSettings* levelSettings) {
		ifstream inFile;

		string filePath = "campaigns\\";
		filePath.append(levelSettings->campaign);
		filePath.append("\\");
		filePath.append(to_string(levelSettings->level));
		filePath.append("\\");
		string basePath = filePath;
		filePath.append("background_objects.txt");

		inFile.open(filePath);

		if (!inFile) {
			err::logMessage("background_objects.txt not present at " + filePath);
		}
		string line;

		int lineNo = 0;
			while (getline(inFile, line)) {
				lineNo++;
				auto lineVec = str_kit::splitOnToken(line, ' ');
				if (line[0] == '/' && line[1] == '/') {
					pass;
				}
				else if (!line.size()) {
					pass;
				}
				else if (lineVec.size() == 10) {
					try {
						SDL_Rect spawnRect;
						spawnRect.x = stoi(lineVec[4]);
						spawnRect.y = stoi(lineVec[5]);
						spawnRect.w = stoi(lineVec[6]);
						spawnRect.h = stoi(lineVec[7]);

						if (!checkInStore(lineVec[9]))
							err::logMessage("ERROR: the background object at " + filePath + " at " + to_string(lineNo) + "is requesting non-existent animation set " + lineVec[9]);

						this->addNewTemplate(
							stoi(lineVec[0]),
							stoi(lineVec[1]),
							stof(lineVec[2]),
							stof(lineVec[3]),
							spawnRect,
							stof(lineVec[8]),
							lineVec[9]
						);
					}
					catch (const std::exception & ex)
					{
						ex.what();
						err::logMessage("ERROR: " + filePath + "//background_objects.txt is corrupted at line: " + to_string(lineNo));
					}
				}
				else
					err::logMessage("At line " + to_string(lineNo) + " in " + filePath + " has invalid ammount of parameters were paresed. Number of parameters are: " + to_string(lineVec.size()));
			}
		inFile.close();
	}

	void update(float backgroundObjectVelocity, int cycle) {
		backgroundObjects.lock();
		
		for (auto temp : templateList) {
			if (random::randomFloat() < temp.chance && (cycle > temp.firstCycle) && (cycle < temp.lastCycle))
				backgroundObjects.pushObject(new BackgroundObject(&temp));
		}

		for (auto bObj : backgroundObjects.getEntList()) {
			bObj->update(backgroundObjectVelocity);
		}
		backgroundObjects.cleanDeathFlags();

		backgroundObjects.unlock();
	}

	void addNewTemplate(int firstCycle, int lastCycle, float minSpawnDepth, float maxSpawnDepth, SDL_Rect spawnArea, float chance, string animationAssignment) {
		BackgroundObjectTemplate newTemplate;
		newTemplate.firstCycle = firstCycle;
		newTemplate.lastCycle = lastCycle;
		newTemplate.maxSpawnDepth = maxSpawnDepth;
		newTemplate.minSpawnDepth = minSpawnDepth;
		newTemplate.chance = chance;
		newTemplate.spawnbox = spawnArea;

		newTemplate.animationAssignment = getFromStore(animationAssignment);

		templateList.push_back(newTemplate);
	}

	void lock() {
		mut.lock();
	}

	void unlock() {
		mut.unlock();
	}

	SharedEntityList<BackgroundObject>* getSharedList() {
		return &backgroundObjects;
	}
};