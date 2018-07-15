//Enumerations and structures associated with levels
#ifndef __LEVEL_ENUM_H_INCLUDED__
#define __LEVEL_ENUM_H_INCLUDED__

#include <atomic>

using namespace std;

//Different stages
enum Stage {
	titleMenu,
	inGame,
	scoreBoard
};

//Enumeration of the different characters
enum Characters {
	first,
	second,
	third
};

//Sets LevelSettings button event flag when
enum ButtonFlag {
	none,
	titleToMenu,

};

//Settings for game levels, carried between levels
struct LevelSettings {
	int points = 0;
	int lives = 5;
	int spells = 5;
	int level = 0;
	int grazes = 0;
	Characters currentCharacter = first;
	atomic<bool> endLevel = false;
	atomic<bool> endGame = false;
	Stage nextStage = titleMenu;
	Stage currentStage = titleMenu;

	atomic<ButtonFlag> buttonFlag = none;
};

#endif