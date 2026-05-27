
#ifndef FFMS_VIDEO_H
#define FFMS_VIDEO_H

#include "ffms2_windows_x86/include/ffms.h"
#include "AudioThread.h"
#include "VideoThread.h"
#include "SoundAL.h"



class FFMS_Video
{
public:

	static void Initialize();

	FFMS_Video();
	~FFMS_Video();

	void LoadMPEG(const char* _filename);
	
	AudioThread*      audioThread;
	VideoCacheThread* videoCacheThread;

	FFMS_VideoSource* videoSource;
	FFMS_AudioSource* audioSource;

	int iCurrentFrame;

protected:

	FFMS_Index* LoadMPEG_CreateIndex(const char* sourcefile);

	FFMS_VideoSource* LoadMPEG_CreateVideoSource(const char* sourcefile, FFMS_Index* index);
	FFMS_AudioSource* LoadMPEG_CreateAudioSource(const char* sourcefile, FFMS_Index* index);

	void LoadMPEG_PrepareVideoFormat(FFMS_VideoSource* videosource);
	void LoadMPEG_PrepareAudioFormat(FFMS_AudioSource* audiosource);

	ALuint idSndSource;

};

#endif