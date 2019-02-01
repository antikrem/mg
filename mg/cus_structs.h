/*Useful structures oftern used*/
#ifndef __CUS_STRUCTS_H__
#define __CUS_STRUCTS_H__

#include "SDL.h"
#include <string>
/**Struct representing a single point
*Operators +, -, scalar * defined as standard vector operations
Casts to Polar outside of scope, instead casts to polar components
*/

struct CUS_Point {
	float x;
	float y;

	CUS_Point operator+(const CUS_Point& b);
	void operator+=(const CUS_Point& b);
	CUS_Point operator-(const CUS_Point& b);
	void operator-=(const CUS_Point& b);
	CUS_Point operator*(const float& b);
	bool operator!=(const CUS_Point& b);
	CUS_Point unitVector();
	//Dot product
	float operator*(const CUS_Point& b);
	CUS_Point projectionOnto(CUS_Point a);
	float toPolarAngle();
	float toPolarMagnitude();
	float getAngleToPoint(CUS_Point in);
	float getDistanceToPoint(CUS_Point in);
	float getTurnToPoint(CUS_Point in);
	std::string toPrint() {
		return std::to_string(x) + " " + std::to_string(y);
	}
};

/**Struct representing a single point in polar form: { magnitude, angle }
Angle is referenced from north, positive clockwise
Operators +, scalar *, dotproduct defined as standard vector operations
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

CUS_Polar toPolar(CUS_Point point);

CUS_Point toPoint(CUS_Polar polar);

bool checkPointInRect(CUS_Point point, SDL_Rect rectangle);

/*Checks distance between two points is around less than a given float
Returns true if condition is met*/
static bool quickCheckTwoPointsDistance(CUS_Point a, CUS_Point b, float distance) {
	if (fabs(a.x - b.x) / 2 < distance) {
		if (fabs(a.y - b.y) / 2 < distance) {
			return true;
		}
		return false;
	}
	return false;
}

/*Sets an angle between 0 to 360*/
static float normaliseAngle(float angle) {
	if (angle <= 360 && angle >= 0)
		return angle;
	else if (angle < 0) {
		while (angle < 0)
			angle += 360;
		return angle;
	}
	else {
		while (angle > 360)
			angle -= 360;
		return angle;
	}
}

#endif
