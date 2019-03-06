/*Bunch of useful string stuff*/

#ifndef __STR_KIT_H_INCLUDED__
#define __STR_KIT_H_INCLUDED__

#include <string>
#include <vector>

using namespace std;

/*Header for useful string manipulation*/
namespace str_kit {
	/*splits string into a vector of strings based on char token*/
	vector<string> splitOnToken(string line, char token);

	/*checks if the string is a digit*/
	bool isADigit(string line);

	/*Takes a float and formats it to score string: XX.XX% or XXX.XX%*/
	string convertToScoreString(float value, bool whole);
	
	/*Takes a string and replaces all instances token with insert */
	string replaceToken(string str, string token, string replacement);
	
}

#endif
