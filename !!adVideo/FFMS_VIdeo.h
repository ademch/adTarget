
#ifndef FFMS_VIDEO_H
#define FFMS_VIDEO_H

#include "ffms2_windows_x86/include/ffms.h"
#include "AudioThread.h"
#include "VideoThread.h"
#include "SoundAL.h"



class FFMS_Video
{
public:

	FFMS_Video()
	{
		videoSource = NULL;
		audioSource = NULL;
	}
	~FFMS_Video()
	{
		videoCacheThread->Stop();
		delete videoCacheThread;

		audioThread->Stop();
		delete audioThread;

		if (audioSource) FFMS_DestroyAudioSource(audioSource);
		if (videoSource) FFMS_DestroyVideoSource(videoSource);
	}

	void Initialize();

	void LoadMPEG(const char* _filename);
	
protected:

	FFMS_VideoSource* videoSource;
	FFMS_AudioSource* audioSource;

	FFMS_Index* LoadMPEG_CreateIndex(const char* sourcefile);

	FFMS_VideoSource* LoadMPEG_CreateVideoSource(const char* sourcefile, FFMS_Index* index);
	FFMS_AudioSource* LoadMPEG_CreateAudioSource(const char* sourcefile, FFMS_Index* index);

	void LoadMPEG_PrepareVideoFormat(FFMS_VideoSource* videosource);
	void LoadMPEG_PrepareAudioFormat(FFMS_AudioSource* audiosource);

protected:

	ALuint idSndSource;

	AudioThread*      audioThread;
	VideoCacheThread* videoCacheThread;
};

#endif