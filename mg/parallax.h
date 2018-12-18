#ifndef __PARALLAX_H_INCLUDED__
#define __PARALLAX_H_INCLUDED__

//Value of shift

//calculates the horrizontal shift of chracters
namespace para {
	void setFOV(float newFov);

	float getFOV();

	void setDistanceFromPlane(float newDistanceFromPlane);

	float getDistanceFromPlane();

	/*Set new DOP based on fov and return*/
	float calcDistanceFromPlane();

	/*Set new fov based on DOP and return*/
	float calcFOV();

	/*Calculate shift in regards to player on plane*/
	int getShift(CUS_Point player);

	/*Calculate shift in regards to player on another plane*/
	int getShift(CUS_Point player, CUS_Point objectMid, float depth);

	/*Return size scale of object with current fov and DOP*/
	float getSizeScaler(float depth);

	/*Get extra sight width given depth
	e.g. how far extra to a side can you see at this depth*/
	float getExcessWidth(float depth);
}

#endif