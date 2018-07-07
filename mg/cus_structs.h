/*Useful structures oftern used*/
#ifndef __CUS_STRUCTS_H__
#define __CUS_STRUCTS_H__

#include "SDL.h"
/**Struct representing a single point
*Operators +, scalar * defined as standard vector operations
Casts to Polar outside of scope, instead casts to polar components
*/

struct CUS_Point {
	float x;
	float y;

	CUS_Point operator+(const CUS_Point& b);
	void operator+=(const CUS_Point& b);
	CUS_Point operator*(const float& b);
	float toPolarAngle();
	float toPolarMagnitude();
	float getAngleToPoint(CUS_Point in);
	float getDistanceToPoint(CUS_Point in);
};

/**Struct representing a single point in polar form: { magnitude, angle }
*Operators +, scalar * defined as standard vector operations
Casts to Point
*/

struct CUS_Polar {
	float magnitude;
	float angle;

	CUS_Point toPoint();

	CUS_Polar operator+(const CUS_Polar& in);

	void operator+=(const CUS_Polar& in);

	CUS_Polar operator*(const float& b);

	CUS_Polar operator*(const CUS_Polar& b);
};

#endif
