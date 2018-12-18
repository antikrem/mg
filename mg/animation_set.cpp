#include "constants.h"
#include "animation_set.h"
#include "str_kit.h"

#include <fstream>

/*Stores a set of animations for default entity
Use to avoid redundant texture storage and reduce memory footprint
*/
AnimationAssignment::AnimationAssignment(bool local, bool campaignLocal) {
	this->local = local;
	this->campaignLocal = campaignLocal;
}

AnimationAssignment::~AnimationAssignment() {
	mt_ani::iterator it;
	for (it = animationMap.begin(); it != animationMap.end(); it++) {
		if (it->second) {
			delete it->second;
			it->second = NULL;
		}
	}
}

void AnimationAssignment::addAnimation(AnimationType animationType_param, Animation* animation) {
	animationMap[animationType_param] = animation;
}

Animation* AnimationAssignment::getAnimation(AnimationType animationType_param) {
	if (!this)
		err::logMessage("Getting animation from an assignment that does not exist");

	if (!animationMap[animationType_param])
		err::logMessage("Animation not initialised for this set");

	return animationMap[animationType_param];
}

SDL_Texture* AnimationAssignment::getFrame(AnimationType animationType_param, int frameNumber) {
	return (animationMap[animationType_param])->getFrame(frameNumber);
}

SDL_Texture* AnimationAssignment::getShadowFrame(AnimationType animationType_param, int frameNumber) {
	return (animationMap[animationType_param])->getShadowFrame(frameNumber);
}

SDL_Texture* AnimationAssignment::getLightFrame(AnimationType animationType_param, int frameNumber) {
	return (animationMap[animationType_param])->getLightFrame(frameNumber);
}


void AnimationAssignment::setLocal(bool local) {
	this->local = local;
}

bool AnimationAssignment::getLocal() {
	return local;
}

///Local Store definitions
//Map containing all Animation Assignments
static map<string, AnimationAssignment*> localStore;

//Global Animation calls
void addAssToStore(const char* assignmentName, AnimationAssignment* newAss) {
	if (!localStore[assignmentName]) {
		localStore[assignmentName] = newAss;
	}
	else {
		string assignmentNameString = assignmentName;
		err::logMessage("Two animation assignments of same name made " + assignmentNameString);
	}
};

bool checkInStore(string assignmentName) {
	return (bool) localStore[assignmentName];
}

AnimationAssignment* getFromStore(string assignmentName) {

	if (localStore[assignmentName]) {
		return localStore[assignmentName];
	}
	else {
		err::logMessage("An animation assignment was requested from a store when it doesn't exist: " + assignmentName);
		return NULL;
	}

}

void clearAnimaionStore() {
	for (auto i : localStore) {
		delete i.second;
		i.second = NULL;
	}
	localStore.clear();
}

void clearLocalAnimaionStore() {
	for (auto i : localStore) {
		if (i.second->getLocal()) {
			delete i.second;
			i.second = NULL;
		}
	}

	auto it = localStore.begin();
	auto end = localStore.end();

	while (it != end) {
		if (it->second == NULL) {
			it = localStore.erase(it);
			end = localStore.end();
		}
		else {
			it++;
		}
	}
}

AnimationType stringToAnimationType(string in) {
	if (in == "spawn")
		return spawn;
	else if (in == "idle")
		return idle;
	else if (in == "move_left")
		return move_left;
	else if (in == "move_right")
		return move_right;
	else if (in == "death")
		return death;
	else {
		err::logMessage("stringToAnimation with a broken type: " + in);
		return idle;
	}

}

void loadStore(SDL_Renderer* gRenderer) {
	ifstream inFile;
	inFile.open("assets//load_list.txt");

	string basePath = "assets//";
	string resetPath = "assets//";

	string line;
	AnimationAssignment* currentAssignment = NULL;

	int lineNo = 0;
	try {
		while (getline(inFile, line)) {
			lineNo++;
			auto lineVec = str_kit::splitOnToken(line, ' ');
			if (line[0] == '/' && line[1] == '/') {
				pass;
			}
			else if (!line.size()) {
				pass;
			}
			else if (lineVec.size() == 1) {
				currentAssignment = new AnimationAssignment();
				addAssToStore((lineVec[0]).c_str(), currentAssignment);
			}
			else if ((lineVec.size() == 4)) {
				basePath += lineVec[0];
				currentAssignment->addAnimation(stringToAnimationType(lineVec[1]),
					new Animation(gRenderer, basePath.c_str(), stoi(lineVec[2]), stoi(lineVec[3])));
				basePath = resetPath;
			}
			else if ((lineVec.size() == 5)) {
				basePath += lineVec[0];
				currentAssignment->addAnimation(stringToAnimationType(lineVec[1]),
					new Animation(gRenderer, basePath.c_str(), stoi(lineVec[2]), stoi(lineVec[3]), stof(lineVec[4])));
				basePath = resetPath;
			}
			else err::logMessage("At line " + to_string(lineNo) + ": invalid ammount of parameters were paresed. Number of parameters are: " + to_string(lineVec.size()));
		}
	}
	catch (const std::exception & ex)
	{
		ex.what();
		err::logMessage("ERROR: assets//load_list.txt is corrupted at line: " + to_string(lineNo));
	}

	inFile.close();

}

void loadStoreLocal(SDL_Renderer* gRenderer, LevelSettings* levelSettings, bool campaignLocal) {
	ifstream inFile;

	string filePath = "campaigns\\";
	filePath.append(levelSettings->campaign);
	filePath.append("\\");
	filePath.append(to_string(levelSettings->level));
	filePath.append("\\");
	string basePath = filePath;
	filePath.append("local_load.txt");

	inFile.open(filePath);

	if (!inFile) {
		err::logMessage("local_load.txt not present at " + filePath);
	}

	string line;
	AnimationAssignment* currentAssignment = NULL;

	int lineNo = 0;
	try {
		while (getline(inFile, line)) {
			lineNo++;
			auto lineVec = str_kit::splitOnToken(line, ' ');
			if (line[0] == '/' && line[1] == '/') {
				pass;
			}
			else if (!line.size()) {
				pass;
			}
			else if (lineVec.size() == 1) {
				if (!campaignLocal)
					currentAssignment = new AnimationAssignment(true);
				else 
					currentAssignment = new AnimationAssignment(false, true);
				addAssToStore((lineVec[0]).c_str(), currentAssignment);
			}
			else if ((lineVec.size() == 4)) {
				string position = filePath;
				position.append(lineVec[0]);
				currentAssignment->addAnimation(stringToAnimationType(lineVec[1]),
					new Animation(gRenderer, position.c_str(), stoi(lineVec[2]), stoi(lineVec[3])));
			}
			else if ((lineVec.size() == 5)) {
				string position = filePath;
				position.append(lineVec[0]);
				currentAssignment->addAnimation(stringToAnimationType(lineVec[1]),
					new Animation(gRenderer, position.c_str(), stoi(lineVec[2]), stoi(lineVec[3]), stof(lineVec[4])));
			}
			else err::logMessage("At line " + to_string(lineNo) + " in " + filePath + "local_load.txt" + ": invalid ammount of parameters were paresed. Number of parameters are: " + to_string(lineVec.size()));
		}
	}
	catch (const std::exception & ex)
	{
		ex.what();
		err::logMessage("ERROR: "+ filePath + " is corrupted at line: " + to_string(lineNo));
	}

	inFile.close();
}