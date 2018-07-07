#include "cus_structs.h"

#define PI 3.14159265

CUS_Point CUS_Point::operator+(const CUS_Point& b) {
	CUS_Point ret;
	ret.x = this->x + b.x;
	ret.y = this->y + b.y;
	return ret;
}

void CUS_Point::operator+=(const CUS_Point& b) {
	this->x += b.x;
	this->y += b.y;
}

CUS_Point CUS_Point::operator*(const float& b) {
	CUS_Point ret;
	ret.x = this->x * b;
	ret.y = this->y * b;
	return ret;
}

float CUS_Point::toPolarAngle() {
	return (float)(atan2(x, y) * 180 / PI);
}

float CUS_Point::toPolarMagnitude() {
	return (float)sqrt(pow(x, 2) + pow(y, 2));
}

float CUS_Point::getAngleToPoint(CUS_Point in) {
	CUS_Point temp;
	temp.x = in.x - this->x;
	temp.y = in.y - this->y;
	return temp.toPolarAngle();
}

float CUS_Point::getDistanceToPoint(CUS_Point in) {
	double xdist;
	double ydist;
	xdist = pow(this->x - in.x, 2);
	ydist = pow(this->y - in.y, 2);
	return (float)sqrt(pow(this->x - in.x, 2) + pow(this->y - in.y, 2));
}

CUS_Point CUS_Polar::toPoint() {
	return { (float)(this->magnitude*sin(angle * PI / 180.0)) , (float)(this->magnitude*cos(angle * PI / 180.0)) };
}

CUS_Polar CUS_Polar::operator+(const CUS_Polar& in) {
	CUS_Point a = this->toPoint();

	CUS_Polar temp = in;
	CUS_Point b = temp.toPoint();

	return { (a + b).toPolarMagnitude(), (a + b).toPolarAngle() };
}

void CUS_Polar::operator+=(const CUS_Polar& in) {
	CUS_Point a = this->toPoint();

	CUS_Polar temp = in;
	CUS_Point b = temp.toPoint();

	a += b;

	this->magnitude = a.toPolarMagnitude();
	this->angle = a.toPolarAngle();
}

CUS_Polar CUS_Polar::operator*(const float& b) {
	return { magnitude * b, angle };
}

CUS_Polar CUS_Polar::operator*(const CUS_Polar& b) {
	return { magnitude * b.magnitude, angle + b.angle };
}