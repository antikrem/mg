#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#define pass true

#define PI (float)3.14159265359

#define not(a) (!(a))
#define F(a) (float)(a)

#define DEG2RAD(a) a*(PI/180)
#define RAD2DEG(a) a*(180/PI)

//engine abstract workspace size
const int WORK_SPACE_X = 1520;
const int WORK_SPACE_Y = 1080;

//Rendered space
const int RENDERED_X = 960;

//"safe space" area to side that is not restricted
const int SAFESPACE = 340;

//Full area rendered
const int FULL_SPACE_X = 1920;
const int FULL_SPACE_Y = 1080;

//Used for calculating mid points
const float MID_OFFSET = F(480);
const float MID_GRADIENT = F(0.36842105);
#endif