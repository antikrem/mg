#ifndef __INPUT_H_INCLUDED__
#define __INPUT_H_INCLUDED__

#include "SDL.h"
#include "error.h"
#include "dict.h"

/*A structure that stores player inputs. Used for in-game movement and menu controls*/
struct Input {
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
	bool shift = false;
	bool dash = false;
	bool shoot = false;
	bool special = false;
	bool enter = false;
	bool esc = false;
	bool console = false;


	bool operator==(Input& rhs);

	bool buttonPressed();
};

struct KeyBind {
	SDL_Scancode leftCode = SDL_SCANCODE_LEFT;
	SDL_Scancode rightCode = SDL_SCANCODE_RIGHT;
	SDL_Scancode upCode = SDL_SCANCODE_UP;
	SDL_Scancode downCode = SDL_SCANCODE_DOWN;
	SDL_Scancode shootCode = SDL_SCANCODE_Z;
	SDL_Scancode specialCode = SDL_SCANCODE_X;
	SDL_Scancode dashCode = SDL_SCANCODE_LSHIFT;
	SDL_Scancode shiftCode = SDL_SCANCODE_LCTRL;
	SDL_Scancode enterCode = SDL_SCANCODE_RETURN;
	SDL_Scancode escCode = SDL_SCANCODE_ESCAPE;
	SDL_Scancode consoleCode = SDL_SCANCODE_GRAVE;
};

void updateScanCodes(KeyBind* keyBind);

#endif