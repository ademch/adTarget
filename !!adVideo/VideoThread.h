#ifndef VIDEO_THREAD_H
#define VIDEO_THREAD_H

#include "ffms2_windows_x86/include/ffms.h"

#include <thread>
#include <atomic>
#include <list>
#include <map>
#include <mutex>


struct FrameItem
{
	int      width;
	int      height;
	uint8_t* data;
};


class VideoCacheThread
{
public:

	VideoCacheThread(FFMS_VideoSource* _videoSource, int _iBehind, int _iAhead);
	~VideoCacheThread();

	void Start();
	void Stop();

	FrameItem* GetFrame(int index);
	void UpdateCacheWindow(int index);

protected:

	FFMS_VideoSource* videoSource;

	void SetPlayhead(int frameInd);

	int idxPlayhead;

	std::atomic<bool> atomic_bRunning;
	std::atomic<bool> atomic_bNewRequest;

	std::thread Thread;

	std::mutex _mutex_mapExclusiveAccess;

	int m_halfWindow;
	int m_hysteresis;

	int m_center;
	bool m_bCacheInitialized;

	std::list<FrameItem*> liFreeFrames;

	std::map<int, FrameItem*> m_cache;

private:

	int iTotalFrames;

	void Worker();

	void InsertIntoCache(int index);
	bool ExistsInCache(int i);

	FrameItem* LoadFrameFromStream(int index);
};


#endif