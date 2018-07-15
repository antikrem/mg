/*Classes for interactive objects, like buttons.
To be used with _controller.cpp*/
#ifndef __INTERACTIVES_H__
#define __INTERACTIVES_H__

#include "SDL.h"
#include "text_entity.h"
#include "input.h"

#include <vector>

/*represents a button, to be accessed by button manager*/
struct Button {
public:
	SDL_Rect position;
	//Function ran when button is pressed
	void(*callback)(LevelSettings*);
	string text;
};

class ButtonManager : public ThreadSafe {
private:
	bool active = false;
	bool loadIn = false;
	bool loadOut = false;
	int timeIn = 0;
	int timeOut;
	int inputDelay = 200;
	int currentButton = 0;
	vector<Button> buttonList;
	//Buttons laid out vertically or horizontally
	bool horizontalMovement = false;

	SDL_Texture* marker = NULL;

	TextContainer* buttonFaces = NULL;
	TextRenderer* tRenderer = NULL;
	Color color;
	Font font;
public:
	ButtonManager(TextRenderer* tRenderer, Color color, Font font) {
		this->tRenderer = tRenderer;
		buttonFaces = new TextContainer(tRenderer);
		this->color = color;
		this->font = font;

		SDL_Surface* markerSurf = SDL_LoadBMP("assets\\menu\\marker.bmp");
		marker = SDL_CreateTextureFromSurface(tRenderer->gRenderer, markerSurf);
		SDL_FreeSurface(markerSurf);
	}

	~ButtonManager() {
		delete buttonFaces;
		SDL_DestroyTexture(marker);
	}

	void activate(int fadeInDelay) {
		loadIn = true;
		active = true;
		loadOut = false;
		this->timeIn = fadeInDelay;

		int i = -1;
		CUS_Point position;
		for (Button button : buttonList) {
			i++;
			position = { button.position.x + ((float)button.position.w / 2) , button.position.y + ((float)button.position.h / 2) };
			buttonFaces->newTextEnt(to_string(i), button.text, position, font, color,
				midMid, true, { 0,0 }, { 0,0 }, INT_MAX, 0, (float)255 / fadeInDelay);
		}
	}

	void deactivate(int fadeOutDelay) {
		loadOut = false;
	}

	void addButton(string text, SDL_Rect position, void(callback)(LevelSettings*)) {
		buttonList.push_back({ position, callback, text });
	}

	void update(Input input, LevelSettings* currentSettings) {
		if (active) {
			buttonFaces->updateAllTextEnts();

			inputDelay--;
			timeIn--;

			if (loadOut) {
				timeOut--;
			}

			if (loadIn && (timeIn <= 0)) {
				loadIn = false;
			}
			if (loadOut && (timeOut <= 0)) {
				active = false;
			}

			if (!input.buttonPressed()) {
				inputDelay = 0;
			}

			//If delay is over and a button is pressed, handle the input
			if ((inputDelay <= 0) && !loadIn && !loadOut && input.buttonPressed()) {

				inputDelay = 70;

				//handle vertical buttons
				if (!horizontalMovement) {
					if (input.up) {
						currentButton--;
					}
					else  if (input.down) {
						currentButton++;
					}
				}
				//handle horrizontal inputs
				else {
					if (input.left) {
						currentButton--;
					}
					else if (input.right) {
						currentButton++;
					}
				}

				//rectify currentButton
				if (currentButton == -1) {
					currentButton = (buttonList.size() - 1);
				}
				else if (currentButton == buttonList.size()) {
					currentButton = 0;
				}

				//on selection input, call the function of the button
				if (input.enter || input.shoot) {
					buttonList[currentButton].callback(currentSettings);
				}
			}
		}
	}

	void render() {
		if (active) {
			buttonFaces->renderText(0);
			if (!loadIn && !loadOut) {
				SDL_Rect renderbox;
				renderbox.x = buttonList[currentButton].position.x;
				renderbox.y = buttonList[currentButton].position.y + (buttonList[currentButton].position.h / 2) + -27;
				renderbox.h = 54;
				renderbox.w = 42;
				SDL_RenderCopy(tRenderer->gRenderer, marker, NULL, &renderbox);
			}
		}
	}
};

#endif