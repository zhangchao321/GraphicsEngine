#include "Sound.h"

Sound::Sound()
{
	mDSound = nullptr;
	mPrimaryBuffer = nullptr;
	mSecondaryBuffer1 = nullptr;
}

Sound::Sound(const Sound& other)
{
}

Sound::~Sound()
{
}

bool Sound::Init(HWND hwnd)
{
	 //Check(InitDirectSound(hwnd));
	 ////You could try sound01.wav to test.
	 //Check(LoadWavFile("Musics/DeepSeaGirl.wav", &mSecondaryBuffer1));
	 //Check(PlayWavFile());

	return true;
}

void Sound::Shutdown()
{
	ShutdownWavFile(&mSecondaryBuffer1);
	ShutdownDirectSound();
}

bool Sound::InitDirectSound(HWND hwnd)
{
	HR(DirectSoundCreate8(NULL, &mDSound, NULL));
	HR(mDSound->SetCooperativeLevel(hwnd, DSSCL_PRIORITY));

	DSBUFFERDESC dsbDesc;
	dsbDesc.dwSize = sizeof(DSBUFFERDESC);
	dsbDesc.dwFlags = DSBCAPS_PRIMARYBUFFER | DSBCAPS_CTRLVOLUME;
	dsbDesc.dwBufferBytes = 0;
	dsbDesc.dwReserved = 0;
	dsbDesc.guid3DAlgorithm = GUID_NULL;
	dsbDesc.lpwfxFormat = NULL;
	
	HR(mDSound->CreateSoundBuffer(&dsbDesc, &mPrimaryBuffer, NULL));

	WAVEFORMATEX waveFormat;
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	HR(mPrimaryBuffer->SetFormat(&waveFormat));

	return true;
}

void Sound::ShutdownDirectSound()
{
	ReleaseCOM(mPrimaryBuffer);
	ReleaseCOM(mDSound);
}

bool Sound::LoadWavFile(char* fileName, IDirectSoundBuffer** secondaryBuffer)
{
	FILE* filePtr;
	int error;
	error = fopen_s(&filePtr, fileName, "rb");
	if (error != 0)
	{
		MessageBox(NULL, L"Can't open wav file.", L"LoadWavFile", NULL);
		return false;
	}

	// First, Check for WaveFileHeader to  
	// verify that we can setup the secondaryBuffer we will load the audio data onto.
	int count;
	WaveHeaderType waveFileHeader;
	count = fread(&waveFileHeader, sizeof(waveFileHeader), 1, filePtr);
	if (count != 1)
	{
		MessageBox(NULL, L"Read wav file error.", L"LoadWavFile", NULL);
		return false;
	}

	if ((waveFileHeader.chunkId[0] != 'R') || (waveFileHeader.chunkId[1] != 'I') ||
		(waveFileHeader.chunkId[2] != 'F') || (waveFileHeader.chunkId[3] != 'F'))
	{
		MessageBox(NULL, L"Chunk id is not RIFF format.", L"LoadWavFile", NULL);
		return false;
	}

	if ((waveFileHeader.format[0] != 'W') || (waveFileHeader.format[1] != 'A') ||
		(waveFileHeader.format[2] != 'V') || (waveFileHeader.format[3] != 'E'))
	{
		MessageBox(NULL, L"File format is not wav.", L"LoadWavFile", NULL);
		return false;
	}

	if ((waveFileHeader.subChunkId[0] != 'f') || (waveFileHeader.subChunkId[1] != 'm') ||
		(waveFileHeader.subChunkId[2] != 't') || (waveFileHeader.subChunkId[3] != ' '))
	{
		MessageBox(NULL, L"Subchunk ID is not the fmt format.", L"LoadWavFile", NULL);
		return false;
	}

	if (waveFileHeader.audioFormat != WAVE_FORMAT_PCM)
	{
		MessageBox(NULL, L"Audio format is not WAVE_FORMAT_PCM.", L"LoadWavFile", NULL);
		return false;
	}

	if (waveFileHeader.numChannels != 2)
	{
		MessageBox(NULL, L"The number of channels is not 2.", L"LoadWavFile", NULL);
		return false;
	}

	if (waveFileHeader.sampleRate != 44100)
	{
		MessageBox(NULL, L"Sample rate is not 44.1 KHz.", L"LoadWavFile", NULL);
		return false;
	}

	if (waveFileHeader.bitsPerSample != 16)
	{
		MessageBox(NULL, L"BitsPerSample is not 16.", L"LoadWavFile", NULL);
		return false;
	}

	if ((waveFileHeader.dataChunkId[0] != 'd') || (waveFileHeader.dataChunkId[1] != 'a') ||
		(waveFileHeader.dataChunkId[2] != 't') || (waveFileHeader.dataChunkId[3] != 'a'))
	{
		MessageBox(NULL, L"DataChunkId is not data.", L"LoadWavFile", NULL);
		return false;
	}

	// Second, creat secondaryBuffer
	WAVEFORMATEX waveFormat;
	waveFormat.wFormatTag = WAVE_FORMAT_PCM;
	waveFormat.nSamplesPerSec = 44100;
	waveFormat.wBitsPerSample = 16;
	waveFormat.nChannels = 2;
	waveFormat.nBlockAlign = (waveFormat.wBitsPerSample / 8) * waveFormat.nChannels;
	waveFormat.nAvgBytesPerSec = waveFormat.nSamplesPerSec * waveFormat.nBlockAlign;
	waveFormat.cbSize = 0;

	DSBUFFERDESC dsbDesc;
	dsbDesc.dwSize = sizeof(DSBUFFERDESC);
	dsbDesc.dwFlags = DSBCAPS_CTRLVOLUME;
	dsbDesc.dwBufferBytes = waveFileHeader.dataSize;
	dsbDesc.dwReserved = 0;
	dsbDesc.lpwfxFormat = &waveFormat;
	dsbDesc.guid3DAlgorithm = GUID_NULL;

	IDirectSoundBuffer* tempBuffer = nullptr;
	// Create a temporary sound buffer with the specific buffer settings.
	HR(mDSound->CreateSoundBuffer(&dsbDesc, &tempBuffer, NULL));
	// Test the buffer format against the direct sound 8 interface and create the secondary buffer.
	HR(tempBuffer->QueryInterface(IID_IDirectSoundBuffer8, (void**)&*secondaryBuffer));
	ReleaseCOM(tempBuffer);

	// Move to the beginning of the wave data which starts at the end of the data chunk header.
	fseek(filePtr, sizeof(WaveHeaderType), SEEK_SET);

	// Read in the wave file data into the newly created buffer.
	unsigned char* waveData = new unsigned char[waveFileHeader.dataSize];
	count = fread(waveData, 1, waveFileHeader.dataSize, filePtr);

	if (count != waveFileHeader.dataSize)
	{
		return false;
	}

	error = fclose(filePtr);
	if (error != 0)
	{
		return false;
	}

	// Lock the secondary buffer to write wave data into it.
	unsigned char* bufferPtr = nullptr;
	unsigned long bufferSize;
	HR((*secondaryBuffer)->Lock(0, waveFileHeader.dataSize, (void**)&bufferPtr, (DWORD*)&bufferSize, NULL, 0, 0));
	
	// Copy the wave data into the buffer.
	memcpy(bufferPtr, waveData, waveFileHeader.dataSize);

	// Unlock the secondary buffer after the data has been written to it.
	HR((*secondaryBuffer)->Unlock((void*)bufferPtr, bufferSize, NULL, 0));

	delete[] waveData;
	waveData = nullptr;

	return true;
}

void Sound::ShutdownWavFile(IDirectSoundBuffer** secondaryBuffer)
{
	ReleaseCOM((*secondaryBuffer));
}

// The PlayWaveFile function will play the audio file stored in the secondary buffer.
// It will automatically mix the audio into the primary buffer and start it playing if it wasn't already.
// Note that we set the position to start playing at the beginning of the secondary sound buffer otherwise it will continue from where it last stopped playing.
bool Sound::PlayWavFile()
{
	// Set position at the beginning of the sound buffer.
	HR(mSecondaryBuffer1->SetCurrentPosition(0));
	// Set volume of the buffer to 100%.
	HR(mSecondaryBuffer1->SetVolume(DSBVOLUME_MAX));

	HR(mSecondaryBuffer1->Play(0, 0, DSBPLAY_LOOPING));

	return true;
}