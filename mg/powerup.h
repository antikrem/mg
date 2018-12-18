#ifndef __POWERUP_H__
#define __POWERUP_H__

#include "listed_entities.h"

enum UpgradeType {
	life,
	power,
	spell,
	points
};

struct PowerUpTable {
	int life = 0;
	int power = 0;
	int spell = 0;
	int points = 0;

	void operator+=(const PowerUpTable& b) {
		this->life += b.life;
		this->power += b.power;
		this->spell += b.spell;
		this->points += b.points;
	}
};

class PowerUp : public ListedEntity {
private:
	bool onRails = false;

	UpgradeType upgradeType;
	CUS_Point accelerator = { 0, (float)0.008 };

	int internalPoints = 0;

public:
	PowerUp(UpgradeType upgradeType, CUS_Point position, int hitbox) {
		CUS_Polar displacement = { (float)2.5 * hitbox * random::randomFloat(),  random::randomFloat() * 359 };
		this->position = position + displacement.toPoint();

		this->upgradeType = upgradeType;

		switch (upgradeType) {
		case(life):
			setAnimationSet(getFromStore("base_powerup"));
			internalPoints = 1000;
			break;
		case(power):
			setAnimationSet(getFromStore("base_powerup"));
			internalPoints = 10;
			break;
		case(spell):
			setAnimationSet(getFromStore("base_powerup"));
			internalPoints = 500;
			break;
		case(points):
			setAnimationSet(getFromStore("base_powerup"));
			internalPoints = 100;
			break;
		}

		updateBox();
	}

	//returns true if its time to delete and spawn some float text
	bool updatePosition(CUS_Point playerPos) {
		if (position.getDistanceToPoint(playerPos) < 200) {
			onRails = true;
		}
		if (position.getDistanceToPoint(playerPos) < 20) {
			flag = false;
		}

		if (onRails) {
			velocity = toPoint({ (float)5, (toPolar(velocity).angle + position.getAngleToPoint(playerPos)) / 2});
		}
		else {
			velocity += accelerator;
		}
		position += velocity;

		itCurrentFrame();
		updateBox();
		return !flag;
	}

	void setFlag(bool flag) {
		this->flag = flag;
	}

	void killOutOfBounds() {
		if (position.y > (WORK_SPACE_Y + 50)) {
			flag = false;
		}
	}

	int getPoints() {
		return internalPoints;
	}

	bool getFlag() {
		return flag;
	}

	UpgradeType getUpgradeType() {
		return upgradeType;
	}
};

#endif