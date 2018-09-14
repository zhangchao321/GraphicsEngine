#ifndef _SOUND_H_
#define _SOUND_H_

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "winmm.lib")

#include <windows.h>
#include <mmsystem.h>
#include <dsound.h>
#include <stdio.h>

#include "Utils.h"

class Sound
{
private:
	struct WaveHeaderType
	{
		char chunkId[4];
		unsigned long chunkSize;
		char format[4];
		char subChunkId[4];
		unsigned long subChunkSize;
		unsigned short audioFormat;
		unsigned short numChannels;
		unsigned long sampleRate;
		unsigned long bytesPerSecond;
		unsigned short blockAlign;
		unsigned short bitsPerSample;
		char dataChunkId[4];
		unsigned long dataSize;
	};

public:
	Sound();
	Sound(const Sound& other);
	~Sound();

	bool Init(HWND hwnd);
	void Shutdown();

private:
	bool InitDirectSound(HWND hwnd);
	void ShutdownDirectSound();

	bool LoadWavFile(char* fileName, IDirectSoundBuffer** secondaryBuffer);
	void ShutdownWavFile(IDirectSoundBuffer** secondaryBuffer);

	bool PlayWavFile();

private:
	IDirectSound8* mDSound;
	IDirectSoundBuffer* mPrimaryBuffer;
	IDirectSoundBuffer* mSecondaryBuffer1;
};

#endif