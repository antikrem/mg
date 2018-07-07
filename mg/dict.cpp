#include "dict.h"
#include "error.h"
#include <string>


Dictionary::Dictionary(const char* path, bool suppressError) {
	filePath = path;
	inFile.open(filePath);
	if (!inFile && !suppressError) {
		err::logMessage("ERROR: unable to open file: " + filePath);
	}
	else if (!suppressError) {
		string line;
		while (getline(inFile, line)) {
			this->addEntry(line.substr(0, line.find(" ")), line.substr(line.find(" ") + 1, line.length()));
		}
	}
	inFile.close();
	this->writeToFile();
}

void Dictionary::writeToFile() {
	outFile.open(filePath);
	mt_dict::iterator it;
	for (it = dict_map.begin(); it != dict_map.end(); it++) {
		outFile << it->first << " " << it->second << endl;
	}
	outFile.close();
}

void Dictionary::addEntry(std::string key, std::string value) {
	if (dict_map[key] != "") {
		err::logMessage("ERROR: key: " + key + " already in use" + filePath);
	}
	else {
		dict_map[key] = value;
		this->writeToFile();
	}
}

void Dictionary::editEntry(std::string key, std::string value) {
	if (dict_map[key] == "") {
		err::logMessage("ERROR: key: " + key + " requested value edit for null entry, new entry created instead");
		this->addEntry(key, value);
	}
	else {
		dict_map[key] = value;
		this->writeToFile();
	}
}

string Dictionary::byKey(string key, bool suppressError) {
	if (dict_map[key] == "") {
		if (!suppressError) {
			err::logMessage("ERROR: key: " + key + " has no value");
		}
		return "";
	}
	else return dict_map[key];
}

int  Dictionary::byIntKey(string key) {
	return stoi( this->byKey(key) );
}

void Dictionary::toReport() {
	mt_dict::iterator it;
	for (it = dict_map.begin(); it != dict_map.end(); it++) {
		err::logMessage(it->first + " => " + it->second);
	}
}