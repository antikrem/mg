/*Player bullet classes*/
#ifndef __BULLET_PLAYER_H_INCLUDED__
#define __BULLET_PLAYER_H_INCLUDED__

#include "box_entity.h"
#include "listed_entities.h"
#include "constants.h"

class PlayerBullet : public ListedEntity {
private:
	//Delete boundary
	SDL_Rect boundaryToDelete;

	//Damage done on contact
	int damage = 0;

	//Sets flag to death if entity outside of delete area
	void outOfBounds() {
		if (!checkPointInRect(position, boundaryToDelete)) {
			flag = false;
		}
	}

public:
	PlayerBullet(string bulletName, CUS_Point spawnPos, float hitBox, int damage, CUS_Point velocity) {
		backupFrameType = idle;
		currentFrameType = idle;
		
		this->damage = damage;

		setAnimationSet(getFromStore(bulletName));

		setHitBox(hitBox);

		position = spawnPos;

		int maxDist = (int)sqrt(pow(getFromStore(bulletName)->getAnimation(idle)->getWidth(), 2) + pow(getFromStore(bulletName)->getAnimation(idle)->getHeight(), 2)) + 5;

		boundaryToDelete = { -maxDist, -maxDist, WORK_SPACE_X + 2 * maxDist , WORK_SPACE_Y + 2 * maxDist + 200};

		this->velocity = velocity;

		updateBox();
	}

	void update() {
		position += velocity;

		angle = (float)(atan2(velocity.x, -velocity.y) * 180 / PI);
		updateBox();
		outOfBounds();
	}

	int getDamage() {
		return damage;
	}

};

#endif