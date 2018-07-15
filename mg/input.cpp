#include "input.h"
#include <cstdio>

bool Input::operator==(Input& rhs) {
	return (up == rhs.up && down == rhs.down && left == rhs.left && right == rhs.right);
}

void updateScanCodes(KeyBind* keyBind) {
	try {
		Dictionary keyDict("config//keybinds.ini");
		keyBind->leftCode = static_cast<SDL_Scancode> (stoi(keyDict.byKey("left", true)));
		keyBind->rightCode = static_cast<SDL_Scancode> (stoi(keyDict.byKey("right")));
		keyBind->upCode = static_cast<SDL_Scancode> (stoi(keyDict.byKey("up")));
		keyBind->downCode = static_cast<SDL_Scancode> (stoi(keyDict.byKey("down")));
		keyBind->shootCode = static_cast<SDL_Scancode> (stoi(keyDict.byKey("shoot")));
		keyBind->specialCode = static_cast<SDL_Scancode> (stoi(keyDict.byKey("special")));
		keyBind->dashCode = static_cast<SDL_Scancode> (stoi(keyDict.byKey("dash")));
		keyBind->shiftCode = static_cast<SDL_Scancode> (stoi(keyDict.byKey("shift")));
		keyBind->enterCode = static_cast<SDL_Scancode> (stoi(keyDict.byKey("enter")));
		keyBind->escCode = static_cast<SDL_Scancode> (stoi(keyDict.byKey("escape")));
	}
	catch (const std::exception & ex)
	{
		ex.what();
		err::logMessage("WARNING: config/keybinds.ini is corrupted, Default keybinds loaded and keybinds.ini reverted");
		
		keyBind->leftCode = static_cast<SDL_Scancode> (80);
		keyBind->rightCode = static_cast<SDL_Scancode> (79);
		keyBind->upCode = static_cast<SDL_Scancode> (82);
		keyBind->downCode = static_cast<SDL_Scancode> (81);
		keyBind->shootCode = static_cast<SDL_Scancode> (29);
		keyBind->specialCode = static_cast<SDL_Scancode> (27);
		keyBind->dashCode = static_cast<SDL_Scancode> (225);
		keyBind->shiftCode = static_cast<SDL_Scancode> (224);
		keyBind->enterCode = static_cast<SDL_Scancode> (40);
		keyBind->escCode = static_cast<SDL_Scancode> (41);

		remove("config//keybinds.ini");
		Dictionary keyDict("config//keybinds.ini", true);
		keyDict.addEntry("left", "80");
		keyDict.addEntry("right", "79");
		keyDict.addEntry("up", "82");
		keyDict.addEntry("down", "81");
		keyDict.addEntry("shoot", "29");
		keyDict.addEntry("special", "27");
		keyDict.addEntry("shift", "224");
		keyDict.addEntry("dash", "225");
		keyDict.addEntry("enter", "40");
		keyDict.addEntry("escape", "41");

	}

	err::logMessage("Scan Codes loaded, parsed and updated");
}

bool Input::buttonPressed() {
	return (up || down || left || right || shift || dash || shoot || special || enter || esc);
}