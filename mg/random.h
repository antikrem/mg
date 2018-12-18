#ifndef __RANDOM_H__
#define __RANDOM_H__
#include <stdlib.h>
#include <ctime>
#include <string>

#include "constants.h"

namespace random {
	void primeRand();

	/*Random float between 0 and 1*/
	float randomFloat();

	/*Random int between a and b inclusive
	a < b or b < a is allowed*/
	int randomInt(int a, int b);


	/*Random float between a and b
	a < b or b < a is allowed*/
	float randomFloat(float a, float b);

	/*See above, different overload*/
	float randomFloat(int a, int b);

	//random sign
	int sign();

	//Hope for the best lol
	std::string randomString();
}

#endif