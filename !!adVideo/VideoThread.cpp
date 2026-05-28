#include "stdafx.h"
#include "VideoThread.h"
#include <assert.h>


VideoCacheThread::VideoCacheThread(FFMS_VideoSource* _videoSource, int maxItems, int hysteresis)
{   
	videoSource    = _videoSource;

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
	for (auto it = liFreeFrames.begin(); it != liFreeFrames.end(); )
	{
		delete [] (*it)->data;
		delete *it;
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
			UpdateCacheWindow(idxPlayhead);

			atomic_bNewRequest = false;
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
}

void VideoCacheThread::UpdateCacheWindow(int index)
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
	for (int i = m_center; i <= iHigh; ++i)
	{
		if (!ExistsInCache(i))
			InsertIntoCache(i);
	}

	for (int i = iLow; i < m_center; ++i)
	{
		if (!ExistsInCache(i))
			InsertIntoCache(i);
	}

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
	std::cout << "Loading " << indexFrame << "\n";

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

	////////////////

	FFMS_Track* videoTrack = FFMS_GetTrackFromVideo(videoSource);
	const FFMS_FrameInfo* info = FFMS_GetFrameInfo(videoTrack, indexFrame);

	int64_t pts = info->PTS;

	const FFMS_TrackTimeBase* tb = FFMS_GetTimeBase(videoTrack);

	frameItem->fS = (double)pts * (double)tb->Num/(double)tb->Den * 0.001;

	////////////////

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


FrameItem* VideoCacheThread::GetFrameByTime(double _fS, int& indexLastRequested)
{
	std::lock_guard<std::mutex> lock(_mutex_mapExclusiveAccess);

		SetPlayhead(indexLastRequested);
	
		// find iterator of last accessed frame and use it as a seed to search forward 
		auto it = m_cache.find(indexLastRequested);

		if (it == m_cache.end()) return NULL;

		while (true)
		{
			// does next frame exist in cache and does it go earlier than asked
			if ( (std::next(it) != m_cache.end()) && (std::next(it)->second->fS < _fS + 0.001) )
				++it;
			else
				break;
		}

		indexLastRequested = it->first;

		return it->second;
}




