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

//Sets LevelSettings button event flag when
enum ButtonFlag {
	none,
	pause,
	titleToMenu,

};

//Settings for game levels, carried between levels
struct LevelSettings {
	int currentCharacter = 0;
	atomic<bool> endLevel = false;
	atomic<bool> endGame = false;
	Stage nextStage = titleMenu;
	Stage currentStage = titleMenu;

	atomic<ButtonFlag> buttonFlag = none;

	string campaign = "main";
	int level = 0;

	int points = 0;
	int lives = 5;
	int spells = 5;
	int grazes = 0;
	
};

#endif