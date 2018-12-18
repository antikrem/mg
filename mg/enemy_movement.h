/*Control for regular enemy entities*/
#ifndef __ENEMY_MOVEMENT_H__
#define __ENEMY_MOVEMENT_H__

using namespace std;

#include <vector>
#include "cus_structs.h"
#include "listed_entities.h"

enum MovementCommand {
	null,

	accelerateVectorAdd,
	velocityVectorAdd,

	accerlerateMagnitudeForce,
	velocityMagnitudeForce,

	accerlerateMagnitudeAdd,
	velocityMagnitudeAdd,

	velocityAngleAdd,

	timeToDieFaggot
};

/*Structs that contain the data for one ode update*/
struct MovementUpdate {
	MovementCommand command = null;
	int startingFrame = 0;
	int duration = 0;
	float updateValue = 0;
	CUS_Polar updatePolar = { 0,0 }; 
	bool flag = true;
	bool deathFlag = false;

	MovementUpdate(MovementCommand command, int startingFrame, int duration, float updateValue = 0, CUS_Polar updatePolar = { 0,0 }) {
		this->command = command;
		this->startingFrame = startingFrame;
		this->duration = duration;
		this->updateValue = updateValue;
		this->updatePolar = updatePolar;
	}
};

/*Inheritable class that implements movement*/
class MovementControl : public ListedEntity {
private:
	int internalCounter = 0;
	vector<MovementUpdate> movementUpdates;

	bool deathFlag = false;

	void parseMovementUpdates() {
		internalCounter++;

		for (auto const& update : movementUpdates) {
			if (update.startingFrame < internalCounter && internalCounter < (update.duration + update.startingFrame)) {

				switch (update.command) {
				case (null):
					break;

				case (accelerateVectorAdd):
					acceleration += toPoint(update.updatePolar);
					break;

				case (velocityVectorAdd):
					velocity += toPoint(update.updatePolar);
					break;

				case (accerlerateMagnitudeForce):
					auto accelerationTemp = toPolar(acceleration);
					accelerationTemp.magnitude = update.updateValue;
					acceleration = toPoint(accelerationTemp);
					break;

				case (velocityMagnitudeForce):
					auto velocityTemp = toPolar(velocity);
					velocityTemp.magnitude = update.updateValue;
					velocity = toPoint(velocityTemp);
					break;

				case (accerlerateMagnitudeAdd):
					auto accelerationTemp = toPolar(acceleration);
					accelerationTemp.magnitude += update.updateValue;
					acceleration = toPoint(accelerationTemp);
					break;

				case (velocityMagnitudeAdd):
					auto velocityTemp = toPolar(velocity);
					velocityTemp.magnitude += update.updateValue;
					velocity = toPoint(velocityTemp);
					break;

				case (velocityAngleAdd):
					auto velocityTemp = toPolar(velocity);
					velocityTemp.angle += update.updateValue;
					velocity = toPoint(velocityTemp);
					break;

				case (timeToDieFaggot):
					deathFlag = true;
					break;
				}
			}

		}
	}

public:
	MovementControl(CUS_Point position, CUS_Polar velocity = { 0,0 }, CUS_Polar acceleration = { 0,0 }) {
		this->position = position;
		this->velocity = toPoint(velocity);
		this->acceleration = toPoint(acceleration);
	}

	void addAMovementUpdate(MovementCommand command, int startingFrame, int duration, float updateValue = 0, CUS_Polar updatePolar = { 0,0 }) {
		movementUpdates.push_back(MovementUpdate(command, startingFrame, duration, updateValue, updatePolar));
	}

	CUS_Point updateMovement(int cycle) {
		parseMovementUpdates();

		velocity += acceleration;
		position += velocity;
		return position;
	}

	bool getDeathFlag() {
		return deathFlag;
	}
};

#endif