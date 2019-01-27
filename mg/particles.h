#ifndef __PARTICLES_H__
#define __PARTICLES_H__

#include <vector>
#include <cmath>

#include "graphics_state.h"

#include "constants.h"
#include "box_entity.h"
#include "listed_entities.h"
#include "thread_safe.h"
#include "random.h"

#include <iostream>

///Particle constants
//There are two components to interaction between a particle and a force applier
//A particle will be affected by the distance from a force applier
//The force appplier will slightly repel nearby particles, displacing them based on mass and also velocity and acceleration of applier
//Distance to particle inversly scale the strength of proximity displacement
//Angle of displacement will be in the direction of the particle from applier
//This is the base distance of how far particles are effected
#define PROXIMITY_DISPLACEMENT F(20)
//Maximum momentum imparted at centerpoint from proximity displacement
#define DISTANCE_CONSTANT F(1)
//Additional scalar for velocity, which is multiplied by magnitude of velocity and added
#define VELOCITY_CONSTANT F(2)
//Another scalar for acceleration, needs to be big, BIG
#define ACCELERATION_CONSTANT F(5)
//The strength of momentum transfered frmom proximity scales from distance
//The displacement is calculated as:
//displacement = (float f = mass * ( (-1*DISTANCE_CONSTANT/PROXIMITY_DISPLACEMENT) distance + DISTANCE_CONSTANT + VELOCITY_CONSTANT*vel) > 0 ? f : 0;

//The number of cycles a particles frame of animation is shown
#define ANIMATION_FRAMES 100

//When wind affects particles it is scaled by this constant
#define WIND_COEFFICIENT 0.2f

//different types of particles
enum ParticleType {
	particle_gold
};

//Represents one particle, requires manager to store textures
struct Particle {
	ParticleType particleType;
	//The identifier of the texture
	int textureIdentifier;
	int cycle = 0;

	//Lock particle for value manipulation
	mutex particleLock;

	CUS_Point position;
	CUS_Point velocity;
	//mass of particle
	float mass = F(1);

	//clear when true
	bool deathFlag = false;

	Particle(ParticleType particleType, CUS_Point position, CUS_Point velocity) {
		this->particleType = particleType;
		this->position = position;
		this->velocity = velocity;
	}

	void updateParticle(CUS_Point totalWind) {
		cycle++;
		velocity += (totalWind * WIND_COEFFICIENT);
		position += velocity * mass;
		if (position.x > WORK_SPACE_X + 10 ||
			position.x < -10 ||
			position.y > WORK_SPACE_Y + 10 ||
			position.y < -10
			) {
			deathFlag = true;
		}
	}
};

//Used in the particle render buffer
struct ParticleRender {
	ParticleType particleType;
	//The identifier of the texture
	int textureIdentifier;
	CUS_Point position;

	ParticleRender(Particle* particle) {
		this->particleType = particle->particleType;
		this->textureIdentifier = particle->textureIdentifier;
		this->position = particle->position;
	}
};

struct ParticleTemplate {
public:
	//Each template needs to load a spritesheet before use
	SDL_Texture* particleSpriteSheet = NULL;
	//Each template needs to load a spritesheet before use
	SDL_Texture* blackParticleSpriteSheet = NULL;
	//Size of first src rectangle, size x horrizontally times frame# for next frame
	SDL_Rect firstFrame;
	//number of frames for this template
	int numberOfFrames;

	SDL_Texture* pTexFromPath(string loc, GraphicsState* graphicsState, int numberOfFrames) {
		firstFrame.x = 0;
		firstFrame.y = 0;

		SDL_Surface *temporarySurfaceStorage = IMG_Load(loc.c_str());
		if (!temporarySurfaceStorage)
			err::logMessage("ERROR: Couldn't load " + loc);
		firstFrame.h = temporarySurfaceStorage->h;
		firstFrame.w = temporarySurfaceStorage->w / numberOfFrames;

		auto tex = SDL_CreateTextureFromSurface(graphicsState->getGRenderer(), temporarySurfaceStorage);
		if (!tex)
			err::logMessage("ERROR: Couldn't convert from " + loc);
		SDL_FreeSurface(temporarySurfaceStorage);
		return tex;
	}

	SDL_Texture* bTexFromPath(string loc, GraphicsState* graphicsState) {
		SDL_Surface *temporarySurfaceStorage = IMG_Load(loc.c_str());
		if (!temporarySurfaceStorage)
			err::logMessage("ERROR: Couldn't load " + loc);
		SDL_PixelFormat *fmt = temporarySurfaceStorage->format;
		if (fmt->BitsPerPixel != 32)
			err::logMessage("Format is borked");
		Uint32 *pixel = (Uint32 *)temporarySurfaceStorage->pixels;
		Uint8 r, g, b, a;

		SDL_LockSurface(temporarySurfaceStorage);
		for (int y = 0; y < temporarySurfaceStorage->h; y++) {
			for (int x = 0; x < temporarySurfaceStorage->w; x++) {
				pixel = (Uint32 *)temporarySurfaceStorage->pixels;
				pixel += ((y*temporarySurfaceStorage->w) + x);
				SDL_GetRGBA(*pixel, fmt, &r, &g, &b, &a);
				*pixel = SDL_MapRGBA(fmt, 255, 255, 255, a);

			}
		}
		SDL_UnlockSurface(temporarySurfaceStorage);
		auto tex = SDL_CreateTextureFromSurface(graphicsState->getGRenderer(), temporarySurfaceStorage);
		if (!tex)
			err::logMessage("ERROR: Couldn't convert from " + loc);
		SDL_FreeSurface(temporarySurfaceStorage);
		return tex;
	}

	SDL_Rect getCurrentFrameRect(int frame) {
		SDL_Rect newFrame = firstFrame;
		newFrame.x += frame * newFrame.w;
		return newFrame;
	}

	SDL_Rect getRenderRect(CUS_Point position) {
		return { (int)(position.x - firstFrame.w/2), (int)(position.y - firstFrame.h / 2), firstFrame.w, firstFrame.h };
	}

	ParticleTemplate(string position, GraphicsState* graphicsState, int numberOfFrames) {
		this->numberOfFrames = numberOfFrames;
		particleSpriteSheet = pTexFromPath(position, graphicsState, numberOfFrames);
		blackParticleSpriteSheet = bTexFromPath(position, graphicsState);
	}
};

//Each force applier has a list of weak pointers 
//Each object that can apply force stores refernce to this object
class ForceApplier : public ThreadSafe {
	//Vector of local particles
	vector<weak_ptr<Particle>> localParticles;

	//The master of the force applier will set this to true right before it dies
	//At which point, the ParticleManager can delete and reclaim memory
	bool toClear = false;

	CUS_Point position = { 0, 0 };
	CUS_Point velocity = { 0, 0 };
	CUS_Point acceleration = { 0, 0 };

	atomic<float> velocityMagnitude = 0;
	atomic<float> accelerationMagnitude = 0;

	//spike values that are used for calculating the current cycle of calculation
	atomic<float> spikeVelocity = 0;
	atomic<float> spikeAcceleration = 0;
	//max values used in the calculation of max affect range
	atomic<float> maxVelocity = 0;
	atomic<float> maxAcceleration = 0;
	//Maximum range that particles are added to this applier
	float maxAffectRange = 0;

	float maximumRange = 0;

	//mass of applier
	float mass = F(1);

public:
	void setFlagFalse() {
		toClear = true;
	}

	bool getFlag() {
		return toClear;
	}

	float getRange() {
		return maxAffectRange;
	}

	//Recalculates the maximum range that this force applier can push to
	void recalculateMaximumRange() {
		maximumRange = (PROXIMITY_DISPLACEMENT / DISTANCE_CONSTANT)*(VELOCITY_CONSTANT * maxVelocity + ACCELERATION_CONSTANT * maxAcceleration);
	}

	void updatePositionAndVelocity(CUS_Point position, CUS_Point velocity) {
		this->position = position;

		acceleration = velocity - this->velocity;
		accelerationMagnitude = acceleration.toPolarMagnitude();
		if (accelerationMagnitude > maxAcceleration)
			maxAcceleration.store(accelerationMagnitude.load());
		if (accelerationMagnitude > spikeAcceleration)
			spikeAcceleration.store(accelerationMagnitude.load());

		this->velocity = velocity;
		velocityMagnitude = velocity.toPolarMagnitude();
		if (velocityMagnitude > maxVelocity)
			maxVelocity.store(velocityMagnitude.load());
		if (velocityMagnitude > spikeVelocity)
			spikeVelocity.store(velocityMagnitude.load());

		//cout << "mag: " << accelerationMagnitude << " " << velocityMagnitude << endl;
		//cout << "max: " << maxAcceleration << " " << maxVelocity << endl;
		//cout << "spike: " << spikeAcceleration << " " << spikeVelocity << endl;
		
	}

	//This updates the velocity of the particle based on force applier interaction
	void update() {
		//add displacement force
		for (auto particle : localParticles) {
			if (auto p = particle.lock()) {
				float distance = (p->position - position).toPolarMagnitude();
				float f;
				auto displacement = (
					f = mass * ((-1 * DISTANCE_CONSTANT / PROXIMITY_DISPLACEMENT)* distance + DISTANCE_CONSTANT + VELOCITY_CONSTANT * velocity.toPolarMagnitude() + ACCELERATION_CONSTANT * accelerationMagnitude
						) > 0 ? f : 0);
			}
		}
	}

	void clearNeighbourhood() {
		localParticles.clear();
	}

	void addAParticle(shared_ptr<Particle> particle) {
		localParticles.push_back(particle);
	}

	CUS_Point getPosition() {
		return position;
	}

	float getMass() {
		return mass;
	}
};

class ParticleManager : public ThreadSafe {
	int internalCounter = -1;
	//Frames between every allocation cycle
	int neighbourhoodReallocationRate = 10;

	//Number of neighbourhood groups, every reallocation cycle, only one of these gorups will be updated
	int neighbourhoodGroupCount = 1;

	//last heighbourhood group neightbourhood was allocated on last allocation cycle
	int neighbourhoodGroupIncrement = 0;
	//last neightbourhhood allocated a force eapplier 
	int neightbourhoodGroupAllocator = 0;
	//last neighbourhood group updated
	int neightbourhoodGroupUpdate = 0;

	GraphicsState* graphicsState = NULL;
	//For rendering purposes
	map<ParticleType, vector<ParticleTemplate>> particleTemplates;
	//Master list
	vector<shared_ptr<Particle>> mainParticleList;
	//Lock when accessing particle list
	mutex mainParticleLock;
	//forceApplierMasterList[i] is the i-th neighbourhood
	vector<vector<ForceApplier*>> forceApplierMasterList;

	/*Sets up particle manager to use this many groups
	Will reallocate existing force appliers
	*/
	void setUpGroups(unsigned int numberOfGroups) {
		if (numberOfGroups != forceApplierMasterList.size()) {
			vector<ForceApplier*> toPush;
			for (auto i : forceApplierMasterList) {
				toPush.insert(toPush.end(), i.begin(), i.end());
			}
			forceApplierMasterList.clear();
			int sizeOfGroup = toPush.size() / numberOfGroups;
			for (unsigned int i = 0; i < numberOfGroups; i++) {
				vector<ForceApplier*> to;
				forceApplierMasterList.push_back(to);
			}
			int allocator = 0;
			for (unsigned int i = 0; i < toPush.size(); i++) {
				allocator = (++allocator) % numberOfGroups;
				forceApplierMasterList[allocator].push_back(toPush[i]);
			}
		}
		neighbourhoodGroupCount = numberOfGroups;
	}

	//Loads particle templates
	void loadParticles() {
		//gold particles
		particleTemplates[particle_gold].push_back(ParticleTemplate("assets\\particles\\gold.bmp", graphicsState, 6));
		

	}
public:
	ParticleManager(GraphicsState* graphicsState) {
		this->graphicsState = graphicsState;
		setUpGroups(2);
		//Load particles, which are tiny
		loadParticles();
	}

	void addForceApplier(ForceApplier* forceApplier) {
		neightbourhoodGroupAllocator = (++neightbourhoodGroupAllocator) % neighbourhoodGroupCount;
		forceApplierMasterList[neightbourhoodGroupAllocator].push_back(forceApplier);
	}

	//The cycle will be prematurly ended if it takes too long
	void grandParticleUpdate(chrono::high_resolution_clock::time_point startCyclePoint, CUS_Point totalWind) {
		internalCounter++;

		//Reallocate every neighbourhood every allocation cycles
		if (internalCounter%neighbourhoodReallocationRate && forceApplierMasterList.size()) {
			neighbourhoodGroupIncrement = (++neighbourhoodGroupIncrement) % neighbourhoodGroupCount;
			//For each applier in this neighbourhood group, clear its neighbourhood
			//and re allocate it a neighbourhood
			for (auto applier : forceApplierMasterList[neighbourhoodGroupIncrement]) {
				applier->clearNeighbourhood();
				applier->recalculateMaximumRange();
				for (auto particle : mainParticleList) {
					if (particle->position.getDistanceToPoint(applier->getPosition()) < applier->getRange()) {
						applier->addAParticle(particle);
					}
				}
			}
		}
		//On other cycles, calculate interaction between force applier and particles
		else if (forceApplierMasterList.size()) {
			neightbourhoodGroupUpdate = (++neightbourhoodGroupUpdate) % neighbourhoodGroupCount;
			//Check if the force applier is still valid, clear dead force appliers
			auto it = forceApplierMasterList[neighbourhoodGroupIncrement].begin();
			while (it != forceApplierMasterList[neighbourhoodGroupIncrement].end()) {
				if ((*it)->getFlag()) {
					it = forceApplierMasterList[neighbourhoodGroupIncrement].erase(it);
				}
				else {
					it++;
				}
			}
		}

		//Update position of particles
		mainParticleLock.lock();
		auto it = mainParticleList.begin();
		bool condition;
		while (it != mainParticleList.end()) {
			(*it)->particleLock.lock();
			(*it)->updateParticle(totalWind);
			condition = (((*it)->cycle / ANIMATION_FRAMES) >= particleTemplates[(*it)->particleType][(*it)->textureIdentifier].numberOfFrames || (*it)->deathFlag);
			(*it)->particleLock.unlock();
			if (condition) {
				it = mainParticleList.erase(it);
			}
			else {
				it++;
			}
		}
		mainParticleLock.unlock();
	}

	int getParticleCount() {
		return (int)mainParticleList.size();
	}

	int getApplierCount() {
		int runningTotal = 0;
		for (auto i : forceApplierMasterList) {
			runningTotal += i.size();
		}
		return runningTotal;
	}

	void renderParticles(int shift) {
		mainParticleLock.lock();
		SDL_Rect src;
		SDL_Rect dst;
		int frame;
		for (auto particle : mainParticleList) {
			particle->particleLock.lock();
			frame = particle->cycle / ANIMATION_FRAMES;
			if (frame < particleTemplates[particle->particleType][particle->textureIdentifier].numberOfFrames) {
				src = particleTemplates[particle->particleType][particle->textureIdentifier].getCurrentFrameRect(frame);
				dst = particleTemplates[particle->particleType][particle->textureIdentifier].getRenderRect(particle->position);
				dst.x += shift;

				SDL_RenderCopy(graphicsState->getGRenderer(),
					particleTemplates[particle->particleType][particle->textureIdentifier].particleSpriteSheet,
					&src,
					&dst);
			}
			particle->particleLock.unlock();
		}
		mainParticleLock.unlock();
	}

	/*Spawns a particle, which will start interacting*/
	void spawnParticle(CUS_Point spawnPosition, CUS_Point spawnVelocity, ParticleType particleType, float mass = 1) {
		auto particle = new Particle(particleType, spawnPosition, spawnVelocity);
		particle->textureIdentifier = random::randomInt(0, particleTemplates[particleType].size() - 1);
		particle->mass = mass;
		mainParticleLock.lock();
		mainParticleList.push_back(shared_ptr<Particle>(particle));
		mainParticleLock.unlock();
	}

	int getNumberOfGroups() {
		return neighbourhoodGroupCount;
	}
};

#endif