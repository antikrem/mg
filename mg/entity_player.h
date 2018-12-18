/*PLayer class, with character specification
To be used only in _level.h*/
#ifndef __ENTITY_PLAYER_H__
#define __ENTITY_PLAYER_H__

#include "constants.h"
#include "listed_entities.h"
#include "thread_safe.h"
#include "input.h"
#include "animation_set.h"

#include "bullets_player.h"
#include "anon_ents.h"

#define MAX_BULLETS_PER_PLAYER 30
#define PLAYER_MAX_DASH_VELOCITY (float)15
#define PLAYER_ACCELERATION (float)0.5
#define PLAYER_MAX_VELOCITY (float)5
#define PLAYER_MAX_FOCUS_VELOCITY (float)2

#define DASH_DURATION 17
#define DASH_COOLDOWN 50

class PlayerEntity : public ListedEntity, public ThreadSafe, public AnonEntInterface {
protected:
	/*Independent of a slave classes cycle counter*/
	int internalCycles = 0;

	Input playIn;

	int indexPos = 0;
	PlayerBullet* bulletTable[MAX_BULLETS_PER_PLAYER];

	//dash variables
	int dashCoolDown = DASH_COOLDOWN;
	int dashTimer = DASH_DURATION;
	bool dashActive = false;
	Input dashDirection;

	//hit stuff
	bool targetable = true;
	int targetableCounter = 0;
	int onRails = 0;

	void cleanBulletTable() {
		for (int i = 0; i < MAX_BULLETS_PER_PLAYER; i++) {
			bulletTable[i] = NULL;
		}
	}

public:
	void playerUpdate(Input in, int shift) {
		playIn = in;

		targetableCounter--;
		if (!targetable && targetableCounter < 0) {
			targetable = true;
		}

		internalCycles++;
		itCurrentFrame();
		updateAnonEnt();
		updateForceApplier();
		updatePosition(shift);
		prepareBulletTable();
		clearDeadAnonEnts();
	}

	void updatePosition(int shift) {
		dashCoolDown--;
		if (onRails < 0) {
			CUS_Point acceleration = { 0.0, 0.0 };

			//resolve y direction
			if (playIn.up)
				acceleration.y -= (float)0.7;

			if (playIn.down) 
				acceleration.y += (float)0.7;

			//resolve x direction
			if (playIn.right) 
				acceleration.x += (float)0.7;

			if (playIn.left) 
				acceleration.x -= (float)0.7;

			//dash
			if (playIn.dash && !dashActive && dashCoolDown < 0) {
				dashTimer = DASH_DURATION;
				dashActive = true;
				dashDirection = playIn;
			}
			else if (dashActive && dashDirection == playIn) {
				dashTimer--;
			}

			if ((dashActive && dashTimer < 0) || (dashActive && !(dashDirection == playIn))) {
				dashActive = false;
				dashCoolDown = DASH_COOLDOWN;
			}

			CUS_Polar tempAccel;
			float tempMAX = PLAYER_MAX_DASH_VELOCITY;

			if ((!playIn.down && !playIn.up && !playIn.right && !playIn.left) ||
				(!playIn.down && !playIn.up && playIn.right && playIn.left) ||
				(playIn.down && playIn.up && !playIn.right && !playIn.left)) {
				tempAccel = { 0 , acceleration.toPolarAngle() };
				tempMAX = 0;
			}
			else {
				tempAccel = { PLAYER_ACCELERATION , acceleration.toPolarAngle() };
			}

			acceleration = tempAccel.toPoint();

			velocity += acceleration;

			CUS_Polar tempVelocity = { dashActive ? tempMAX : velocity.toPolarMagnitude() , velocity.toPolarAngle() };

			if (!dashActive) {
				if (tempVelocity.magnitude > PLAYER_MAX_VELOCITY && !playIn.shift) {
					tempVelocity.magnitude = PLAYER_MAX_VELOCITY;
				}
				else if (tempVelocity.magnitude > PLAYER_MAX_FOCUS_VELOCITY && playIn.shift) {
					tempVelocity.magnitude = PLAYER_MAX_FOCUS_VELOCITY;
				}
			}

			if (acceleration.toPolarMagnitude() == 0) {
				tempVelocity.magnitude -= PLAYER_ACCELERATION;
			}
			if (tempVelocity.magnitude < 0) {
				tempVelocity.magnitude = 0;
			}

			velocity = tempVelocity.toPoint();

			position += velocity;

			//check bounds
			if (position.x < 0)
				position.x = 0;

			if (position.x > WORK_SPACE_X)
				position.x = WORK_SPACE_X;

			if (position.y < 0)
				position.y = 0;

			if (position.y > WORK_SPACE_Y)
				position.y = WORK_SPACE_Y;

			//If shifted, check scrict bounds
			if (playIn.shift) {
				if (position.x < abs(shift+4))
					position.x = F(abs(shift + 4));
				if (position.x > (abs(shift + 4) + RENDERED_X))
					position.x = F(abs(shift + 4) + RENDERED_X);
			}
		}
		else {
			onRails--;
			position.y -= (float)2.5;
		}

		updateBox();
	}

	void registerHit() {
		targetable = false;
		targetableCounter = 550;
		onRails = 270;
		position = { (float)(WORK_SPACE_X) / 2, (float)(WORK_SPACE_Y + 500) };
	}

	bool getOnRails() {
		return (onRails < 0);
	}

	virtual void prepareBulletTable() {
		indexPos = 0;
		//overwrite
	}

	bool tableResidual() {
		for (int i = 0; i < MAX_BULLETS_PER_PLAYER; i++) {
			if (bulletTable[i] != NULL) {
				return true;
			}
		}
		return false;
	}

	int tableResidualPosition() {
		for (int i = 0; i < MAX_BULLETS_PER_PLAYER; i++) {
			if (bulletTable[i] != NULL) {
				return i;
			}
		}
		err::logMessage("tableResidualPosition failed to find a proper value" + to_string(internalCycles) );
		return -1;
	}

	PlayerBullet* getBullet(int index) {
		PlayerBullet* temp = bulletTable[index];
		bulletTable[index] = NULL;
		return temp;
	}

	bool getFocus() {
		return playIn.shift;
	}

	bool getTargetable() {
		return targetable;
	}

	void setTargetable(bool targetable, int duration = 0) {
		this->targetable = targetable;
		if (targetable == false) {
			targetableCounter = duration;
		}
	}
};

class Sorceress : public PlayerEntity {
	float turretOffset = 0;

	float turreSpeed = (float)1;

public:
	Sorceress() {
		cleanBulletTable();
		backupFrameType = idle;
		currentFrameType = idle;

		hitBox = 50;

		setAnimationSet( getFromStore("default_player") );

		position = { WORK_SPACE_X / 2 , WORK_SPACE_Y - 100.0 };

		updateBox();

		//Load fire circle anon ent
		addAnonEnt("fire_cicle0", "fire_circle", { position.x + velocity.x * 4, position.y - 128 }, true);
		addAnonEnt("fire_cicle5", "fire_circle", { position.x + velocity.x * 4, position.y - 128 }, true);
		addAnonEnt("fire_cicle1", "fire_circle", { position.x + velocity.x * 4, position.y - 128 }, true);
		addAnonEnt("fire_cicle4", "fire_circle", { position.x + velocity.x * 4, position.y - 128 }, true);
		addAnonEnt("fire_cicle2", "fire_circle", { position.x + velocity.x * 4, position.y - 128 }, true);
		addAnonEnt("fire_cicle3", "fire_circle", { position.x + velocity.x * 4, position.y - 128 }, true);

	}

	void updateAnonEnt() {
		CUS_Point temporarypoint = { (float)(position.x + velocity.x * 2.5), (float)(position.y - 138) };

		if (getAnonEntExistence("fire_cicle0")) {
			CUS_Polar temporarypointspeed = { -310 * turretOffset, 180 - 35 };
			getAnonEntByName("fire_cicle0")->updatePosition(temporarypoint + temporarypointspeed.toPoint());

			getAnonEntByName("fire_cicle0")->spinEnt(turreSpeed);
		}

		if (getAnonEntExistence("fire_cicle1")) {
			CUS_Polar temporarypointspeed = { -310 * turretOffset, 180 - 21 };
			getAnonEntByName("fire_cicle1")->updatePosition(temporarypoint + temporarypointspeed.toPoint());

			getAnonEntByName("fire_cicle1")->spinEnt(turreSpeed);
		}

		if (getAnonEntExistence("fire_cicle2")) {
			CUS_Polar temporarypointspeed = { -310 * turretOffset, 180 - 7 };
			getAnonEntByName("fire_cicle2")->updatePosition(temporarypoint + temporarypointspeed.toPoint());

			getAnonEntByName("fire_cicle2")->spinEnt(turreSpeed);
		}

		if (getAnonEntExistence("fire_cicle3")) {
			CUS_Polar temporarypointspeed = { -310 * turretOffset, 180 + 7 };
			getAnonEntByName("fire_cicle3")->updatePosition(temporarypoint + temporarypointspeed.toPoint());

			getAnonEntByName("fire_cicle3")->spinEnt(turreSpeed);
		}

		if (getAnonEntExistence("fire_cicle4")) {
			CUS_Polar temporarypointspeed = { -310 * turretOffset, 180 + 21 };
			getAnonEntByName("fire_cicle4")->updatePosition(temporarypoint + temporarypointspeed.toPoint());

			getAnonEntByName("fire_cicle4")->spinEnt(turreSpeed);
		}
		if (getAnonEntExistence("fire_cicle5")) {
			CUS_Polar temporarypointspeed = { -310 * turretOffset, 180 + 35 };
			getAnonEntByName("fire_cicle5")->updatePosition(temporarypoint + temporarypointspeed.toPoint());

			getAnonEntByName("fire_cicle5")->spinEnt(turreSpeed);
		}
	}

	void prepareBulletTable() {
		indexPos = 0;
		
		if (playIn.shift && turretOffset < 1) {
			turretOffset += (float) 0.01;
		}
		if (!playIn.shift && turretOffset > 0) {
			turretOffset -= (float) 0.01;
			if (turretOffset < 0) {
				turretOffset = 0;
			}
		}

		if (internalCycles % 20 == 0 && playIn.shoot) {

			CUS_Point startPos = { (float)(position.x + velocity.x * 2.5), (float)(position.y - 138) };

			CUS_Polar temporarypointspeed = { -310 * turretOffset, 180 + 35 };
			CUS_Polar tempThing = { 19, 180 - 15 + turretOffset * (float) 26.5 };
			PlayerBullet* temp = new PlayerBullet("default_bullet", startPos + temporarypointspeed.toPoint(), 50, 10, tempThing.toPoint());

			temporarypointspeed = { -310 * turretOffset, 180 + 21 };
			tempThing = { 19, 180 - 9 + turretOffset * (float)16 };
			PlayerBullet* temp1 = new PlayerBullet("default_bullet", startPos + temporarypointspeed.toPoint(), 50, 10, tempThing.toPoint());

			temporarypointspeed = { -310 * turretOffset, 180 + 7 };
			tempThing = { 19, 180 - 3 + turretOffset * (float) 5.4 };
			PlayerBullet* temp2 = new PlayerBullet("default_bullet", startPos + temporarypointspeed.toPoint(), 50, 10, tempThing.toPoint());

			temporarypointspeed = { -310 * turretOffset, 180 - 7 };
			tempThing = { 19, 180 + 3 - turretOffset * (float) 5.4 };
			PlayerBullet* temp3 = new PlayerBullet("default_bullet", startPos + temporarypointspeed.toPoint(), 50, 10, tempThing.toPoint());

			temporarypointspeed = { -310 * turretOffset, 180 - 21 };
			tempThing = { 19, 180 + 9 - turretOffset * (float)16 };
			PlayerBullet* temp4 = new PlayerBullet("default_bullet", startPos + temporarypointspeed.toPoint(), 50, 10, tempThing.toPoint());

			temporarypointspeed = { -310 * turretOffset, 180 - 35 };
			tempThing = { 19, 180 + 15 - turretOffset * (float) 26.5 };
			PlayerBullet* temp5 = new PlayerBullet("default_bullet", startPos + temporarypointspeed.toPoint(), 50, 10, tempThing.toPoint());


			bulletTable[1] = temp;
			bulletTable[2] = temp1;
			bulletTable[3] = temp2;
			bulletTable[0] = temp3;
			bulletTable[4] = temp4;
			bulletTable[5] = temp5;
		}
	}

};

#endif