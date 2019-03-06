/*Handles all elements related to scripting levels*/
#include "str_kit.h"
#include "error.h"
#include <vector>

//returns an iterable list that replaces the lex
vector<string> createIterable(string str) {
	vector<string> iterable;
	vector<string> strVecCom = str_kit::splitOnToken(str, '(');
	//check that a command and set of parameters were given
	if (strVecCom.size() != 2) {
		err::logMessage("todo: script error");
		iterable.push_back("ERROR");
		return iterable;
	}
	
	//command type
	string iterableType = strVecCom[0];
	
	strVecCom[1].pop_back();
	vector<string> parameters = str_kit::splitOnToken(strVecCom[1], ',');
	
	
	if (strVecCom[0] == "RANGE") {
		//todo error checking
		int step;
		if (parameters.size() == 3)
			step = stoi(parameters[2]);
		else if (parameters.size() == 2)
			step = 1;
		else {
			iterable.push_back("ERROR");
			return iterable;
		}
			
		int start = stoi(parameters[0]);
		int end = stoi(parameters[1]);
		for (int i = start; i < end; i+= step) {
			iterable.push_back(to_string(i));
		}
	}
	
	return iterable;
}

/*Pre execution of a command line
this function resolves scripting elements such as 
for loop, and numeric operations
Calls recursivly before exit*/
vector<string> firstInterpret(string str) {
	if (!str.size()) {
		vector<string> empty;
		return empty;
	}
		
	vector<string> commandVec = str_kit::splitOnToken(str, ' ');
	//The vector of strings that will be returned
	vector<string> returnVec;
	
	//evaluate a for loop
	if (commandVec[0] == "FOR") {
		commandVec[3].pop_back();
		string lex = commandVec[1];
		string currentLine;
		vector<string> iterable = createIterable(commandVec[3]);
		//In a for loop the first 4 words are control
		//Since these for loop parameters are loaded, delete the first 4 elements
		for (int i = 0; i < 4; i++) {
			commandVec.erase(commandVec.begin());
		}

		for (auto i : iterable) {
			currentLine = str;
			str_kit::replaceToken(currentLine, lex, i);
			auto temp = firstInterpret(currentLine);
			returnVec.insert(returnVec.end(), temp.begin(), temp.end());
		}
	}

	else {
		returnVec.push_back(str);
	}
	return returnVec;
}
