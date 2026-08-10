#include "stdafx.h"
#include "VideoThread.h"
#include <assert.h>


VideoCacheThread::VideoCacheThread(FFMS_VideoSource* _videoSource, int maxItems, int hysteresis)
{   
	videoSource    = _videoSource;

	const FFMS_VideoProperties *videoprops = FFMS_GetVideoProperties(videoSource);
	iTotalFrames   = videoprops->NumFrames;

	m_halfWindow   = maxItems/2;
	m_hysteresis   = hysteresis;
	m_center       = 0;

	m_bCacheInitialized  = false;

	idxPlayhead	   = 0;

	atomic_bRunning    = false;
	atomic_bNewRequest = false;
}

VideoCacheThread::~VideoCacheThread()
{
	// free cached frames
	for (auto it = m_cache.begin(); it != m_cache.end(); )
	{
		delete [] it->second->data;
		delete it->second;
		it = m_cache.erase(it);
	}

	// free frames from pool
	for (auto p : liFreeFrames)
	{
		delete[] p->data;
		delete p;
	}
}


void VideoCacheThread::Start()
{
	atomic_bRunning = true;
	Thread = std::thread(&VideoCacheThread::Worker, this);
}

void VideoCacheThread::Stop()
{
	atomic_bRunning = false;

	if (Thread.joinable())
	{
		Thread.join();
	}
}


void VideoCacheThread::SetPlayhead(int _idxPlayhead)
{
	idxPlayhead = _idxPlayhead;
	atomic_bNewRequest = true;
}


void VideoCacheThread::Worker()
{
	while (atomic_bRunning)
	{
		if (atomic_bNewRequest)
		{
			if (!UpdateCacheWindow(idxPlayhead)) continue;

			atomic_bNewRequest = false;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

bool VideoCacheThread::UpdateCacheWindow(int index)
{
	if (!m_bCacheInitialized)
	{
		m_center = index;
		m_bCacheInitialized = true;
	}
	else if (std::abs(index - m_center) > m_hysteresis)
	{
		m_center = index;
	}

	int iLow  = m_center - m_halfWindow;
	int iHigh = m_center + m_halfWindow;

	if (iLow < 0) iLow = 0;

	_mutex_mapExclusiveAccess.lock();

		// 1. EVICT outside window
		for (auto it = m_cache.begin(); it != m_cache.end(); )
		{
			if (it->first < iLow || it->first > iHigh)
			{
				// return FrameItem to the pull of buffers
				liFreeFrames.emplace_back(it->second);
				it = m_cache.erase(it);
			}
			else
			{
				++it;
			}
		}

	_mutex_mapExclusiveAccess.unlock();

	// 2. LOAD missing (iLow -> iHigh ensures correct order)
	for (int i = m_center; i <= iHigh && i <= iTotalFrames-1; ++i)
	{
		// check for user frame to be loaded during this batch
		if ((idxPlayhead < iLow) || (idxPlayhead > iHigh))
		{
			printf("cache: rebasing on %i\n", idxPlayhead);
			return false;
		}
		
		if (!ExistsInCache(i))
			InsertIntoCache(i);
	}

	for (int i = iLow; i < m_center && i <= iTotalFrames-1; ++i)
	{
		// check for user frame to be loaded during this batch
		if ((idxPlayhead < iLow) || (idxPlayhead > iHigh))
		{
			printf("cache: rebasing on %i\n", idxPlayhead);
			return false;
		}

		if (!ExistsInCache(i))
			InsertIntoCache(i);
	}

	return true;
}

bool VideoCacheThread::ExistsInCache(int i)
{
	std::lock_guard<std::mutex> lock(_mutex_mapExclusiveAccess);

		return (m_cache.find(i) != m_cache.end());
}


void VideoCacheThread::InsertIntoCache(int index)
{
	FrameItem* frameItem = LoadFrameFromStream(index);

	_mutex_mapExclusiveAccess.lock();
		m_cache[index] = frameItem;
	_mutex_mapExclusiveAccess.unlock();
}


FrameItem* VideoCacheThread::LoadFrameFromStream(int indexFrame)
{
	std::cout << "cache: Loading " << indexFrame << "\n";

	char errmsg[1024];
	FFMS_ErrorInfo err;
	err.Buffer     = errmsg;
	err.BufferSize = sizeof(errmsg);

	const FFMS_Frame* frame = FFMS_GetFrame(videoSource, indexFrame, &err);

	FrameItem* frameItem;
	if (!liFreeFrames.empty())
	{
		// reuse frame from the pool of free frames
		frameItem = liFreeFrames.front();
		liFreeFrames.pop_front();

		assert(frameItem->width*4*frameItem->height == frame->Linesize[0]*frame->ScaledHeight);
	}
	else
	{
		frameItem = new FrameItem();

		frameItem->width  = frame->ScaledWidth;
		frameItem->height = frame->ScaledHeight;
		frameItem->data = new uint8_t[frame->Linesize[0]*frame->ScaledHeight];
	}

	memcpy(frameItem->data, frame->Data[0], frame->Linesize[0]*frame->ScaledHeight);

	return frameItem;
}


FrameItem* VideoCacheThread::GetFrame(int index)
{
	std::lock_guard<std::mutex> lock(_mutex_mapExclusiveAccess);

		SetPlayhead(index);

		auto it = m_cache.find(index);

		if (it == m_cache.end()) return NULL;

		return it->second;
}

