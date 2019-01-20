//TODO: properly free text texture memory
#ifndef __TEXT_ENTITY_H__
#define __TEXT_ENTITY_H__

#include "box_entity.h"
#include "thread_safe.h"
#include "SDL_ttf.h"
#include "error.h"
#include "graphics_state.h"
#include "constants.h"
#include "listed_entities.h"

#include <map>
#include <algorithm>
#include <memory>

#include <iostream>

enum Alignment {
	topLeft,
	midLeft,
	midMid,
	topRight,
	bottomLeft
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
	SDL_Renderer* RENDERER;
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
	sansFontStyle
};

enum FontColors {
	whiteFontColor,
	redFontColor,
	blackFontColor,
	goldFontColor
};

#define FONTCOLOURLENGTH 4

#define CHAR2FONTINDEX(a) ((int)a - 32)
#define FONTINDEX2CHAR(a) ((char)(a+32))
#define FONTINDEXSIZE CHAR2FONTINDEX('}')+1

#define INT2COLOR(a) static_cast<FontColors>(a)

struct FontLibrary {
	int fontSize;
	Fonts font;
	TTF_Font* baseFont = NULL;
	TTF_Font* boldFont = NULL;
	SDL_Texture* baseTextureList[FONTINDEXSIZE][FONTCOLOURLENGTH];
	SDL_Texture* boldTextureList[FONTINDEXSIZE][FONTCOLOURLENGTH];
	int characterWidth[FONTINDEXSIZE];
	int realHeight;

};

class TextGlobalMaster {
	GraphicsState* gState;
	SDL_Renderer* gRenderer;

	map<Fonts, map<int, FontLibrary>> fontSources;
	map<FontColors, SDL_Color> colors;

	void loadAFont(Fonts font, int size, string fontName) {
		if (fontSources.count(font) != 0) {
			if (fontSources[font].count(size) != 0) {
				err::logMessage("WARNING: A font was requested when it already exists, no changes made");
				return;
			}
		}

		//Prepare library
		FontLibrary fontLibrary;
		fontLibrary.fontSize = size;
		fontLibrary.font = font;
		string location = "assets\\fonts\\" + fontName + ".ttf";
		fontLibrary.baseFont = TTF_OpenFont(location.c_str(), size);
		fontLibrary.boldFont = TTF_OpenFont(location.c_str(), size);
		TTF_SetFontOutline(fontLibrary.boldFont, 2);

		//Create textures and save char widths
		string s;
		SDL_Surface* surf = NULL;
		SDL_Surface* surfOutline = NULL;
		SDL_Rect blitzRect;
	
		for (int i = 0; i < FONTCOLOURLENGTH; i++) {
			for (int j = 0; j < FONTINDEXSIZE; j++) {
				s = FONTINDEX2CHAR(j);
				surf = TTF_RenderText_Solid(fontLibrary.baseFont, s.c_str(), colors[INT2COLOR(i)]);
				fontLibrary.baseTextureList[j][i] = SDL_CreateTextureFromSurface(gRenderer, surf);
				surfOutline = TTF_RenderText_Solid(fontLibrary.boldFont, s.c_str(), colors[INT2COLOR(i)]);
				blitzRect = { 1,1,surf->w, surf->h };
				SDL_BlitSurface(surf, NULL, surfOutline, &blitzRect);
				fontLibrary.boldTextureList[j][i] = SDL_CreateTextureFromSurface(gRenderer, surf);
				fontLibrary.realHeight = surf->h;
				fontLibrary.characterWidth[j] = surf->w;
				SDL_FreeSurface(surf);
				SDL_FreeSurface(surfOutline);

				fontSources[font][size] = fontLibrary;
			}
		}

	}

public:

	TextGlobalMaster(GraphicsState* gState) {
		this->gState = gState;
		this->gRenderer = gState->getGRenderer();

		//colors
		colors[whiteFontColor] = { 255, 255, 255 };
		colors[redFontColor] = { 255, 0, 0 };
		colors[blackFontColor] = { 0,0,0 };
		colors[goldFontColor] = { 255,215,0,255 };

		loadAFont(sansFontStyle, 36, "Sans");
		loadAFont(sansFontStyle, 32, "Sans");
		loadAFont(sansFontStyle, 28, "Sans");
		loadAFont(sansFontStyle, 22, "Sans");
	}

	~TextGlobalMaster() {
		cout << "TODO:" << endl;
	}

	SDL_Renderer* getRenderer() {
		return gRenderer;
	}

	//Checks font and returns true if not there
	bool checkFontInMaster(Fonts font, int size, string fontName) {
		if (fontSources.count(font) != 0) {
			if (fontSources[font].count(size) != 0) {
				return true;
			}
		}
		loadAFont(font, size, fontName);
		return false;
	}

	//TODO: check
	FontLibrary getFontLibrary(Fonts font, int size) {
		return fontSources[font][size];
	}
};

//Container to interface with TextMaster
class TextEntityA : public ThreadSafe, public ListedEntity {
	int internalCounter = 0;
	int cycles = -1;

	bool outline = false;

	string text;
	Alignment alignment;
	int size;
	Fonts font;
	FontColors color;
	int spacing = 0;

	int textWrapeLength = 0;

	float alphaVelocity = 0;

public:
	TextEntityA(string text, CUS_Point position, int size, Fonts font, FontColors color, Alignment alignment, int cycles, int spacing, bool outline) {
		this->text = text;
		this->position = position;
		this->size = size;
		this->font = font;
		this->color = color;
		this->alignment = alignment;
		this->cycles = cycles;
		this->spacing = spacing;
		this->outline = outline;
	}

	void setAlphaEx(float alpha, float alphaVelocity) {
		lock();
		this->alpha = alpha;
		this->alphaVelocity = alphaVelocity;
		unlock();
	}

	void setMovement(CUS_Point velocity, CUS_Point acceleration) {
		lock();
		this->velocity = velocity;
		this->acceleration = acceleration;
		unlock();
	}

	void setText(string text) {
		lock();
		this->text = text;
		unlock();
	}
	
	void update() {
		lock();
		internalCounter++;

		if (cycles != -1) {
			if (cycles <= internalCounter) {
				flag = false;
			}
		}

		velocity += acceleration;
		position += velocity;
		alpha += alphaVelocity;
		unlock();
	}

	void render(int shift, TextGlobalMaster* textGlobalMaster) {
		lock();
		FontLibrary lib = textGlobalMaster->getFontLibrary(font, size);
		SDL_Rect textPos;

		//Get length of entire string for balancing purposes
		int lengthOfString = 0;
		if not(textWrapeLength) {
			for (auto c : text) {
				lengthOfString += lib.characterWidth[CHAR2FONTINDEX(c)] + spacing;
			}
		}
		else {
			//TODO Wrap text look up too
		}

		//Switch for top y position
		switch (alignment) {
		case topLeft:
		case topRight:
			textPos.y = (int)position.y;
			break;
		case midLeft:
		case midMid:
			textPos.y = (int)position.y + (int)(size/2);
			break;
		case bottomLeft:
			textPos.y = (int)position.y + (int)size;
			break;
		}
		textPos.h = lib.realHeight;

		//Switch for x position
		switch (alignment) {
		case topLeft:
		case midLeft:
		case bottomLeft:
			textPos.x = (int)position.x;
			break;
		case midMid:
			textPos.x = (int)position.x + (int)(lengthOfString / 2);
			break;
		case topRight:
			textPos.x = (int)position.x - (int)lengthOfString;
			break;
		}
		textPos.x += shift;

		//Begin drawing
		for (auto c : text) {
			textPos.w = lib.characterWidth[CHAR2FONTINDEX(c)];
			auto texture = outline ? lib.boldTextureList[CHAR2FONTINDEX(c)][color] : lib.baseTextureList[CHAR2FONTINDEX(c)][color];
			if (alpha > 250) {
				SDL_RenderCopyEx(textGlobalMaster->getRenderer(), texture, NULL, &textPos, 0, NULL, SDL_FLIP_NONE);
			}
			else if (alpha > 0) {
				SDL_SetTextureAlphaMod(texture, (Uint8)alpha);
				SDL_RenderCopyEx(textGlobalMaster->getRenderer(), texture, NULL, &textPos, 0, NULL, SDL_FLIP_NONE);
				SDL_SetTextureAlphaMod(texture, 255);
			}
			
			textPos.x += textPos.w + spacing;
		}
		unlock();
	}

	void changeText(string text) {
		lock();
		this->text = text;
		unlock();
	}

};
/*
Locked when textEntityList is changed in size.
*/
class TextMaster {
	map< string, unique_ptr<TextEntityA> > textEntityList;

	//Changing textEntityList size requires both locks to be engaged
	mutex lock;

		//Counts up from zero when allocating no reference objects
	int allocator = 0;

	TextGlobalMaster* textGlobalMaster = NULL;
	SDL_Renderer* RENDERER = NULL;

public:
	TextMaster(TextGlobalMaster* master) {
		textGlobalMaster = master;
		RENDERER = master->getRenderer();
	}

	~TextMaster() {
		textEntityList.clear();
	}

	void updateAllTextEnts() {
		lock.lock();
		for (auto const& i : textEntityList) {
			i.second->update();
		}

		for (std::map<string, unique_ptr<TextEntityA>>::iterator it = textEntityList.begin(); it != textEntityList.end();) {
			if (!(it->second->getFlag())) {
				it = textEntityList.erase(it);
			}
			else {
				it++;
			}
		}
		lock.unlock();
		
	}

	void renderText(int shift, int check = false) {
		lock.lock();
		for (auto const& i : textEntityList) {
			i.second->render(shift, textGlobalMaster);
		}
		lock.unlock();
	}

	//Use an empty string to 
	void addTextEnt(string identifier, string text, CUS_Point position, int size, Fonts font, FontColors color, Alignment alignment = midMid, 
		int spacing = 0, bool outline = false, CUS_Polar velocity = { 0,0 }, CUS_Polar acceleration = { 0,0 },
		int cycles = -1, float alpha = 255, float alphaVelocity = 0) {
		lock.lock();

		string newIdentifier = identifier;
		if (newIdentifier == "") {
			newIdentifier = to_string(++allocator);
		}

		if (textEntityList.count(identifier)) {
			err::logMessage("WARNING: A text ent was added when it already existed, existing ent changed instead");
		}
		

		textEntityList[newIdentifier] = make_unique<TextEntityA>(text, position, size, font, color, alignment, cycles, spacing, outline);
		textEntityList[newIdentifier]->setMovement(velocity.toPoint(), acceleration.toPoint());
		textEntityList[newIdentifier]->setAlphaEx(alpha, alphaVelocity);

		lock.unlock();
	}

	void updateTextByKey(string identifier, string newText) {
		lock.lock();
		textEntityList[identifier]->setText(newText);
		lock.unlock();
	}
};

#endif