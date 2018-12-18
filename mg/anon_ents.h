#ifndef __ANON_ENTS_H__
#define __ANON_ENTS_H__

#include <vector>
#include <memory>

#include "box_entity.h"
#include "graphics_state.h"
#include "listed_entities.h"

#include <iostream>

class AnonEnt : public ListedEntity {
private:
	string name;
	bool flag = true;
	bool abovePlayerBullets = false;

public:
	AnonEnt(string name, string animationName, CUS_Point position, bool abovePlayerBullets);

	bool getAbovePlayerBullets();

	void updatePosition(CUS_Point position);

	void spinEnt(float angle);

	string getName();
};

/*Inherit to have an interface for anon ents.
anon ents can be updated with updatePosition() and spinEnt() */
class AnonEntInterface {
protected:
	vector< AnonEnt* > toPush;
	vector< shared_ptr<AnonEnt> > pushed;

	void addAnonEnt(string name, string animationName, CUS_Point position, bool abovePlayerBullets = false);

	/*overwrite to update each anon ent every cycle please god do it*/
	virtual void updateAnonEnt();

public:
	AnonEntInterface();

	bool toPushResidual();

	shared_ptr<AnonEnt> getAnonEnt();

	bool getAnonEntExistence(string name);

	shared_ptr<AnonEnt> getAnonEntByName(string name);

	void clearDeadAnonEnts();
};

#endif