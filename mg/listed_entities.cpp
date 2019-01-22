#include "listed_entities.h"

ListedEntity::~ListedEntity() {
	if (forceApplier)
		forceApplier->setFlagFalse();
	forceApplier = NULL;
}

void ListedEntity::setHitBox(float hitBox) {
	this->hitBox = hitBox;
}

float ListedEntity::getHitBox() {
	return hitBox;
}

void ListedEntity::setFlag(bool flag) {
	this->flag = flag;
}

bool ListedEntity::getFlag() {
	return flag;
}

void ListedEntity::updateForceApplier() {
	if (forceApplier) {
		forceApplier->updatePositionAndVelocity(position, velocity);
	}
}

ForceApplier* ListedEntity::particulate(bool important) {
	if (!forceApplier)
		forceApplier = new ForceApplier();
	return forceApplier;
}