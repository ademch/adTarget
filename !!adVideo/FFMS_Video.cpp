#include "stdafx.h"
#include "FFMS_VIdeo.h"
#include <algorithm>


FFMS_Video::FFMS_Video()
{
	videoSource = NULL;
	audioSource = NULL;

	iCurrentFrame = 0;
	iTotalFrames  = 0;
}


FFMS_Video::~FFMS_Video()
{
	videoCacheThread->Stop();
	delete videoCacheThread;

	if (audioSource)
	{
		audioThread->Stop();
		delete audioThread;

		FFMS_DestroyAudioSource(audioSource);
	}
	if (videoSource)
		FFMS_DestroyVideoSource(videoSource);
}


void FFMS_Video::Initialize()
{
	printf("Initializing FFMS2...\n");

	FFMS_Init(0, 0);

	int v = FFMS_GetVersion();
	printf("    FFMS version %d.%d.%d.%d\n",(v >> 24) & 0xFF,
											(v >> 16) & 0xFF,
											(v >> 8)  & 0xFF,
											(v >> 0)  & 0xFF);
}


void FFMS_Video::LoadMPEG(const char* _filename)
{
	printf("Loading %s...\n", _filename);

	FFMS_Index* index;
	index = LoadMPEG_CreateIndex(_filename);

	printf("  Index contains %d tracks\n", FFMS_GetNumTracks(index));

	videoSource = LoadMPEG_CreateVideoSource(_filename, index);
	LoadMPEG_PrepareVideoFormat(videoSource);

	BuildPTSIndex(videoSource);

	audioSource = LoadMPEG_CreateAudioSource(_filename, index);
	LoadMPEG_PrepareAudioFormat(audioSource);

	if (audioSource)
	{
		// Create sound source
		alGenSources(1, &idSndSource);
		SoundAL::SetSourceDefaultParams(idSndSource);

		audioThread = new AudioThread(audioSource, idSndSource, 4);
		audioThread->ParseSourceProperties();
		audioThread->EnqueueInitialBuffers();
		//audioThread->Start();
	}

	videoCacheThread = new VideoCacheThread(videoSource, 20, 5);
	videoCacheThread->UpdateCacheWindow(0);
	videoCacheThread->Start();

}

FFMS_Index* FFMS_Video::LoadMPEG_CreateIndex(const char* sourcefile)
{
	char errmsg[1024];
	FFMS_ErrorInfo errinfo;
	errinfo.Buffer	   = errmsg;
	errinfo.BufferSize = sizeof(errmsg);

	FFMS_Indexer *indexer = FFMS_CreateIndexer(sourcefile, &errinfo);
	if (indexer == NULL)
	{
		printf("%s\n", errinfo.Buffer);
	}

	int iNumber = FFMS_GetNumTracksI(indexer);
	for (int i=0; i < iNumber; i++)
		printf("  Track %d codec: %s\n", i, FFMS_GetCodecNameI(indexer, i));

	FFMS_TrackTypeIndexSettings(indexer, FFMS_TYPE_AUDIO, 1, 0);

	// calling this function destroys the FFMS_Indexer object and frees the memory allocated by FFMS_CreateIndexer
	FFMS_Index *index = FFMS_DoIndexing2(indexer, FFMS_IEH_IGNORE, &errinfo);
	if (index == NULL)
	{
		printf("%s\n", errinfo.Buffer);
	}

	return index;
}

FFMS_VideoSource* FFMS_Video::LoadMPEG_CreateVideoSource(const char* sourcefile, FFMS_Index* index)
{
	char errmsg[1024];
	FFMS_ErrorInfo errinfo;
	errinfo.Buffer	   = errmsg;
	errinfo.BufferSize = sizeof(errmsg);

	/* Retrieve the track number of the first video track */
	int trackno = FFMS_GetFirstTrackOfType(index, FFMS_TYPE_VIDEO, &errinfo);
	if (trackno < 0)
	{
		// no video tracks found in the file, this is bad and you should handle it
		printf("%s\n", errinfo.Buffer);
	}

	printf("Track %d: VIDEO\n", trackno);

	/* We now have enough information to create the video source object */
	FFMS_VideoSource* videosource = FFMS_CreateVideoSource(sourcefile, trackno, index, 1, FFMS_SEEK_NORMAL, &errinfo);
	if (videosource == NULL)
	{
		printf("%s\n", errinfo.Buffer);
	}

	return videosource;
}

FFMS_AudioSource* FFMS_Video::LoadMPEG_CreateAudioSource(const char* sourcefile, FFMS_Index* index)
{
	char errmsg[1024];
	FFMS_ErrorInfo errinfo;
	errinfo.Buffer	   = errmsg;
	errinfo.BufferSize = sizeof(errmsg);

	/* Retrieve the track number of the first audio track */
	int trackno = FFMS_GetFirstTrackOfType(index, FFMS_TYPE_AUDIO, &errinfo);
	if (trackno < 0)
	{
		// no audio tracks found in the file
		printf("%s\n", errinfo.Buffer);
		return NULL;
	}

	printf("Track %d: AUDIO\n", trackno);

	/* We now have enough information to create the video source object */
	FFMS_AudioSource* audiosource = FFMS_CreateAudioSource(sourcefile, trackno, index, FFMS_DELAY_NO_SHIFT, &errinfo);
	if (audiosource == NULL)
	{
		printf("%s\n", errinfo.Buffer);
	}

	return audiosource;
}


void FFMS_Video::LoadMPEG_PrepareVideoFormat(FFMS_VideoSource* videosource)
{
	char errmsg[1024];
	FFMS_ErrorInfo errinfo;
	errinfo.Buffer	   = errmsg;
	errinfo.BufferSize = sizeof(errmsg);

	// Retrieve video properties so we know what we're getting
	const FFMS_VideoProperties *videoprops = FFMS_GetVideoProperties(videosource);

	/* Now you may want to do something with the info, like check how many frames the video has */
	iTotalFrames = videoprops->NumFrames;

	/* Get the first frame for examination so we know what we're getting. This is required
	because resolution and colorspace is a per frame property and NOT global for the video. */
	const FFMS_Frame *propframe = FFMS_GetFrame(videosource, 1, &errinfo);

	printf("    Video encoded resolution: %d x %d\n", propframe->EncodedWidth, propframe->EncodedHeight);
	printf("    Video encoded pixelformat: %d\n",	  propframe->EncodedPixelFormat);

	/* If you want to change the output colorspace or resize the output frame size,
	now is the time to do it. IMPORTANT: This step is also required to prevent
	resolution and colorspace changes midstream. You can you can always tell a frame's
	original properties by examining the Encoded* properties in FFMS_Frame. */
	/* See libavutil/pixfmt.h for the list of pixel formats/colorspaces.
	To get the name of a given pixel format, strip the leading PIX_FMT_
	and convert to lowercase. For example, PIX_FMT_YUV420P becomes "yuv420p". */

	/* A -1 terminated list of the acceptable output formats. */
	int pixfmts[2];
	pixfmts[0] = FFMS_GetPixFmt("rgba");
	pixfmts[1] = -1;

	if (FFMS_SetOutputFormatV2( videosource,
								pixfmts,
								propframe->EncodedWidth,
								propframe->EncodedHeight,
								FFMS_RESIZER_BICUBIC,
								&errinfo))
	{
		printf("%s\n", errinfo.Buffer);
	}

}

void FFMS_Video::LoadMPEG_PrepareAudioFormat(FFMS_AudioSource* audiosource)
{
	char errmsg[1024];
	FFMS_ErrorInfo errinfo;
	errinfo.Buffer	   = errmsg;
	errinfo.BufferSize = sizeof(errmsg);

	if (!audiosource) return;

	// Retrieve video properties so we know what we're getting
	const FFMS_AudioProperties *audioprops = FFMS_GetAudioProperties(audiosource);

	/* Now you may want to do something with the info, like check how many frames the video has */
	//int num_frames = videoprops->NumFrames;

	/* Get the first frame for examination so we know what we're getting. This is required
	because resolution and colorspace is a per frame property and NOT global for the video. */
	//const FFMS_Frame *propframe = FFMS_GetFrame(videosource, 0, &errinfo);

	printf("    Frequency: %d\n", audioprops->SampleRate);
	printf("    Samples: %lld\n", audioprops->NumSamples);
}


void FFMS_Video::BuildPTSIndex(FFMS_VideoSource* videoSource)
{
	FFMS_Track* videoTrack = FFMS_GetTrackFromVideo(videoSource);
	const FFMS_TrackTimeBase* tb = FFMS_GetTimeBase(videoTrack);
	
	int frameCount = FFMS_GetNumFrames(videoTrack);

	liIndex.reserve(frameCount);

	for (int i = 0; i < frameCount; i++)
	{
		const FFMS_FrameInfo* info = FFMS_GetFrameInfo(videoTrack, i);

		double fS = 0.001*(info->PTS*(double)tb->Num/(double)tb->Den);

		liIndex.push_back(fS);
	}

	return;
}


int FFMS_Video::NextIndexOrEqualFromTime(double fS)
{
	// lower_bound returns first element greater than or equal to fS
	auto it = std::lower_bound(liIndex.begin(), liIndex.end(), fS);

	if (it == liIndex.end())
		return -1;

	return (int)std::distance(liIndex.begin(), it);
}