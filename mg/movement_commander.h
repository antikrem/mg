#ifndef __MOVEMENT_COMMANDER_H__
#define __MOVEMENT_COMMANDER_H__

#include "box_entity.h"
#include "listed_entities.h"
#include "constants.h"

enum AngleState {
	verticleAngle,
	velocityAngle

};

//If not force, add
struct MovementCommand {

	bool ignoreAngle;
	bool forceAngle;
	bool relativeToPlayer;
	float angle;

	bool ignoreSpeed;
	bool forceSpeed;
	float speed;


	MovementCommand() {
	}

	MovementCommand(bool ignoreAngle, bool forceAngle, bool relativeToPlayer, float angle, bool ignoreSpeed, bool forceSpeed, float speed) {
		this->ignoreAngle = ignoreAngle;
		this->forceAngle = forceAngle;
		this->relativeToPlayer = relativeToPlayer;
		this->angle = angle;

		this->ignoreSpeed = ignoreSpeed;
		this->forceSpeed = forceSpeed;
		this->speed = speed;
	}
};

class MovementCommander : public ListedEntity {
protected:
	int internalMovementCycle = -1;

	//Position of the first master
	CUS_Point lastMaster = { 0,0 };

	//Direction used for pointing
	AngleState angleState = verticleAngle;

	map <int, MovementCommand> upcomingUpdates;
	MovementCommand currentUpdate = { true, false, false, 0, true, false, 0 };

public:
	void addFirstMaster(CUS_Point firstMaster) {
		this->lastMaster = firstMaster;
	}

	void addMovementCommand(int startingFrame, MovementCommand currentUpdate) {
		upcomingUpdates[startingFrame] = currentUpdate;
	}

	void setStartingPosition(CUS_Point position) {
		this->position = position;
	}

	void setStartingVelocity(CUS_Point velocity) {
		this->velocity = velocity;
		currentUpdate.angle = velocity.toPolarAngle();
		if (angleState == velocityAngle) {
			angle = currentUpdate.angle;
		}
		else if (angleState == verticleAngle) {
			angle = 0;
		}
		
	}

	//updates and returns new position
	CUS_Point updateMovement(CUS_Point playerPosition, CUS_Point* masterPosition) {
		internalMovementCycle++;

		if (upcomingUpdates.count(internalMovementCycle)) {
			if (!upcomingUpdates[internalMovementCycle].ignoreAngle) {
				currentUpdate.relativeToPlayer = upcomingUpdates[internalMovementCycle].relativeToPlayer;

				currentUpdate.ignoreAngle = false;
				currentUpdate.angle = upcomingUpdates[internalMovementCycle].angle;
				currentUpdate.forceAngle = upcomingUpdates[internalMovementCycle].forceAngle;
			}
			if (!upcomingUpdates[internalMovementCycle].ignoreSpeed) {
				currentUpdate.ignoreSpeed = false;
				currentUpdate.speed = upcomingUpdates[internalMovementCycle].speed;
				currentUpdate.forceSpeed = upcomingUpdates[internalMovementCycle].forceSpeed;
			}
		}

		auto tempVelocity = toPolar(velocity);
		if (angleState == velocityAngle) {
			angle = -tempVelocity.angle + 180;
		} 
		else if (angleState == verticleAngle) {
			angle = 0;
		}

		if (!currentUpdate.ignoreSpeed) {
			if (currentUpdate.forceSpeed)
				tempVelocity.magnitude = currentUpdate.speed;
			else
				tempVelocity.magnitude += currentUpdate.speed;
		}
		if (!currentUpdate.ignoreAngle) {
			if (!currentUpdate.relativeToPlayer) {
				if (currentUpdate.forceAngle)
					tempVelocity.angle = currentUpdate.angle;
				else
					tempVelocity.angle += currentUpdate.angle;
			}
			else {
				if (currentUpdate.forceAngle)
					tempVelocity.angle = (position.getAngleToPoint(playerPosition) + currentUpdate.angle);
				else {
					float targetAngle = position.getAngleToPoint(playerPosition);
					tempVelocity.angle += currentUpdate.angle;
					if (abs(currentUpdate.angle) < abs(targetAngle - tempVelocity.angle)) {
						if (targetAngle > currentUpdate.angle) {
							tempVelocity.angle += currentUpdate.angle;
						}
						else if (targetAngle < currentUpdate.angle) {
							tempVelocity.angle -= currentUpdate.angle;
						}
					}

				}
					
			}
			
		}

		velocity = toPoint(tempVelocity);
		position += velocity;

		if (masterPosition) {
			position = position - lastMaster;
			position += *masterPosition;
			lastMaster = *masterPosition;
		}
		return position;
	}
};

#endif