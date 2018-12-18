#include "random.h"

void random::primeRand() {
	srand((int)time(NULL));
}

float random::randomFloat() {
	return ((float)rand() / (float)RAND_MAX);
}

int random::randomInt(int a, int b) {
	if (a < b)
		return (int)(F(a) + randomFloat()*(F(b) + 1 - F(a)));
	else
		return (int)(F(b) + randomFloat()*(F(a) + 1 - F(b)));
}

float random::randomFloat(float a, float b) {
	if (a < b)
		return (a + randomFloat()*(b - a));
	else
		return (b + randomFloat()*(a - b));
}

float random::randomFloat(int a, int b) {
	return randomFloat((float)a, (float)b);
}

int random::sign() {
	return random::randomFloat() < (float) 0.5 ? -1 : 1;
}

std::string random::randomString() {
	return std::to_string(random::randomFloat() * (random::randomFloat() + random::randomFloat()));
}