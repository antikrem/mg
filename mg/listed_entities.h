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

	/*Generally, should be called every cycle for every listed entity
	If it has not been particulated, nothing will happen*/
	void updateForceApplier();

	/*Can be used when SharedEntityList::listed is true*/
	float list = 0.0;

	/*Call to create and pull a force applier, to be gracefully inserted into particle manager*/
	ForceApplier* particulate(bool important);
};

/*Listed entities can be stored in 2 Shared Entity Lists at the same time
if the same shared pointer is properly copied over
If a death flag is set to true then both lists will clear that object on cleanDeathFlags()
If required, the list can also be rendered with render(..) which will buffer list.
*/
template <class T>
class SharedEntityList : public ThreadSafe {
private:
	vector< shared_ptr<T> > entList;
	
	bool listed = false;

	//Vector of info to render
	vector< RenderInformation > renderBuffer;

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
		lock();
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
		unlock();
		return sharedPush;
	}

	shared_ptr<T> pushObject(shared_ptr<T> toPush) {
		lock();
		if (!listed)
			entList.push_back(toPush);
		else {
			auto s = entList.begin();
			auto e = entList.end();

			entList.insert(
				lower_bound(s, e, toPush, comp),
				toPush);
		}
		unlock();
		return toPush;
	}

	/*NOT SAFE FOR USE, DO NOT USE FOR RENDERING WITHOUT mutex* getLock(..)*/
	vector< shared_ptr<T> > getEntList() {
		return entList;
	}

	/*Clears entities with death flag
	Return value for error checking*/
	int cleanDeathFlags() {
		lock();
		int size = entList.size();
		entList.erase(remove_if(
			entList.begin(), entList.end(),
			[](shared_ptr<T>& x) {
			return !(x->getFlag());
		}), entList.end());
		unlock();
		return (size - entList.size());
	}


	int size() {
		return entList.size();
	}

	//Pushes current renderable objects into render buffer
	void pushToRenderBuffer(int shift) {
		if (tryLock()) {
			renderBuffer.clear();
			for (auto i : entList) {
				renderBuffer.push_back(i->renderCopy(shift));
			}
			unlock();
		}
		else {
			err::logMessage("Render push failed");
		}
		
	}

	shared_ptr<T> getEntPtr(int index) {
		return entList[index];
	}

	//Renders what evers in the renderBufferList, returns number of objects rendered
	int render(GraphicsState* gState, int shift, int darkness, float postScale) {
		lock();
		vector< RenderInformation > secondBuffer = renderBuffer;
		unlock();

		int noBoxes = 0;
		for (auto i : secondBuffer) {
			if (postScale) {
				i.renderSize.w = (int)((float)i.renderSize.w * postScale);
				i.renderSize.h = (int)((float)i.renderSize.h * postScale);
				i.renderSize.x = (int)((float)i.renderSize.x - ((float)i.renderSize.w / 2));
				i.renderSize.y = (int)((float)i.renderSize.y - ((float)i.renderSize.h / 2));
			}

			i.renderSize.x += shift;
			i.renderSize.x += 220;
			i.renderSize.y += 10;
			if not(i.inView) {
				pass;
			}
			else if (i.alpha > 250) {
				SDL_RenderCopyEx(gState->getGRenderer(), i.currentFrame, NULL, &i.renderSize, (double)i.angle, NULL, SDL_FLIP_NONE);
				noBoxes++;
			}
			else if (i.alpha > 0) {
				SDL_SetTextureAlphaMod(i.currentFrame, (int)i.alpha);
				SDL_RenderCopyEx(gState->getGRenderer(), i.currentFrame, NULL, &i.renderSize, (double)i.angle, NULL, SDL_FLIP_NONE);
				SDL_SetTextureAlphaMod(i.currentFrame, 255);
				noBoxes++;
			}
			
			if (i.alpha > 0 && i.inView) {
				//Add shadow
				if (darkness > 0) {
					if (i.alpha > 250) {
						SDL_SetTextureAlphaMod(i.shadowFrame, darkness);
						SDL_RenderCopyEx(gState->getGRenderer(), i.shadowFrame, NULL, &i.renderSize, (double)i.angle, NULL, SDL_FLIP_NONE);
					}
					else if (i.alpha > 0) {
						SDL_SetTextureAlphaMod(i.shadowFrame, (int)((((float)i.alpha) / 255) * (float)darkness));
						SDL_RenderCopyEx(gState->getGRenderer(), i.shadowFrame, NULL, &i.renderSize, (double)i.angle, NULL, SDL_FLIP_NONE);
					}
					noBoxes++;
				}
				//Or add light
				else if (darkness < 0) {
					if (i.alpha > 250) {
						SDL_SetTextureAlphaMod(i.lightFrame, -1 * darkness);
						SDL_RenderCopyEx(gState->getGRenderer(), i.lightFrame, NULL, &i.renderSize, (double)i.angle, NULL, SDL_FLIP_NONE);
					}
					else if (i.alpha > 0) {
						SDL_SetTextureAlphaMod(i.lightFrame, -1 * (int)((((float)i.alpha) / 255) * (float)darkness));
						SDL_RenderCopyEx(gState->getGRenderer(), i.lightFrame, NULL, &i.renderSize, (double)i.angle, NULL, SDL_FLIP_NONE);
					}
					noBoxes++;
				}
			}
		}

		return noBoxes;
	}

	//kills all entities and clears list
	void deepClear() {
		lock();
		for (auto i : entList) {
			i->setFlag(false);
		}
		unlock();
		cleanDeathFlags();
	}


	//Draw hitboxes
	int renderHitBoxes(bool render, GraphicsState* gState, SDL_Texture* hitboxTex, int shift) {
		int drawn = 0;
		if (render) {
			lock();
			SDL_Rect hitRect;
			for (auto i : entList) {
				drawn++;
				hitRect = { (int)(i->getPosition().x - i->getHitBox() + 220 + shift), (int)(i->getPosition().y - i->getHitBox()), 2*(int)(i->getHitBox()), 2*(int)(i->getHitBox()) };
				SDL_RenderCopy(gState->getGRenderer(), hitboxTex, NULL, &hitRect);
			}

			unlock();
		}
		return drawn;
	}
};

#endif