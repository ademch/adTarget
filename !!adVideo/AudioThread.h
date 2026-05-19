
#ifndef AUDIO_THREAD_H
#define AUDIO_THREAD_H

#include "ffms2_windows_x86/include/ffms.h"

#include <thread>
#include <atomic>
#include "SoundAL.h"


class AudioThread
{
public:

	AudioThread(FFMS_AudioSource* _audioSource, ALuint _mSource, int iDynBufferCount);

	std::thread Thread;
	std::atomic<bool> atomic_bRunning = false;

	void ParseSourceProperties();
	void EnqueueInitialBuffers();

	void Start();
	void Stop();

protected:

	int iSampleRate;
	int iChannels;
	int iBytesPerSample;

	int iSamplesPerChunk;

	int64_t i64CurrentSample;

private:

	FFMS_AudioSource* audioSource;
	ALuint mSndSource;

	int iDynBufferCount;

	void Worker();
};

#endif