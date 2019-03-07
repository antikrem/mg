#include "str_kit.h"
#include "constants.h"
#include <iostream>

vector<string> str_kit::splitOnToken(string line, char token) {
	if (line.size()) {
		while (line.back() == ' ') {
			line.pop_back();
		}
	}

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

string str_kit::convertToScoreString(float value, bool whole) {
	string score;
	string source = to_string(value);
	string wholeNumber = source.substr(0, source.find('.'));
	if (wholeNumber.length() > 1)
		score.append(wholeNumber);
	else {
		score.append("0");
		score.append(wholeNumber);
	}
	if (whole)
		return score+"%";
	score.append(".");
	score.append( source.substr(source.find('.')+1, 2) );
	return score + "%";
}

string str_kit::replaceToken(string str, string token, string replacement) {
	string rstr = str;

	size_t location = rstr.find(token);

	while (location != string::npos) {
		rstr.replace(location, token.size(), replacement);
		location = rstr.find(token);
	}

	return rstr;
}

string str_kit::reconstituteVectorIntoString(vector<string> input, string filler, int ignorePreceding) {
	vector<string> temp = input;
	if (ignorePreceding) {
		for (int i = 0; i < ignorePreceding; i++) {
			temp.erase(temp.begin());
		}
	}

	string str;
	bool flip = false;
	for (auto i : temp) {
		if not(flip)
			flip = true;
		else
			str.append(filler);

		str.append(i);
	}

	return str;
}
