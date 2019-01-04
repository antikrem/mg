/*Asyncronous error handling file.
Call primeErrorFile() at start, and endlog() at end*/
#ifndef __ERROR_H_INCLUDED__
#define __ERROR_H_INCLUDED__

#include <string>
#include "level_enum.h"

namespace err {
	//Needs to be run to prepare errorfile
	void primeErrorFile();
	//Flushes output stream and ends output file
	void endLog();

	//adds a string as a new line to error file directly
	//Not added to the error buffer and done synchronously
	void appendToErrorFile(std::string message);

	//update logging cycle
	void setLoggingCycle(int cycle);

	//Add a message to the error log buffer, which is pushed on occasion
	//Generic when logCycle is zero, else cycle is also noted
	void logMessage(std::string message);

	void logFPS(float);
	void logCPS(float);

	void logLevelStart(LevelSettings* level);
}

#endif