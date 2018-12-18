#include "constants.h"
#include "cus_structs.h"
#include "parallax.h"
#include <cmath>

//TODO
#include <iostream>

namespace para {
	//Each unit is a pixel
	static float fov = 51.0f;

	static float distanceFromPlane = 1000.0f;

	void setFOV(float newFov) {
		fov = newFov;
		calcDistanceFromPlane();
	}

	float getFOV() {
		return fov;
	}

	void setDistanceFromPlane(float newDistanceFromPlane) {
		distanceFromPlane = newDistanceFromPlane;
		calcFOV();
	}

	float getDistanceFromPlane() {
		return distanceFromPlane;
	}

	float calcDistanceFromPlane() {
		distanceFromPlane = RENDERED_X / ( 2 * RAD2DEG( tan( DEG2RAD(fov)/2 ) ) );
		return distanceFromPlane;
	}

	float calcFOV() {
		fov = 2 * RAD2DEG( atan(RENDERED_X/(2*distanceFromPlane)) );
		return fov;
	}
	
	int getShift(CUS_Point pos) {
		return (int)(-1 * ((float)(pos.x) / (float)WORK_SPACE_X)*((float)WORK_SPACE_X - (float)RENDERED_X));
	}

	int getShift(CUS_Point player, CUS_Point objectMid, float depth) {
		int baseShift = getShift(player);
		int offset = (int)(getSizeScaler(depth) * (objectMid.x - player.x));
		return (offset + baseShift);
	}

	float getSizeScaler(float depth) {
		float realDepth = depth + getDistanceFromPlane();
		if (realDepth < 0)
			return 0;
		return RENDERED_X / (2*realDepth*tan(DEG2RAD(fov/2)));
	}

	float getExcessWidth(float depth) {
		float realDepth = depth + getDistanceFromPlane();
		return realDepth * tan(DEG2RAD(fov / 2));
	}
}