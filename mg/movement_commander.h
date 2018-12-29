#ifndef __MOVEMENT_COMMANDER_H__
#define __MOVEMENT_COMMANDER_H__

#include "box_entity.h"
#include "listed_entities.h"
#include "constants.h"

//If not force, add
struct MovementCommand {

	bool ignoreAngle;
	bool forceAngle;
	bool relativeToPlayer;
	float angle;

	bool ignoreSpeed;
	bool forceSpeed;
	float speed;

	bool fixToMaster = false;

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

	CUS_Point temporaryPosition = { 0,0 };

	map <int, MovementCommand> upcomingUpdates;
	MovementCommand currentUpdate = { true, false, false, 0, true, false, 0 };

public:
	void addMovementCommand(int startingFrame, MovementCommand currentUpdate) {
		upcomingUpdates[startingFrame] = currentUpdate;
	}

	void updateStartingPosition(CUS_Point temporaryPosition) {
		this->temporaryPosition = temporaryPosition;
	}

	void updateMovement(CUS_Point playerPosition, CUS_Point* masterPosition) {
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
		temporaryPosition += velocity;

		if (masterPosition)
			position = temporaryPosition + *masterPosition;
		else
			position = temporaryPosition;
	}
};

#endif