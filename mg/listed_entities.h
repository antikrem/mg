/*Level Class.
To be used with _controller.cpp*/
#ifndef __LISTED_ENTITIES_H__
#define __LISTED_ENTITIES_H__
	
#include "box_entity.h"
#include "error.h"

#include <memory>
#include <mutex>
#include <type_traits>

/*Allows entities that are stored in */
class ListedEntity {
protected:
	bool flag = true;

	float hitBox = 0;

	CUS_Point acceleration = { 0,0 };
	CUS_Point velocity = { 0,0 };
	CUS_Point position = { 0,0 };

public:
	void setHitBox(float hitBox) {
		this->hitBox = hitBox;
	}

	float getHitBox() {
		return hitBox;
	}

	void setFlag(bool flag) {
		this->flag = flag;
	}

	bool getFlag() {
		err::logMessage("oop");
		return flag;
	}
};

template <class T>
class SharedEnityList {
private:
	mutex lock;
	vector< shared_ptr<T> > entList;
public:
	void pushObject(T* toPush) {
		static_assert(std::is_base_of<ListedEntity, T>::value, "T not derived from ListedEntity");
		entList.push_back(shared_ptr<T>(toPush));
	}

	vector< shared_ptr<T> > getEntList() {
		return entList;
	}

	/*Clears entities with death flags
	Thread safe*/
	void cleanDeathFlags() {
		lock.lock();
		entList.erase(remove_if(
			entList.begin(), entList.end(),
			[](shared_ptr<T>& x) {
			return !(x->getFlag());
		}), entList.end());
		lock.unlock();
	}

	int size() {
		return entList.size();
	}

	shared_ptr<T> getEntPtr(int index) {
		return entList[index];
	}
};

#endif