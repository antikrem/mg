//TODO: properly free text texture memory
#ifndef __TEXT_ENTITY_H__
#define __TEXT_ENTITY_H__

#include "box_entity.h"
#include "thread_safe.h"
#include "SDL_ttf.h"
#include "error.h"
#include "graphics_state.h"

#include <map>
#include <algorithm>
#include <memory>

#include <iostream>

enum Alignment {
	topLeft,
	midLeft,
	midMid
};

enum Font {
	sans18,
	sans20,
	sans22,
	sans28,
	sans36,
	sans48
};

enum Color {
	white,
	red,
	black,
	gold
};

/*Rendering context for text objects*/
struct TextRenderer : public ThreadSafe {
	GraphicsState* gState;
	SDL_Renderer* gRenderer;

	map<Font, TTF_Font*> fonts;
	map<Font, TTF_Font*> bolds;
	map<Color, SDL_Color> colors;

	TextRenderer(GraphicsState* gState) {
		this->gState = gState;
		this->gRenderer = gState->getGRenderer();

		//fonts
		fonts[sans18] = TTF_OpenFont("assets\\fonts\\Sans.ttf", 18);
		fonts[sans20] = TTF_OpenFont("assets\\fonts\\Sans.ttf", 20);
		fonts[sans22] = TTF_OpenFont("assets\\fonts\\Sans.ttf", 22);
		fonts[sans28] = TTF_OpenFont("assets\\fonts\\Sans.ttf", 28);
		fonts[sans36] = TTF_OpenFont("assets\\fonts\\Sans.ttf", 36);
		fonts[sans48] = TTF_OpenFont("assets\\fonts\\Sans.ttf", 48);

		//bolds
		bolds[sans18] = TTF_OpenFont("assets\\fonts\\Sans.ttf", 18);
		TTF_SetFontOutline(bolds[sans18], 2);
		bolds[sans20] = TTF_OpenFont("assets\\fonts\\Sans.ttf", 18);
		TTF_SetFontOutline(bolds[sans20], 2);
		bolds[sans22] = TTF_OpenFont("assets\\fonts\\Sans.ttf", 22);
		TTF_SetFontOutline(bolds[sans22], 2);
		bolds[sans28] = TTF_OpenFont("assets\\fonts\\Sans.ttf", 28);
		TTF_SetFontOutline(bolds[sans28], 2);
		bolds[sans36] = TTF_OpenFont("assets\\fonts\\Sans.ttf", 36);
		TTF_SetFontOutline(bolds[sans36], 2);
		bolds[sans48] = TTF_OpenFont("assets\\fonts\\Sans.ttf", 48);
		TTF_SetFontOutline(bolds[sans48], 2);

		//colors
		colors[white] = { 255, 255, 255 };
		colors[red] = { 255, 0, 0 };
		colors[black] = { 0,0,0 };
		colors[gold] = { 255,215,0,255 };
	}

	~TextRenderer() {
		TTF_CloseFont(fonts[sans18]);
		TTF_CloseFont(fonts[sans20]);
		TTF_CloseFont(fonts[sans22]);
		TTF_CloseFont(fonts[sans28]);
		TTF_CloseFont(fonts[sans36]);
		TTF_CloseFont(fonts[sans48]);

		TTF_CloseFont(bolds[sans18]);
		TTF_CloseFont(bolds[sans20]);
		TTF_CloseFont(bolds[sans22]);
		TTF_CloseFont(bolds[sans28]);
		TTF_CloseFont(bolds[sans36]);
		TTF_CloseFont(bolds[sans48]);
	}
};

class TextEntity {
	int internalCounter = 0;
	bool decay = false;
	int cycles = INT_MAX;

	bool outline = false;

	TextRenderer* TEXT_RENDERER;
	bool flag = true;

	float angle = 0;
	string text;
	Alignment alignment;
	Font font;
	Color color;

	SDL_Texture* textTex = NULL;
	SDL_Rect textRect;

	CUS_Point position;

	float alpha = 255;
	float alphaVelocity = 0;

	CUS_Polar velocity = { 0,0 };
	CUS_Polar acceleration = { 0,0 };

	void fixAllignment() {
		switch (alignment) {
		case(topLeft):
			textRect.x = (int)position.x;
			textRect.y = (int)position.y;
			break;
		case(midMid):
			textRect.x = (int)position.x - (int)(textRect.w / 2);
			textRect.y = (int)position.y - (int)(textRect.h / 2);
			break;
		case(midLeft):
			textRect.x = (int)position.x;
			textRect.y = (int)position.y - (int)(textRect.h / 2);
			break;
		}
	}

public:
	TextEntity(TextRenderer* TEXT_RENDERER, string text, CUS_Point position, Font font, Color color, Alignment alignment = midMid, int cycles = INT_MAX, bool outline = false) {
		this->text = text;
		this->font = font;
		this->alignment = alignment;
		this->color = color;
		this->position = position;
		this->outline = outline;
		this->TEXT_RENDERER = TEXT_RENDERER;

		if (cycles != INT_MAX) {
			decay = true;
			this->cycles = cycles;
		}

		SDL_Surface* textSurf;

		if (this->outline) {
			SDL_Surface* foreground = TTF_RenderText_Solid((*TEXT_RENDERER).fonts[font], text.c_str(), (*TEXT_RENDERER).colors[color]);;
			textSurf = TTF_RenderText_Solid((*TEXT_RENDERER).bolds[font], text.c_str(), (*TEXT_RENDERER).colors[black]);
			SDL_Rect blitzRect = { 1,1,foreground->w,foreground->h };
			SDL_BlitSurface(foreground, NULL, textSurf, &blitzRect);
			SDL_FreeSurface(foreground);
		}
		else {
			textSurf = TTF_RenderText_Solid((*TEXT_RENDERER).fonts[font], text.c_str(), (*TEXT_RENDERER).colors[color]);
		}

		textTex = SDL_CreateTextureFromSurface(TEXT_RENDERER->gRenderer, textSurf);
		textRect = { 0, 0, textSurf->w, textSurf->h };
		fixAllignment();
		SDL_FreeSurface(textSurf);
	}

	~TextEntity() {
		SDL_DestroyTexture(textTex);
	}

	void setAlpha(float alpha, float alphaVelocity = 0) {
		this->alpha = alpha;
		this->alphaVelocity = alphaVelocity;
	}

	void setMovement(CUS_Polar velocity, CUS_Polar acceleration = { 0,0 }) {
		this->velocity = velocity;
		this->acceleration = acceleration;
	}

	void update() {
		internalCounter++;

		if (decay) {
			if (cycles <= internalCounter) {
				flag = false;
			}
		}

		velocity += acceleration;
		alpha += alphaVelocity;
		if (alpha >= 250) {
			alpha = 255;
		}

		position += velocity.toPoint();
		fixAllignment();
	}

	void changeText(string newText) {
		this->text = newText;

		SDL_DestroyTexture(textTex);

		SDL_Surface* textSurf;

		if (this->outline) {
			SDL_Surface* foreground = TTF_RenderText_Solid((*TEXT_RENDERER).fonts[font], text.c_str(), (*TEXT_RENDERER).colors[color]);;
			textSurf = TTF_RenderText_Solid((*TEXT_RENDERER).bolds[font], text.c_str(), (*TEXT_RENDERER).colors[black]);
			SDL_Rect blitzRect = { 1,1,foreground->w,foreground->h };
			SDL_BlitSurface(foreground, NULL, textSurf, &blitzRect);
			SDL_FreeSurface(foreground);
		}
		else {
			textSurf = TTF_RenderText_Solid((*TEXT_RENDERER).fonts[font], text.c_str(), (*TEXT_RENDERER).colors[color]);
		}

		textTex = SDL_CreateTextureFromSurface(TEXT_RENDERER->gRenderer, textSurf);
		textRect = { 0, 0, textSurf->w, textSurf->h };
		fixAllignment();
		SDL_FreeSurface(textSurf);
	}

	int getInternalCounter() {
		return internalCounter;
	}

	bool getFlag() {
		return flag;
	}

	void setFlag(bool flag) {
		this->flag = flag;
	}

	SDL_Texture* getTex() {
		return textTex;
	}

	SDL_Rect getRect() {
		return textRect;
	}

	float getAngle() {
		return angle;
	}

	int getAlpha() {
		if (alpha > 250) {
			return 255;
		}
		else if (alpha > 0) {
			return (int)alpha;
		}
		else {
			return 0;
		}
	}

};

class TextContainer : public ThreadSafe {
private:
	SDL_Renderer * RENDERER;
	TextRenderer* TEXT_RENDERER;
	map<string, unique_ptr<TextEntity> > textMap;

public:
	TextContainer(TextRenderer* TEXT_RENDERER) {
		this->RENDERER = TEXT_RENDERER->gRenderer;
		this->TEXT_RENDERER = TEXT_RENDERER;
	}
	~TextContainer() {
		textMap.clear();
	}

	void renderText(int shift) {
		SDL_Rect temp;
		for (auto const& textEnt : textMap) {
			temp = textEnt.second->getRect();
			temp.x += shift;
			if (textEnt.second->getAlpha() > 250) {
				SDL_RenderCopyEx(RENDERER, textEnt.second->getTex(), NULL, &temp, (double)(textEnt.second->getAngle()), NULL, SDL_FLIP_NONE);
			}
			else if (textEnt.second->getAlpha() > 0) {
				SDL_SetTextureAlphaMod(textEnt.second->getTex(), (int)textEnt.second->getAlpha());
				SDL_RenderCopyEx(RENDERER, textEnt.second->getTex(), NULL, &temp, (double)(textEnt.second->getAngle()), NULL, SDL_FLIP_NONE);
				SDL_SetTextureAlphaMod(textEnt.second->getTex(), 255);
			}
		}
	}

	void fadeOutAllText(int fadeOutDelay) {
		for (auto const& textEnt : textMap) {
			textEnt.second->setAlpha(255, -(float)255 / fadeOutDelay);
		}
	}

	void newTextEnt(string identifier, string text, CUS_Point position, Font font, Color color, Alignment alignment = midMid,
		bool outline = false, CUS_Polar velocity = { 0,0 }, CUS_Polar acceleration = { 0,0 },
		int cycles = INT_MAX, float alpha = 255, float alphaVelocity = 0) {
		TextEntity* toPush = new TextEntity(TEXT_RENDERER, text, position, font, color, alignment, cycles, outline);
		toPush->setMovement(velocity, acceleration);
		toPush->setAlpha(alpha, alphaVelocity);
		textMap[identifier] = unique_ptr<TextEntity>(toPush);
	}

	void updateAllTextEnts() {

		if (textMap.size() != 0) {
			for (auto const& textEnt : textMap) {
				textEnt.second->update();
			}

			for (std::map<string, unique_ptr<TextEntity>>::iterator it = textMap.begin(); it != textMap.end();) {
				if (!(it->second->getFlag())) {
					it = textMap.erase(it);
				}
				else {
					it++;
				}
			}
		}
	}

	void updateTextByKey(string identifier, string text, bool suppressWarning = false) {
		if (textMap.count(identifier) == 0 && !suppressWarning) {
			err::logMessage("A text was edited when the identifier didn't exist. Identifier is, ");
			err::logMessage(identifier);
		}
		else if (textMap.count(identifier) != 0) {
			textMap[identifier]->changeText(text);
		}
	}

};

#endif