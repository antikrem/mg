#include "sound.h"
#include "constants.h"

#include <string>
#include <map>
#include <iostream>
#include <vector>

#include <windows.h>

/*A structure that contains a sound piece and metadata*/
struct Sound {
	//If true, delete on end of campaign
	bool campaignLoad = false;
	union Data {
		Mix_Music* music;
		Mix_Chunk* sound;
	} data;
};

//Sound lists
map< string, Sound > musicList;
map< string, Sound > soundList;

//Volume control for globally
int volumeGlobal = 128;
//Channel base volume control, from 0 to 1 float
float volumeChannel[SCLength];
//Volume of music
int volumeMusic = 128;

//Resets every channel volume in reference to global volume and individual volume
void setAllVolumeChannels() {
	for (int i = 0; i < SCLength; i++) {
		int newVolume = (newVolume = (int)(volumeChannel[i] * F(volumeGlobal)))>=128 ? 128 : newVolume;
		cout << "new volume " << newVolume << endl;
		Mix_Volume(i, newVolume);
	}
}

void sound_ini() {
	//Initialize SDL_mixer
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0) {
		err::logMessage("Sounds systems failed to initialise");
	}

	Mix_Volume(-1, volumeGlobal);
	for (int i = 0; i < SCLength; i++)
		volumeChannel[i] = 1.0f;
	//Default volume values
	volumeChannel[SCplayerFire] = 0.3f;
	setAllVolumeChannels();
}

void setGlobalVolume(int newVolume) {
	if not(newVolume <= 128 && newVolume >= 0) {
		err::logMessage("A global volume was added which was not in the range of 0 to 128 whole number");
		return;
	}
	volumeGlobal = newVolume;

}

void setLocalVolume(SoundChannel soundChannel, float newValue) {
	if not(soundChannel < SCLength && soundChannel >= 0) {
		err::logMessage("A Sound Channel was attempted to be changed with a channel that doesn't exist, valid channels are from 0 to " + to_string(SCLength - 1));
		return;
	}
	if not(newValue >=0 && newValue <= 1.0f) {
		err::logMessage("A Sound Channel was attempted to be changed to a value outside of 0 to 1");
		return;
	}
	volumeChannel[soundChannel] = newValue;
	int newVolume = (newVolume = (int)(volumeChannel[soundChannel] * F(volumeGlobal))) >= 128 ? 128 : newVolume;
	cout << "new volume " << newVolume << endl;
	Mix_Volume(soundChannel, newVolume);
}

void loadSoundAndMusic(LevelSettings* levelSettings) {
	string basePath;

	if (!levelSettings)
		basePath = "assets\\sound\\";

	HANDLE fileHandle;
	WIN32_FIND_DATA data;
	string searchPath = basePath + "*.wav";
	fileHandle = FindFirstFile(searchPath.c_str(), &data);
	if (fileHandle != INVALID_HANDLE_VALUE) {
		do {
			string file = data.cFileName;
			string fileContents = basePath + file;
			file = file.substr(0, file.size() - 4);

			auto sample = Mix_LoadWAV(fileContents.c_str());
			if (!sample) {
				err::logMessage("ERROR: A sound file was not able to be launched: " + file + " at " + fileContents);
				string error = Mix_GetError();
				err::logMessage(error);
			}
			else {
				Sound push;
				push.campaignLoad = false;
				push.data.sound = sample;
				soundList[file] = push;
			}
		} while (FindNextFile(fileHandle, &data));
		FindClose(fileHandle);
	}

	if (!levelSettings)
		basePath = "assets\\music\\";

	//Load music of a few formats
	vector<string> seachParameters = { "*.wav" , "*.mp3", "*.flac" };
	for (auto para : seachParameters) {
		searchPath = basePath + para;
		fileHandle = FindFirstFile(searchPath.c_str(), &data);
		if (fileHandle != INVALID_HANDLE_VALUE) {
			do {
				string file = data.cFileName;
				string fileContents = basePath + file;
				file = file.substr(0, file.size() - 4);

				auto sample = Mix_LoadMUS(fileContents.c_str());
				if (!sample) {
					err::logMessage("ERROR: A sound file was not able to be launched: " + file + " at " + fileContents);
					string error = Mix_GetError();
					err::logMessage(error);
				}
				else {
					Sound push;
					push.campaignLoad = false;
					push.data.music = sample;
					if (musicList.count(file)) {
						err::logMessage("ERROR: A sound file was included with the same name but different extension: " + file);
						Mix_FreeMusic(push.data.music);
					}
					else {
						musicList[file] = push;
					}
				}
			} while (FindNextFile(fileHandle, &data));
			FindClose(fileHandle);
		}
	}

}

void playSound(string soundName, SoundChannel channel) {
	if (soundList.count(soundName)){
		int newVolume = (newVolume = (int)(volumeChannel[channel] * F(volumeGlobal))) >= 128 ? 128 : newVolume;
		Mix_Volume(channel, newVolume);
		Mix_PlayChannel(channel, soundList[soundName].data.sound, 0);
	}
	else
		err::logMessage("A sound file isn't availible when called for play: " + soundName);
}

void playLoopedSound(string soundName, SoundChannel channel) {
	int newVolume = (newVolume = (int)(volumeChannel[channel] * F(volumeGlobal))) >= 128 ? 128 : newVolume;
	Mix_Volume(channel, newVolume);
	if (soundList.count(soundName))
		Mix_PlayChannel(channel, soundList[soundName].data.sound, -1);
	else
		err::logMessage("A sound file isn't availible when called for play: " + soundName);
}

void stopChannel(SoundChannel channel, int ms) {
	if (!ms)
		Mix_HaltChannel(channel);
	else
		Mix_FadeOutChannel(channel, ms);
}

void playMusic(string music) {
	if not(musicList.count(music)) {
		err::logMessage("A music file isn't availible when called for play: " + music);
		return;
	}
	Mix_PlayMusic(musicList[music].data.music, 0);
	Mix_VolumeMusic(volumeMusic);
}

void setMusicVolume(int volume) {
	volumeMusic = volume;
	Mix_VolumeMusic(volumeMusic);
}

void stopMusic(int fade) {
	int fadeTime = (int)(F(fade * 10) / 3);
	cout << fadeTime << endl;
	Mix_FadeOutMusic(fadeTime);
}

void sound_end() {
	Mix_HaltChannel(-1);
	Mix_HaltMusic();
	for (auto i : soundList) {
		Mix_FreeChunk(i.second.data.sound);
	}
	for (auto i : musicList) {
		Mix_FreeMusic(i.second.data.music);
	}
	Mix_Quit();
}

