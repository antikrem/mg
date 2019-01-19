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
	midMid,
	topRight
};

enum Font {
	sans18,
	sans20,
	sans22,
	sans28,
	sans32,
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

	TextRenderer(GraphicsState* gState);

	~TextRenderer();
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

	int textWrapeLength = 0;

	CUS_Point position;

	float alpha = 255;
	float alphaVelocity = 0;

	CUS_Polar velocity = { 0,0 };
	CUS_Polar acceleration = { 0,0 };

	/*changes render box to current alignment*/
	void fixAllignment();

public:
	TextEntity(TextRenderer* TEXT_RENDERER, string text, CUS_Point position, Font font, Color color, Alignment alignment = midMid, int cycles = INT_MAX, bool outline = false);

	~TextEntity();

	void setAlpha(float alpha, float alphaVelocity = 0);

	void setMovement(CUS_Polar velocity, CUS_Polar acceleration = { 0,0 });

	void update();

	void changeText(string newText);

	//This kills the outline
	void setTextWrapLength(int textWrapeLength);

	int getInternalCounter();
	bool getFlag();
	void setFlag(bool flag);

	SDL_Texture* getTex();
	SDL_Rect getRect();
	float getAngle();
	int getAlpha();

	void setPosition(CUS_Point position);
};

class TextContainer : public ThreadSafe {
protected:
	SDL_Renderer * RENDERER;
	TextRenderer* TEXT_RENDERER;
	map<string, unique_ptr<TextEntity> > textMap;

public:
	TextContainer(TextRenderer* TEXT_RENDERER);

	~TextContainer();

	void renderText(int shift);

	void fadeOutAllText(int fadeOutDelay);

	void newTextEnt(string identifier, string text, CUS_Point position, Font font, Color color, Alignment alignment = midMid,
		bool outline = false, CUS_Polar velocity = { 0,0 }, CUS_Polar acceleration = { 0,0 },
		int cycles = INT_MAX, float alpha = 255, float alphaVelocity = 0);

	void updateAllTextEnts();

	void updateTextByKey(string identifier, string text, bool suppressWarning = false);

	int noOfEnts();

	TextEntity* getEnt(string name);
};

enum Fonts {
	sans
};

struct fontLibrary {
	int size;

};

class TextGlobalMaster {
	GraphicsState* gState;
	SDL_Renderer* gRenderer;

	map<Font, map<int, TTF_Font*>> fontSources;

	TextGlobalMaster(GraphicsState* gState) {
		this->gState = gState;
		this->gRenderer = gState->getGRenderer();
	}

	~TextGlobalMaster();
};

#endif