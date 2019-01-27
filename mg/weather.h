/*A lower overhead system for weather, biggest fucking mistke I have written in my pathetic life
Also can do lightning, which i guess is cool*/
#ifndef __WEATHER_H__
#define __WEATHER_H__

#define HORIZONTAL_SKEW F(0.1)
#define ABOVE_SCALE F(1.5)
#define BELOW_SCALE F(0.7)

#define WEATHER_DARK_TRANSITION F(0.004)

#include "sdl.h"
#include "sdl_image.h"
#include "cus_structs.h"
#include "graphics_state.h"
#include "constants.h"
#include "random.h"
#include "light_master.h"

#include <vector>

#include <iostream>

using namespace std;

enum WeatherType {
	noweather,
	rain,
	fog
};

class WeatherEffect {
public:
	CUS_Point position;
	//How much to the left/right the bullet will tend to after a cycle
	float verticalskew;
	float angle = 0;
	float scale = 1;
	int frameNumber = 0;
};

class WeatherEffectManager {
private:
	GraphicsState* graphicsState = NULL;

	WeatherType weatherType = noweather;

	SDL_Texture* masterRainTexture[2] = { NULL,NULL };
	SDL_Texture* masterRainTextureBlack[2] = { NULL,NULL };
	SDL_Texture* masterRainTextureWhite[2] = { NULL,NULL };
	SDL_Point baseSize;
	SDL_Renderer* renderer = NULL;

	vector<WeatherEffect> weatherEffectsAbove;
	vector<WeatherEffect> weatherEffectsMiddle;
	vector<WeatherEffect> weatherEffectsBelow;

	//Cumalative drag, which is used to add new tiles
	CUS_Point totalDriftAbove = { 0,0 };
	CUS_Point totalDriftMiddle = { 0,0 };
	CUS_Point totalDriftBelow = { 0,0 };

public:
	void startRain() {
		weatherType = rain;

		playLoopedSound("rain", SCWeatherBackground);

		WeatherEffect effect;
		for (float i = F(-baseSize.x / 2); i < F(WORK_SPACE_X + baseSize.x / 2); i += F(ABOVE_SCALE * baseSize.x / 2 + random::randomFloat(-20, 20))) {
			for (float j = F(-baseSize.y / 2); j < F(WORK_SPACE_Y + baseSize.y / 2); j += F(ABOVE_SCALE * baseSize.y / 2 + random::randomFloat(-20, 20))) {
				effect.position = { i, j };
				effect.scale = ABOVE_SCALE * random::randomFloat(F(0.95), F(1));
				effect.verticalskew = ABOVE_SCALE * random::randomFloat(-1 * HORIZONTAL_SKEW, HORIZONTAL_SKEW);
				effect.frameNumber = random::randomInt(0, 1);
				weatherEffectsAbove.push_back(effect);
			}
		}

		for (float i = F(-baseSize.x / 2); i < F(WORK_SPACE_X + baseSize.x / 2); i += F(baseSize.x / 2 + random::randomFloat(-20, 20))) {
			for (float j = F(-baseSize.y / 2); j < F(WORK_SPACE_Y + baseSize.y / 2); j += F(baseSize.y / 2 + random::randomFloat(-20, 20))) {
				effect.position = { i, j };
				effect.scale = random::randomFloat(F(0.95), F(1));
				effect.verticalskew = random::randomFloat(-1 * HORIZONTAL_SKEW, HORIZONTAL_SKEW);
				effect.frameNumber = random::randomInt(0, 1);
				weatherEffectsMiddle.push_back(effect);
			}
		}

		for (float i = F(-baseSize.x / 2); i < F(WORK_SPACE_X + baseSize.x / 2); i += F(BELOW_SCALE * baseSize.x / 2 + random::randomFloat(0, 50))) {
			for (float j = F(-baseSize.y / 2); j < F(WORK_SPACE_Y + baseSize.y / 2); j += F(BELOW_SCALE * baseSize.y / 2 + random::randomFloat(0, 50))) {
				effect.position = { i, j };
				effect.scale = BELOW_SCALE * random::randomFloat(F(0.95), F(1));
				effect.verticalskew = BELOW_SCALE * random::randomFloat(-1 * HORIZONTAL_SKEW, HORIZONTAL_SKEW);
				effect.frameNumber = random::randomInt(0, 1);
				weatherEffectsBelow.push_back(effect);
			}
		}
	}

	WeatherEffectManager(GraphicsState* graphicsState, WeatherType weatherType) {
		this->graphicsState = graphicsState;
		this->weatherType = weatherType;
		renderer = graphicsState->getGRenderer();

		SDL_Surface *temporarySurfaceStorage;
		SDL_PixelFormat *fmt;

		for (int i = 0; i < 2; i++) {
			string pos = "assets\\weather\\rain" + to_string(i) + ".png";
			temporarySurfaceStorage = IMG_Load(pos.c_str());
			masterRainTexture[i] = SDL_CreateTextureFromSurface(renderer, temporarySurfaceStorage);
			baseSize.x = temporarySurfaceStorage->w;
			baseSize.y = temporarySurfaceStorage->h;

			fmt = temporarySurfaceStorage->format;
			if (fmt->BitsPerPixel != 32)
				err::logMessage("Format is borked");

			Uint32 *pixel = (Uint32 *)temporarySurfaceStorage->pixels;
			Uint8 r, g, b, a;

			//Generates dark map
			SDL_LockSurface(temporarySurfaceStorage);
			for (int y = 0; y < temporarySurfaceStorage->h; y++) {
				for (int x = 0; x < temporarySurfaceStorage->w; x++) {
					pixel = (Uint32 *)temporarySurfaceStorage->pixels;
					pixel += ((y*temporarySurfaceStorage->w) + x);
					SDL_GetRGBA(*pixel, fmt, &r, &g, &b, &a);
					*pixel = SDL_MapRGBA(fmt, 0, 0, 0, a);

				}
			}
			SDL_UnlockSurface(temporarySurfaceStorage);

			masterRainTextureBlack[i] = SDL_CreateTextureFromSurface(renderer, temporarySurfaceStorage);

			//Generates white map
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

			masterRainTextureWhite[i] = SDL_CreateTextureFromSurface(renderer, temporarySurfaceStorage);

			SDL_FreeSurface(temporarySurfaceStorage);
		}		

		if (weatherType == rain) {
			startRain();
		}
	}

	~WeatherEffectManager() {
		if (masterRainTexture[0]) {
			SDL_DestroyTexture(masterRainTexture[0]);
			masterRainTexture[0] = NULL;
		}
		if (masterRainTexture[1]) {
			SDL_DestroyTexture(masterRainTexture[1]);
			masterRainTexture[1] = NULL;
		}
	}

	void update(CUS_Polar windspeed, LightMaster* lightMaster, float velocityShift) {
		if (weatherType == noweather)
			lightMaster->setLightMode(LMNormal);
		if (weatherType == rain)
			lightMaster->setLightMode(LMRain);

		//Delete any weather effects if the current setting doesn't allow it
		if (graphicsState->getWeatherVolume() < medium)
			weatherEffectsAbove.clear();
		if (graphicsState->getWeatherVolume() < high)
			weatherEffectsMiddle.clear();
		if (graphicsState->getWeatherVolume() < ultra)
			weatherEffectsBelow.clear();

		vector<WeatherEffect>::iterator it;
		//FOR ABOVE
		if (graphicsState->getWeatherVolume() > low) {
			CUS_Point newPos;
			for (unsigned int i = 0; i < weatherEffectsAbove.size(); i++) {
				CUS_Point oldPos = weatherEffectsAbove[i].position;
				weatherEffectsAbove[i].position.x += ABOVE_SCALE * windspeed.toPoint().x;
				weatherEffectsAbove[i].position.y += ABOVE_SCALE * windspeed.toPoint().y;
				weatherEffectsAbove[i].position.y += ABOVE_SCALE * 10;
				weatherEffectsAbove[i].position.x += ABOVE_SCALE * weatherEffectsAbove[i].verticalskew;
				newPos = weatherEffectsAbove[i].position;
				newPos.x += ABOVE_SCALE * velocityShift;
				weatherEffectsAbove[i].angle = -1 * newPos.getAngleToPoint(oldPos);
			}

			totalDriftAbove += toPoint(windspeed);
			totalDriftAbove.y += 10;

			if (weatherType == rain) {
				//If drifted out too far to the right
				if (totalDriftAbove.x > ABOVE_SCALE * baseSize.x / 2) {
					totalDriftAbove.x = 0;
					WeatherEffect effect;
					for (float j = F(-baseSize.y / 2); j < (WORK_SPACE_Y + baseSize.y / 2); j += (ABOVE_SCALE * baseSize.y / 2 + random::randomFloat(-20, 20))) {
						effect.position = { F(ABOVE_SCALE * -baseSize.x / 2), j };
						effect.verticalskew = ABOVE_SCALE * random::randomFloat(-1 * HORIZONTAL_SKEW, HORIZONTAL_SKEW);
						effect.scale = ABOVE_SCALE * random::randomFloat(F(0.95), F(1));
						weatherEffectsAbove.push_back(effect);
					}
				}

				//If drifted out too far to the left
				if (totalDriftAbove.x < ABOVE_SCALE * -baseSize.x / 2) {
					totalDriftAbove.x = 0;
					WeatherEffect effect;
					for (float j = F(-baseSize.y / 2); j < (WORK_SPACE_Y + baseSize.y / 2); j += (ABOVE_SCALE * baseSize.y / 2 + random::randomFloat(-20, 20))) {
						effect.position = { F(WORK_SPACE_X + ABOVE_SCALE * baseSize.x / 2), j };
						effect.verticalskew = ABOVE_SCALE * random::randomFloat(-1 * HORIZONTAL_SKEW, HORIZONTAL_SKEW);
						effect.scale = ABOVE_SCALE * random::randomFloat(F(0.95), F(1));
						weatherEffectsAbove.push_back(effect);
					}
				}

				//If drifted out too far down
				if (totalDriftAbove.y > ABOVE_SCALE * baseSize.y / 2) {
					totalDriftAbove.y = 0;
					WeatherEffect effect;
					for (float i = F(-baseSize.x / 2) + random::randomFloat(F(0), F(baseSize.x / 2)); i < (WORK_SPACE_X + baseSize.x / 2); i += (ABOVE_SCALE * baseSize.x / 2 + random::randomFloat(-20, 20))) {
						effect.position = { i, F((ABOVE_SCALE * -baseSize.y) / 1.2f) };
						effect.verticalskew = ABOVE_SCALE * random::randomFloat(-1 * HORIZONTAL_SKEW, HORIZONTAL_SKEW);
						effect.scale = ABOVE_SCALE * random::randomFloat(F(0.95), F(1));
						weatherEffectsAbove.push_back(effect);
					}
				}

				//If drifted out too far up
				if (totalDriftAbove.y < ABOVE_SCALE * -baseSize.y / 2) {
					totalDriftAbove.y = 0;
					WeatherEffect effect;
					for (float i = F(-baseSize.x) / 2; i < (WORK_SPACE_X + baseSize.x / 2); i += (ABOVE_SCALE * baseSize.x / 2 + random::randomFloat(-20, 20))) {
						effect.position = { i, F(WORK_SPACE_Y + ABOVE_SCALE * -baseSize.y / 2) };
						effect.verticalskew = ABOVE_SCALE * random::randomFloat(-1 * HORIZONTAL_SKEW, HORIZONTAL_SKEW);
						effect.scale = ABOVE_SCALE * random::randomFloat(F(0.95), F(1));
						weatherEffectsAbove.push_back(effect);
					}
				}
			}

			//Clear passed effects
			it = weatherEffectsAbove.begin();
			while (it != weatherEffectsAbove.end()) {
				if (it->position.x < ABOVE_SCALE * -baseSize.x ||
					it->position.x >(ABOVE_SCALE * baseSize.x + WORK_SPACE_X) ||
					it->position.y < ABOVE_SCALE * -baseSize.y ||
					it->position.y >(ABOVE_SCALE * baseSize.y + WORK_SPACE_Y)
					) {
					it = weatherEffectsAbove.erase(it);
				}
				else {
					it++;
				}
			}
		}

		//FOR MIDDLE
		if (graphicsState->getWeatherVolume() > medium) {
			CUS_Point newPos;
			for (unsigned int i = 0; i < weatherEffectsMiddle.size(); i++) {
				CUS_Point oldPos = weatherEffectsMiddle[i].position;
				weatherEffectsMiddle[i].position.x += windspeed.toPoint().x;
				weatherEffectsMiddle[i].position.y += windspeed.toPoint().y;
				weatherEffectsMiddle[i].position.y += 10;
				weatherEffectsMiddle[i].position.x += weatherEffectsMiddle[i].verticalskew;
				newPos = weatherEffectsMiddle[i].position;
				newPos.x += velocityShift;
				weatherEffectsMiddle[i].angle = -1 * newPos.getAngleToPoint(oldPos);
			}

			totalDriftMiddle += toPoint(windspeed);
			totalDriftMiddle.y += 10;

			if (weatherType == rain) {
				//If drifted out too far to the right
				if (totalDriftMiddle.x > baseSize.x / 2) {
					totalDriftMiddle.x = 0;
					WeatherEffect effect;
					for (float j = F(-baseSize.y / 2); j < (WORK_SPACE_Y + baseSize.y / 2); j += (baseSize.y / 2 + random::randomFloat(-20, 20))) {
						effect.position = { F(-baseSize.x / 2), j };
						effect.verticalskew = random::randomFloat(-1 * HORIZONTAL_SKEW, HORIZONTAL_SKEW);
						weatherEffectsMiddle.push_back(effect);
					}
				}

				//If drifted out too far to the left
				if (totalDriftMiddle.x < -baseSize.x / 2) {
					totalDriftMiddle.x = 0;
					WeatherEffect effect;
					for (float j = F(-baseSize.y / 2); j < (WORK_SPACE_Y + baseSize.y / 2); j += (baseSize.y / 2 + random::randomFloat(-20, 20))) {
						effect.position = { F(WORK_SPACE_X + baseSize.x / 2), j };
						effect.verticalskew = random::randomFloat(-1 * HORIZONTAL_SKEW, HORIZONTAL_SKEW);
						weatherEffectsMiddle.push_back(effect);
					}
				}

				//If drifted out too far down
				if (totalDriftMiddle.y > baseSize.y / 2) {
					totalDriftMiddle.y = 0;
					WeatherEffect effect;
					for (float i = F(-baseSize.x / 2) + random::randomFloat(F(0), F(baseSize.x / 2)); i < (WORK_SPACE_X + baseSize.x / 2); i += (baseSize.x / 2 + random::randomFloat(-20, 20))) {
						effect.position = { i, F(-baseSize.y / 2) };
						effect.verticalskew = random::randomFloat(-1 * HORIZONTAL_SKEW, HORIZONTAL_SKEW);
						weatherEffectsMiddle.push_back(effect);
					}
				}

				//If drifted out too far up
				if (totalDriftMiddle.y < -baseSize.y / 2) {
					totalDriftMiddle.y = 0;
					WeatherEffect effect;
					for (float i = F(-baseSize.x) / 2; i < (WORK_SPACE_X + baseSize.x / 2); i += (baseSize.x / 2 + random::randomFloat(-20, 20))) {
						effect.position = { i, F(WORK_SPACE_Y + -baseSize.y / 2) };
						effect.verticalskew = random::randomFloat(-1 * HORIZONTAL_SKEW, HORIZONTAL_SKEW);
						weatherEffectsMiddle.push_back(effect);
					}
				}
			}

			//Clear passed effects
			it = weatherEffectsMiddle.begin();
			while (it != weatherEffectsMiddle.end()) {
				if (it->position.x < -baseSize.x ||
					it->position.x >(baseSize.x + WORK_SPACE_X) ||
					it->position.y < -baseSize.y ||
					it->position.y >(baseSize.y + WORK_SPACE_Y)
					) {
					it = weatherEffectsMiddle.erase(it);
				}
				else {
					it++;
				}
			}
		}

		//FOR BELOW
		if (graphicsState->getWeatherVolume() > high) {
			CUS_Point newPos;
			for (unsigned int i = 0; i < weatherEffectsBelow.size(); i++) {
				CUS_Point oldPos = weatherEffectsBelow[i].position;
				weatherEffectsBelow[i].position.x += BELOW_SCALE * windspeed.toPoint().x;
				weatherEffectsBelow[i].position.y += BELOW_SCALE * windspeed.toPoint().y;
				weatherEffectsBelow[i].position.y += BELOW_SCALE * 10;
				weatherEffectsBelow[i].position.x += BELOW_SCALE * weatherEffectsBelow[i].verticalskew;
				newPos = weatherEffectsBelow[i].position;
				newPos.x += BELOW_SCALE * velocityShift;
				weatherEffectsBelow[i].angle = -1 * newPos.getAngleToPoint(oldPos);
			}

			totalDriftBelow += toPoint(windspeed);
			totalDriftBelow.y += 10;

			if (weatherType == rain) {
				//If drifted out too far to the right
				if (totalDriftBelow.x > BELOW_SCALE * baseSize.x / 2) {
					totalDriftBelow.x = 0;
					WeatherEffect effect;
					for (float j = F(-baseSize.y / 2); j < (WORK_SPACE_Y + baseSize.y / 2); j += (BELOW_SCALE * baseSize.y / 2 + random::randomFloat(-20, 20))) {
						effect.position = { F(BELOW_SCALE * -baseSize.x / 2), j };
						effect.verticalskew = BELOW_SCALE * random::randomFloat(-1 * HORIZONTAL_SKEW, HORIZONTAL_SKEW);
						effect.scale = BELOW_SCALE * random::randomFloat(F(0.95), F(1));
						weatherEffectsBelow.push_back(effect);
					}
				}

				//If drifted out too far to the left
				if (totalDriftBelow.x < BELOW_SCALE * -baseSize.x / 2) {
					totalDriftBelow.x = 0;
					WeatherEffect effect;
					for (float j = F(-baseSize.y / 2); j < (WORK_SPACE_Y + baseSize.y / 2); j += (BELOW_SCALE * baseSize.y / 2 + random::randomFloat(-20, 20))) {
						effect.position = { F(WORK_SPACE_X + BELOW_SCALE * baseSize.x / 2), j };
						effect.verticalskew = BELOW_SCALE * random::randomFloat(-1 * HORIZONTAL_SKEW, HORIZONTAL_SKEW);
						effect.scale = BELOW_SCALE * random::randomFloat(F(0.95), F(1));
						weatherEffectsBelow.push_back(effect);
					}
				}

				//If drifted out too far down
				if (totalDriftBelow.y > BELOW_SCALE * baseSize.y / 2) {
					totalDriftBelow.y = 0;
					WeatherEffect effect;
					for (float i = F(-baseSize.x / 2) + random::randomFloat(F(0), F(baseSize.x / 2)); i < (WORK_SPACE_X + baseSize.x / 2); i += (BELOW_SCALE * baseSize.x / 2 + random::randomFloat(-20, 20))) {
						effect.position = { i, F(BELOW_SCALE * -baseSize.y / 2) };
						effect.verticalskew = BELOW_SCALE * random::randomFloat(-1 * HORIZONTAL_SKEW, HORIZONTAL_SKEW);
						effect.scale = BELOW_SCALE * random::randomFloat(F(0.95), F(1));
						weatherEffectsBelow.push_back(effect);
					}
				}

				//If drifted out too far up
				if (totalDriftBelow.y < BELOW_SCALE * -baseSize.y / 2) {
					totalDriftBelow.y = 0;
					WeatherEffect effect;
					for (float i = F(-baseSize.x) / 2; i < (WORK_SPACE_X + baseSize.x / 2); i += (BELOW_SCALE * baseSize.x / 2 + random::randomFloat(-20, 20))) {
						effect.position = { i, F(WORK_SPACE_Y + BELOW_SCALE * -baseSize.y / 2) };
						effect.verticalskew = BELOW_SCALE * random::randomFloat(-1 * HORIZONTAL_SKEW, HORIZONTAL_SKEW);
						effect.scale = BELOW_SCALE * random::randomFloat(F(0.95), F(1));
						weatherEffectsBelow.push_back(effect);
					}
				}
			}

			//Clear passed effects
			it = weatherEffectsBelow.begin();
			while (it != weatherEffectsBelow.end()) {
				if (it->position.x < BELOW_SCALE * -baseSize.x ||
					it->position.x >(BELOW_SCALE * baseSize.x + WORK_SPACE_X) ||
					it->position.y < BELOW_SCALE * -baseSize.y ||
					it->position.y >(BELOW_SCALE * baseSize.y + WORK_SPACE_Y)
					) {
					it = weatherEffectsBelow.erase(it);
				}
				else {
					it++;
				}
			}
		}

	}

	void drawAboveWeather(LightMaster* lm, int shift, int darkness) {
		SDL_Rect drawPos;
		drawPos.w = 400;
		drawPos.h = 400;

		if (darkness > 0) {
			for (int i = 0; i < 2; i++)
				SDL_SetTextureAlphaMod(masterRainTextureBlack[i], darkness);
		}
		else if (darkness < 0) {
			for (int i = 0; i < 2; i++)
				SDL_SetTextureAlphaMod(masterRainTextureWhite[i], -1 *darkness);
		}

		if (graphicsState->getWeatherVolume() > low) {
			for (auto effect : weatherEffectsAbove) {
				drawPos.w = (int)(baseSize.x * effect.scale);
				drawPos.h = (int)(baseSize.y * effect.scale);
				drawPos.x = (int)(effect.position.x + ABOVE_SCALE * shift);
				drawPos.y = (int)effect.position.y;

				SDL_RenderCopyEx(graphicsState->getGRenderer(), masterRainTexture[effect.frameNumber], NULL, &drawPos, effect.angle, NULL, SDL_FLIP_NONE);
				
				if (darkness > 0) {
					SDL_RenderCopyEx(graphicsState->getGRenderer(), masterRainTextureBlack[effect.frameNumber], NULL, &drawPos, effect.angle, NULL, SDL_FLIP_NONE);
				}
				else if (darkness < 0) {
					SDL_RenderCopyEx(graphicsState->getGRenderer(), masterRainTextureWhite[effect.frameNumber], NULL, &drawPos, effect.angle, NULL, SDL_FLIP_NONE);
				}

			}
		}

	}

	void drawBackWeather(LightMaster* lm, int shift, int darkness) {
		SDL_Rect drawPos;
		drawPos.w = 400;
		drawPos.h = 400;

		if (darkness > 0) {
			for (int i = 0; i < 2; i++)
				SDL_SetTextureAlphaMod(masterRainTextureBlack[i], darkness);
		}
		else if (darkness < 0) {
			for (int i = 0; i < 2; i++)
				SDL_SetTextureAlphaMod(masterRainTextureWhite[i], -1 * darkness);
		}

		if (graphicsState->getWeatherVolume() > high) {
			for (auto effect : weatherEffectsBelow) {
				drawPos.w = (int)(baseSize.x * effect.scale);
				drawPos.h = (int)(baseSize.y * effect.scale);
				drawPos.x = (int)(effect.position.x + BELOW_SCALE * shift);
				drawPos.y = (int)effect.position.y;

				SDL_RenderCopyEx(graphicsState->getGRenderer(), masterRainTexture[effect.frameNumber], NULL, &drawPos, effect.angle, NULL, SDL_FLIP_NONE);
				
				if (darkness > 0) {
					SDL_RenderCopyEx(graphicsState->getGRenderer(), masterRainTextureBlack[effect.frameNumber], NULL, &drawPos, effect.angle, NULL, SDL_FLIP_NONE);
				}
				else if (darkness < 0) {
					SDL_RenderCopyEx(graphicsState->getGRenderer(), masterRainTextureWhite[effect.frameNumber], NULL, &drawPos, effect.angle, NULL, SDL_FLIP_NONE);
				}

			}
		}

	}

	void drawMiddleWeather(LightMaster* lm, int shift, int darkness) {
		SDL_Rect drawPos;
		drawPos.w = 400;
		drawPos.h = 400;

		if (darkness > 0) {
			for (int i = 0; i < 2; i++)
				SDL_SetTextureAlphaMod(masterRainTextureBlack[i], darkness);
		}
		else if (darkness < 0) {
			for (int i = 0; i < 2; i++)
				SDL_SetTextureAlphaMod(masterRainTextureWhite[i], -1 * darkness);
		}

		if (graphicsState->getWeatherVolume() > medium) {
			for (auto effect : weatherEffectsMiddle) {
				drawPos.w = (int)(baseSize.x * effect.scale);
				drawPos.h = (int)(baseSize.y * effect.scale);
				drawPos.x = (int)(effect.position.x + shift);
				drawPos.y = (int)effect.position.y;

				SDL_RenderCopyEx(graphicsState->getGRenderer(), masterRainTexture[effect.frameNumber], NULL, &drawPos, effect.angle, NULL, SDL_FLIP_NONE);
			
				if (darkness > 0) {
					SDL_RenderCopyEx(graphicsState->getGRenderer(), masterRainTextureBlack[effect.frameNumber], NULL, &drawPos, effect.angle, NULL, SDL_FLIP_NONE);
				}
				else if (darkness < 0) {
					SDL_RenderCopyEx(graphicsState->getGRenderer(), masterRainTextureWhite[effect.frameNumber], NULL, &drawPos, effect.angle, NULL, SDL_FLIP_NONE);
				}
			}

		}

	}

	//Stops the weather, but doesnt clear current weather effects
	void stopWeather() {
		weatherType = noweather;
		stopChannel(SCWeatherBackground, 300);
	}

	int getEffectsCountByIndex(int index) {
		switch (index) {
		case 0:
			return weatherEffectsBelow.size();
		case 1:
			return weatherEffectsMiddle.size();
		case 2:
			return weatherEffectsAbove.size();
		}
		cout << "missindex bro " << endl;
		return 0;
	}

	WeatherType getWeatherType() {
		return weatherType;
	}

};


#endif