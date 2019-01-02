/*Light master, Handles big boy tasks like assignning light values for rendered elements and lightning
Theres 4 major light levels:
The first extends right under player plane, covering background object and middle weather and anon ents under player bullets
This layer is special, because it further scales based on depth, with the level 0 to level 1 scaled
The second extends from player bullets to player, covering all important gameplay elements
The third extends from particles to closest weather layer
These layers are enumerated 0 to 2, 0 being furthest and first rendered
Behind each layer is a pure light render. This is a flat render of transparent colour.
*/
#ifndef __LIGHT_MASTER_H__
#define __LIGHT_MASTER_H__
#include "thread_safe.h"
#include <mutex>

using namespace std;

#define FULLNORMAL 0
#define FULLDARK F(255)
#define FULLBRIGHT F(-255)

enum LightLevels {
	LLfirst,
	LLsecond,
	LLthird,
};

const int lightLevels = 3;

/*Modes are different states the level master can be in
When a state changes, the light master will transition
Certain transition happen at different speeds
*/
enum LightModes {
	LMNormal, //Slight darkening of further levels
	LMRain // Lots of darkening
};

class LightMaster : public ThreadSafe {
	GraphicsState* gState = NULL;
	SDL_Renderer* renderer = NULL;
	SDL_Texture* blackTexture = NULL;

	//The speed at which the light value will recover towards normal
	float lightChangeRate = 0.1f;
	//-- on cycle, light will not change when positive
	int lightChangePause = 0;
	//Current Light mode
	LightModes lightMode = LMNormal;
	//Range between 255 and -255, with negative numbers being brighter, which is applied to all the layers
	float lightBrightness[lightLevels] = { FULLNORMAL, FULLNORMAL, FULLNORMAL };
	//In addition, a layer of translucent colour is drawn before the layer, plus an extra right at the end
	float lightLevelBrightness[lightLevels+1] = { FULLNORMAL, FULLNORMAL, FULLNORMAL, FULLNORMAL };

	void lightBrightnessSet(float changeRate, float far, float medium, float close) {
		if (lightBrightness[0] < far) {
			lightBrightness[0] += changeRate;
		}
		else if (lightBrightness[0] > far) {
			lightBrightness[0] -= changeRate;
		}

		if (lightBrightness[1] < medium) {
			lightBrightness[1] += changeRate;
		}
		else if (lightBrightness[1] > medium) {
			lightBrightness[1] -= changeRate;
		}

		if (lightBrightness[2] < close) {
			lightBrightness[2] += changeRate;
		}
		else if (lightBrightness[2] > close) {
			lightBrightness[2] -= changeRate;
		}
	}

	void lightLevelBrightnessSet(float changeRate, float far, float medium, float close, float closest) {
		if (lightLevelBrightness[0] < far) {
			lightLevelBrightness[0] += changeRate;
		}
		else if (lightLevelBrightness[0] > far) {
			lightLevelBrightness[0] -= changeRate;
		}

		if (lightLevelBrightness[1] < medium) {
			lightLevelBrightness[1] += changeRate;
		}
		else if (lightLevelBrightness[1] > medium) {
			lightLevelBrightness[1] -= changeRate;
		}

		if (lightLevelBrightness[2] < close) {
			lightLevelBrightness[2] += changeRate;
		}
		else if (lightLevelBrightness[2] > close) {
			lightLevelBrightness[2] -= changeRate;
		}

		if (lightLevelBrightness[3] < closest) {
			lightLevelBrightness[3] += changeRate;
		}
		else if (lightLevelBrightness[3] > closest) {
			lightLevelBrightness[3] -= changeRate;
		}
	}

public:

	LightMaster(GraphicsState* graphicsState) {
		gState = graphicsState;
		this->renderer = graphicsState->getGRenderer();
		SDL_Surface *temporarySurfaceStorage = IMG_Load("assets\\lighting\\black.png");
		if (temporarySurfaceStorage == NULL)
			err::logMessage("Black image for lighting failed to load");
		blackTexture = SDL_CreateTextureFromSurface(renderer, temporarySurfaceStorage);
		SDL_FreeSurface(temporarySurfaceStorage);
	}

	LightMaster() {
		SDL_DestroyTexture(blackTexture);
	}

	void setLightMode(LightModes lightMode) {
		this->lightMode = lightMode;
	}

	void update() {
		if (lightChangePause > 0) {
			lightChangePause--;
		}
		else {
			if (lightMode == LMNormal) {
				lightBrightnessSet(0.1f, FULLNORMAL, FULLNORMAL, FULLNORMAL);
				lightLevelBrightnessSet(0.1f, 10.0f, 5.0f, 2.0f, FULLNORMAL);
			}

			if (lightMode == LMRain) {
				lightBrightnessSet(0.1f, FULLNORMAL, FULLNORMAL, FULLNORMAL);
				lightLevelBrightnessSet(0.3f, 50.0f, 50.0f, 50.0f, 50.0f);
			}

		}
	}

	void drawLightLevel(int level) {
		if (level < 0 || level > 3) {
			err::logMessage("hahaha + " + to_string(level));
		}
		SDL_Rect drawPos;
		drawPos.w = 400;
		drawPos.h = 400;

		//Draw blackground
		if (lightLevelBrightness[level] > 0.4f) {
			SDL_SetTextureAlphaMod(blackTexture, (Uint8)(lightLevelBrightness[level]));
			for (int i = 0; i < (WORK_SPACE_X + 400); i += 400) {
				for (int j = 0; j < (WORK_SPACE_Y + 400); j += 400) {
					drawPos.x = i;
					drawPos.y = j;
					SDL_RenderCopyEx(gState->getGRenderer(), blackTexture, NULL, &drawPos, 0, NULL, SDL_FLIP_NONE);
				}
			}
		}
		
	}
};


#endif