#ifndef VIDEO_THREAD_H
#define VIDEO_THREAD_H

#include "ffms2_windows_x86/include/ffms.h"

#include <thread>
#include <atomic>
#include <vector>

#include <list>
#include <unordered_map>
#include <mutex>
#include <condition_variable>

struct FrameItem
{
	int width;
	int height;
	uint8_t* data;
};

struct CacheItem
{
	int index;
	FrameItem* FrameItem;
};

class VideoCacheThread
{
public:

	VideoCacheThread(FFMS_VideoSource* _videoSource, int _iBehind, int _iAhead);
	~VideoCacheThread()
	{
		for (auto it = m_cache.begin(); it != m_cache.end(); )
		{
			delete [] it->FrameItem->data;

			m_lookup.erase(it->index);
			it = m_cache.erase(it);
		}

		for (auto it = liFreeFrames.begin(); it != liFreeFrames.end(); )
		{
			delete [] (*it)->data;
			delete *it;
		}

	}

	void Start();
	void Stop();

	FrameItem* GetFrame(int index);

protected:

	FFMS_VideoSource* videoSource;

	void SetPlayhead(int frameInd);

	int idxPlayhead;

	std::atomic<bool> atomic_bRunning;

	std::thread Thread;

	std::mutex _mutex;
	std::condition_variable cvFrameIdChanged;

	int m_halfWindow;
	int m_hysteresis;

	int m_center;
	bool m_bCacheInitialized;

	std::vector<FrameItem*> liFreeFrames;

	std::list<CacheItem> m_cache;
	std::unordered_map<int, std::list<CacheItem>::iterator> m_lookup;

private:

	void Worker();

	void MaintainWindow(int index);
	void InsertIntoCache(int index);
	bool ExistsInCache(int i);
	FrameItem* LoadFrameFromStream(int index);
};



#endif