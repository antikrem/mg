#include "error.h"

#include <windows.h>
#include <fstream>
#include <iostream>

//remove
#include <stdlib.h>

#include <ctime>

#include <vector>
#include <thread>
#include <mutex>
#include <atomic>
#include <algorithm>

using namespace std;

namespace err {
	struct ErrorMessage {
		int cycle = 0;
		string message;
	};
	vector<ErrorMessage> messageBuffer;
	mutex writeLock;
	atomic<bool> safeToEnd = false;
	atomic<bool> logActive = false;
	atomic<int> loggingCycle = 0;

	string currentTime() {
		time_t t = time(0);
		tm* now = NULL;
		now = localtime(&t);
		return asctime(now);
	}

	void setLoggingCycle(int cycle) {
		loggingCycle.store(cycle);
	}

	void appendToErrorFile(std::string message) {
		std::ofstream outfile;

		outfile.open("log.txt", std::ios_base::app);
		outfile << message << std::endl;
		outfile.close();
	}

	void appendToErrorFile(int cycle, std::string message) {
		std::ofstream outfile;

		outfile.open("log.txt", std::ios_base::app);
		outfile << cycle << ": " << message << std::endl;
		outfile.close();
	}

	void loggingDuty() {
		while (logActive) {
		//	system("CLS");
		//	cout << "BufferSize " << messageBuffer.size() << endl;
			//If safe to write and there is a message in the buffer
			if (messageBuffer.size()) {
				//Locks safety and adds to file
				writeLock.lock();
				for (ErrorMessage i : messageBuffer) {
					if (i.cycle == 0) {
						appendToErrorFile(i.message);
					}
					else {
						appendToErrorFile(i.cycle, i.message);
					}
				}
				messageBuffer.clear();
				writeLock.unlock();
			}
		}

		safeToEnd = true;
	}

	void primeErrorFile() {
	 	std::ofstream outfile;
		outfile.open("log.txt", std::ofstream::out | std::ofstream::trunc);
		outfile << "LOG FILE FOR 2tick+1-INDEV INSTANCE RAN ON: " << currentTime () << std::endl;
		outfile.close();

		logActive = true;
		std::thread logThread(&err::loggingDuty);
		logThread.detach();
	}

	void endLog() {
		logActive = false;
		int meme = 0;
		while (!safeToEnd) {
			meme++;
		};

		writeLock.lock();
		std::ofstream outfile;
		outfile.open("log.txt", std::ios_base::app);
		outfile << "SAFELY EXITED AT: " << currentTime() << std::endl;
		outfile << meme << std::endl;
		outfile.close();
		writeLock.unlock();
	}

	void logMessage(std::string message) {
		ErrorMessage toPush = { loggingCycle.load(), message };
		writeLock.lock();
		messageBuffer.push_back(toPush);
		writeLock.unlock();
	}

	void logFPS(float fps) {
		appendToErrorFile("Average FPS was: " + std::to_string(fps));
	}

	void logCPS(float cps) {
		appendToErrorFile("Average CPS was: " + std::to_string(cps));
	}

	void logLevelStart(LevelSettings level) {
		appendToErrorFile("LEVEL STARTED WITH THE FOLLOWING SETTINGS:");
		appendToErrorFile("Level:" + std::to_string(level.level));
		switch (level.currentCharacter) {
		case first:
			appendToErrorFile("Character: first"); break;
		case second:
			appendToErrorFile("Character: second"); break;
		case third:
			appendToErrorFile("Character: third"); break;
		}
		appendToErrorFile("");
	}
}