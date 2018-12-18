#ifndef __RANDOM_H__
#define __RANDOM_H__
#include <stdlib.h>
#include <ctime>
#include <string>

#include "constants.h"

namespace random {
	void primeRand() {
		srand((int)time(NULL));
	}

	/*Random float between 0 and 1*/
	float randomFloat() {
		return ((float)rand() / (float)RAND_MAX);
	}

	/*Random int between a and b inclusive
	a < b or b < a is allowed*/
	int randomInt(int a, int b) {
		if (a < b)
			return (int)(F(a) + randomFloat()*(F(b) + 1 - F(a)));
		else
			return (int)(F(b) + randomFloat()*(F(a)+  1 - F(b)));
	}


	/*Random float between a and b
	a < b or b < a is allowed*/
	float randomFloat(float a, float b) {
		if (a < b)
			return (a + randomFloat()*(b - a));
		else
			return (b + randomFloat()*(a - b));
	}

	/*See above, different overload*/
	float randomFloat(int a, int b) {
		return randomFloat((float) a, (float) b);
	}

	//random sign
	int sign() {
		return random::randomFloat() < (float) 0.5 ? -1 : 1;
	}

	//Hope for the best lol
	std::string randomString() {
		return std::to_string(random::randomFloat() * (random::randomFloat() + random::randomFloat()));
	}
}

#endif