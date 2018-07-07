/*Dictionary Class*/
#ifndef __DICT_H_INCLUDED__
#define __DICT_H_INCLUDED__

#include <iostream>
#include <fstream>
#include <string>
#include <map>

using namespace std;

typedef map<string, string> mt_dict;

/*Associates a key(string) to value(string).
Can be loaded/read to/from a file*/
class Dictionary {
private:
	mt_dict dict_map;
	ifstream inFile;
	ofstream outFile;
	string filePath;
	void writeToFile();

public:
	Dictionary(const char* path, bool suppressError = false);
	/*Adds new entry, with some error handling (check for existing values etc)
	Writes to file syncronously, use outside of game loops/performance-centric environments*/
	void addEntry(std::string key, std::string value);
	/*edits old entry, with some error handling (check for existence etc)
	Writes to file syncronously, use outside of game loops/performance-centric environments*/
	void editEntry(std::string key, std::string value);
	string byKey(string key, bool suppressError = false);
	int byIntKey(string key);
	void toReport();
};

#endif