/*Listed entities are an interface for objects regularily contained in lists
It allows easy updates and deletion of objects*/
#ifndef __LISTED_ENTITIES_H__
#define __LISTED_ENTITIES_H__
	
#include "box_entity.h"
#include "particles.h"

#include <memory>
#include <type_traits>

/*Allows entities that are stored in a SharedEntityList
Also has an interface for interacting with particles*/
class ListedEntity : public BoxEntity {
protected:
	bool flag = true;

	float hitBox = 0;

	CUS_Point acceleration = { 0,0 };
	CUS_Point velocity = { 0,0 };

	ForceApplier* forceApplier = NULL;

public:
	~ListedEntity();

	void setHitBox(float hitBox);

	float getHitBox();

	void setFlag(bool flag);

	bool getFlag();

	CUS_Point getPosition();

	/*Generally, should be called every cycle for every listed entity
	If it has not been particulated, nothing will happen*/
	void updateForceApplier();

	/*Can be used when SharedEntityList::listed is true*/
	float list = 0.0;

	/*Call to create and pull a force applier, to be gracefully inserted into particle manager*/
	ForceApplier* particulate(bool important);
};

template <class T>
class SharedEntityList : public ThreadSafe {
private:
	vector< shared_ptr<T> > entList;
	
	bool listed = false;

	static bool comp(shared_ptr<T> a, shared_ptr<T> b) {
		return a->list < b->list;
	}
public:
	SharedEntityList() {
		static_assert(std::is_base_of<ListedEntity, T>::value, "T not derived from ListedEntity");
	}

	//Sets list to true
	void makeListed() {
		this->listed = true;
	}

	//Returns shared pointer to pushed object that can be added to a new shared entity list
	shared_ptr<T> pushObject(T* toPush) {
		auto sharedPush = shared_ptr<T>(toPush);
		if (!listed)
			entList.push_back(sharedPush);
		else {
			auto s = entList.begin();
			auto e = entList.end();

			entList.insert(
				lower_bound(s, e, sharedPush, comp),
				sharedPush);
		}
		return sharedPush;
	}

	shared_ptr<T> pushObject(shared_ptr<T> toPush) {
		if (!listed)
			entList.push_back(toPush);
		else {
			auto s = entList.begin();
			auto e = entList.end();

			entList.insert(
				lower_bound(s, e, toPush, comp),
				toPush);
		}
		return toPush;
	}

	vector< shared_ptr<T> > getEntList() {
		return entList;
	}

	/*Clears entities with death flag
	Return value for error checking*/
	int cleanDeathFlags() {
		int size = entList.size();
		entList.erase(remove_if(
			entList.begin(), entList.end(),
			[](shared_ptr<T>& x) {
			return !(x->getFlag());
		}), entList.end());
		return (size - entList.size());
	}


	int size() {
		return entList.size();
	}

	shared_ptr<T> getEntPtr(int index) {
		return entList[index];
	}
};

#endif