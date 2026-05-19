#include "stdafx.h"
#include "AudioThread.h"
#include <chrono>
#include <vector>
#include <emmintrin.h>   // SSE2


const int bytesPerSampleTable[] =
{
	sizeof(uint8_t), // FFMS_FMT_U8
	sizeof(int16_t), // FFMS_FMT_S16
	sizeof(int32_t), // FFMS_FMT_S32
	sizeof(float),   // FFMS_FMT_FLT
	sizeof(double)   // FFMS_FMT_DBL
};


// input:  4 float samples in range roughly [-1, +1]
// output: 4 int16 samples
static inline void Float4ToInt16_SSE(const float* in, int16_t* out)
{
	// load 4 floats
	__m128 v = _mm_loadu_ps(in);

	// clamp to [-1.0, 1.0]
	const __m128 minv  = _mm_set1_ps(-1.0f);
	const __m128 maxv  = _mm_set1_ps( 1.0f);

	v = _mm_max_ps(v, minv);
	v = _mm_min_ps(v, maxv);

	// scale
	const __m128 scale = _mm_set1_ps(32767.0f);
	v = _mm_mul_ps(v, scale);

	// float -> int32
	__m128i i32 = _mm_cvtps_epi32(v);

	// pack 4x int32 -> 4x int16 with saturation
	__m128i i16 = _mm_packs_epi32(i32, i32);

	// store lower 64 bits = 4 int16 samples
	_mm_storel_epi64((__m128i*)out, i16);
}


AudioThread::AudioThread(FFMS_AudioSource* _audioSource, ALuint _mSource, int _iDynBufferCount)
{
	audioSource      = _audioSource;
	mSndSource       = _mSource;
	iDynBufferCount  = _iDynBufferCount;

	i64CurrentSample = 0;
}

void AudioThread::ParseSourceProperties()
{
	const FFMS_AudioProperties *audioprops = FFMS_GetAudioProperties(audioSource);

	iBytesPerSample  = bytesPerSampleTable[audioprops->SampleFormat];
	iSampleRate      = audioprops->SampleRate;
	iChannels        = audioprops->Channels;
	iSamplesPerChunk = iSampleRate/10; // 100ms @ 48kHz
}

void AudioThread::Start()
{
	atomic_bRunning = true;
	Thread = std::thread(&AudioThread::Worker, this);
}

void AudioThread::Stop()
{
	atomic_bRunning = false;

	if (Thread.joinable())
		Thread.join();
}

void AudioThread::EnqueueInitialBuffers()
{
	char errmsg[1024];
	FFMS_ErrorInfo err;
	err.Buffer     = errmsg;
	err.BufferSize = sizeof(errmsg);

	std::vector<float>   pcmFloat(iSamplesPerChunk * iChannels);
	std::vector<int16_t> pcmInt16(iSamplesPerChunk * iChannels);

	unsigned int buffer_id;

	for (int i = 0; i < iDynBufferCount; i++)
	{
		// Create buffers
		alGenBuffers(1, &buffer_id);
		if (!SoundAL::CheckALError())
		{	
			printf("Out of buffers error occured in Load\n");
			return;
		}

		if ( FFMS_GetAudio(audioSource, pcmFloat.data(), i64CurrentSample, iSamplesPerChunk, &err) )
		{
			printf("FFMS audio error: %s\n", errmsg);

			atomic_bRunning = false;
			break;
		}
		i64CurrentSample += iSamplesPerChunk;

		int iCalls = iSamplesPerChunk * iChannels;
		for (int i = 0; i < iCalls; i+=4)
		{
			Float4ToInt16_SSE(&pcmFloat[i], &pcmInt16[i]);
		}

		int chunkBytes = iSamplesPerChunk * iChannels * sizeof(int16_t);
		alBufferData(buffer_id, iChannels == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16, pcmInt16.data(), chunkBytes, iSampleRate);

		// Place buffer to queue
		alSourceQueueBuffers(mSndSource, 1, &buffer_id);
		if (!SoundAL::CheckALError())
		{	
			printf("Error occured while queueing buffers\n");
			return;
		}
	}
}

void AudioThread::Worker()
{
	char errmsg[1024];
	FFMS_ErrorInfo err;
	err.Buffer     = errmsg;
	err.BufferSize = sizeof(errmsg);

	std::vector<float>   pcmFloat(iSamplesPerChunk * iChannels);
	std::vector<int16_t> pcmInt16(iSamplesPerChunk * iChannels);

	while (atomic_bRunning)
	{
		ALint processed = 0;
		alGetSourcei(mSndSource, AL_BUFFERS_PROCESSED, &processed);

		while (processed > 0)
		{
			ALuint buf = 0;
			alSourceUnqueueBuffers(mSndSource, 1, &buf);

			if ( FFMS_GetAudio(audioSource, pcmFloat.data(), i64CurrentSample, iSamplesPerChunk, &err) )
			{
				printf("FFMS audio error: %s\n", errmsg);

				atomic_bRunning = false;
				break;
			}
			i64CurrentSample += iSamplesPerChunk;

			int iCalls = iSamplesPerChunk * iChannels;
			for (int i = 0; i < iCalls; i+=4)
			{
				Float4ToInt16_SSE(&pcmFloat[i], &pcmInt16[i]);
			}

			int chunkBytes = iSamplesPerChunk * iChannels * 2;
			alBufferData( buf, iChannels == 2 ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16, pcmInt16.data(), chunkBytes, iSampleRate);

			alSourceQueueBuffers( mSndSource, 1, &buf);

			processed--;
		}

		ALint state = 0;
		alGetSourcei( mSndSource, AL_SOURCE_STATE, &state);

		if (state != AL_PLAYING)
		{
			ALint queued = 0;
			alGetSourcei(mSndSource, AL_BUFFERS_QUEUED, &queued);

			if (queued > 0)
				alSourcePlay(mSndSource);
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(50));
	}
}


