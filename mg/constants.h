#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#define pass true

//engine abstract workspace size

const int WORK_SPACE_X = 1520;

const int WORK_SPACE_Y = 1060;

const int FULL_SPACE_X = 1920;

const int FULL_SPACE_Y = 1080;

//parallax shift velocity

const float PARALLAX_VIEWCHANGE_VELOCITY = (float) 4.5;

//rendered size

const int RENDERED_X = 960;

const int RENDERED_Y = 1060;

//engine static entity limits

const int MAX_ENEMY_ENTITY_TOSPAWN = 50;

const int MAX_ENEMY_ENTITY_ACTIVE = 20;

const int MAX_PLAYER_BULLETS_ACTIVE = 50;

//Player movement constants

const float PLAYER_ACCELERATION = (float)0.5;

const float PLAYER_MAX_VELOCITY = (float)5;

const float PLAYER_MAX_FOCUS_VELOCITY = (float)2;

const float PLAYER_MAX_DASH_VELOCITY = (float)15;

const float PLAYER_TURNRATE = (float) 0.63;

//player bullet settings

const int MAX_BULLETS_PER_PLAYER = 30;

//total backgrounds able to be rendered at a time

const int MAX_BACKGROUND_TABLE = 6;

//hitbox growth and shrink size

const float HITBOX_GROWTH = (float) 0.01;

const float HITBOX_SHRINK = (float) 0.02;

//Anonymous entity shift

const float ANONYMOUS_ENTITIY_ABOVE_SHIFT_COEFFICIENT = (float) 2.3;

const float ANONYMOUS_ENTITIY_BELOW_SHIFT_COEFFICIENT = (float) 0.8;

#endif