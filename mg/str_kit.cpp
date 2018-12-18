#include "str_kit.h"

vector<string> str_kit::splitOnToken(string line, char token) {
	int length = (int)line.length();
	vector<string> returnVec;
	int lastToken = -1;

	for (int i = 1; i < length; i++) {
		if (line[i] == token) {
			returnVec.push_back(line.substr(lastToken + 1, i - lastToken - 1));
			lastToken = i;
		}
	}
	returnVec.push_back(line.substr(lastToken + 1));

	return returnVec;
}

bool str_kit::isADigit(string line) {
	for (auto character : line) {
		if ((int)character < 48 || (int)character > 57)
			return false;
	}
	return true;
}