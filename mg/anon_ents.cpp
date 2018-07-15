#include <algorithm>
#include <stdlib.h>

#include "box_entity.h"
#include "anon_ents.h"

#include <iostream>


AnonEnt::AnonEnt(string name, GraphicsState* animationStore, string animationName, CUS_Point position, bool abovePlayerBullets) {
	this->name = name;
	this->abovePlayerBullets = abovePlayerBullets;

	position_Entity = position;
	setAnimationSet(animationStore->getFromStore(animationName));
	updateBox();
}

bool AnonEnt::getAbovePlayerBullets() {
	return abovePlayerBullets;
}

void AnonEnt::updatePosition(CUS_Point position) {
	position_Entity = position;
	updateBox();
}

void AnonEnt::spinEnt(float angle) {
	this->angle += angle;
	updateBox();
}

string AnonEnt::getName() {
	return name;
}

void AnonEntInterface::addAnonEnt(string name, GraphicsState* animationStore, string animationName, CUS_Point position, bool abovePlayerBullets) {
	toPush.push_back(new AnonEnt(name, animationStore, animationName, position, abovePlayerBullets));
}

void AnonEntInterface::updateAnonEnt() {
	err::logMessage("MAJOR_FUCKING_ERROR:AnonEnt was inherited but updateAnonEnt was not overwritten");
}

AnonEntInterface::AnonEntInterface() {
	toPush.clear();
	pushed.clear();
}

bool AnonEntInterface::toPushResidual() {
	return toPush.size();
}

shared_ptr<AnonEnt> AnonEntInterface::getAnonEnt() {
	shared_ptr<AnonEnt> temporaryPtr = shared_ptr<AnonEnt>(toPush.front());
	pushed.push_back(temporaryPtr);
	toPush.erase(toPush.begin());
	return temporaryPtr;
}

bool AnonEntInterface::getAnonEntExistence(string name) {
	for (auto anonEnt : pushed) {
		if (anonEnt->getName() == name && anonEnt->getFlag()) {
			return true;
		}
	}
	return false;
}

shared_ptr<AnonEnt> AnonEntInterface::getAnonEntByName(string name) {
	for (auto anonEnt : pushed) {
		if (anonEnt->getName() == name && anonEnt->getFlag()) {
			return anonEnt;
		}
	}
	err::logMessage("getAnonEntByName was given a name that doesn't exist in table, name is: " + name);
	return pushed[0];
}

void AnonEntInterface::clearDeadAnonEnts() {
	pushed.erase(remove_if(
		pushed.begin(), pushed.end(),
		[](shared_ptr<AnonEnt>& x) {
		return !(x->getFlag());
	}), pushed.end());
}