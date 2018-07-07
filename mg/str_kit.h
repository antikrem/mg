/*Bunch(one) of useful string stuff*/

#ifndef __STR_KIT_H_INCLUDED__
#define __STR_KIT_H_INCLUDED__

#include <string>
#include <vector>

using namespace std;

namespace str_kit {
	/*splits string into a vector of strings based on char token*/
	vector<string> static str_kit::splitOnToken(string line, char token) {
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
}

#endif