/*Loads all sounds at start of game at start of game (sound_ini()), deloads at end (sound_end())
*/
#include "error.h"

#include <SDL_mixer.h>
#ifndef __SOUND_H__
#define __SOUND_H__

//Enumeration of possible channels to use for sound
enum SoundChannel {
	SCbase,
	SCplayerFire,
	SCLightning,
	SCWeatherBackground
};

const int SCLength = 4;

void sound_ini();

//Sets global volume from 0 to 128
void setGlobalVolume(int newVolume);

//Sets a volume channel from 0 to 1 float
void setLocalVolume(SoundChannel soundChannel, float newValue);

//For global load, feed null pointer
void loadSoundAndMusic(LevelSettings* levelSettings);

//Frees local sound and music
void freeLocalSoundAndMusic();

//Simple call to play sound in given channel
void playSound(string soundName, SoundChannel channel);

//Alternative call that will continue to play looped on channel, must be stopped with stopChannel
void playLoopedSound(string soundName, SoundChannel channel);

//Stop the channel, Optional fade
void stopChannel(SoundChannel channel, int ms = 0);

//Stop all the channels
void stopAllSoundChannels();

//Plays music once
void playMusic(string music);

//Sets volume for music 0-128
void setMusicVolume(int volume);

//Stop music, optional fade
void stopMusic(int fade = 0);

//Checks if music track is in library
bool checkMusic(string name);

void sound_end();


#endif
