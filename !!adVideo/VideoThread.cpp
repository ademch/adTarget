#include "stdafx.h"
#include "VideoThread.h"


VideoCacheThread::VideoCacheThread(FFMS_VideoSource* _videoSource, int maxItems, int hysteresis)
{   
	videoSource = _videoSource;

	m_halfWindow   = maxItems/2;
	m_hysteresis   = hysteresis;
	m_center       = 0;
	m_bCacheInitialized  = false;

	idxPlayhead = 0;

	atomic_bRunning = false;
}


void VideoCacheThread::Start()
{
	atomic_bRunning = true;
	Thread = std::thread(&VideoCacheThread::Worker, this);
}

void VideoCacheThread::Stop()
{
	atomic_bRunning = false;
	cvFrameIdChanged.notify_all();

	if (Thread.joinable())
		Thread.join();
}

void VideoCacheThread::SetPlayhead(int _idxPlayhead)
{
	// lock _mutex with automatic unlock on out of context
	std::lock_guard<std::mutex> lock(_mutex);

	idxPlayhead = _idxPlayhead;
	cvFrameIdChanged.notify_all();
}

void VideoCacheThread::Worker()
{
	while (atomic_bRunning)
	{
		std::unique_lock<std::mutex> lock(_mutex);
		cvFrameIdChanged.wait_for(lock, std::chrono::milliseconds(10));

		MaintainWindow(idxPlayhead);
	}
}

void VideoCacheThread::MaintainWindow(int index)
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

	// 1. EVICT outside window
	for (auto it = m_cache.begin(); it != m_cache.end(); )
	{
		if (it->index < iLow || it->index > iHigh)
		{
			// return FrameItem to the pull of buffers
			liFreeFrames.push_back(it->FrameItem);

			m_lookup.erase(it->index);
			it = m_cache.erase(it);
		}
		else
		{
			++it;
		}
	}

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
	return m_lookup.find(i) != m_lookup.end();
}

void VideoCacheThread::InsertIntoCache(int index)
{
	// find insertion place to keep list ordered by index
	auto it = m_cache.begin();
	while (it != m_cache.end() && it->index < index)
	{
		++it;
	}

	FrameItem* frameItem = LoadFrameFromStream(index);

	auto inserted = m_cache.insert(it, { index, frameItem });
	m_lookup[index] = inserted;
}

FrameItem* VideoCacheThread::LoadFrameFromStream(int index)
{
	char errmsg[1024];
	FFMS_ErrorInfo err;
	err.Buffer     = errmsg;
	err.BufferSize = sizeof(errmsg);

	const FFMS_Frame* frame = FFMS_GetFrame(videoSource, index, &err);

	FrameItem* frameItem;
	// try to reuse frames from the pool of free frames
	if (!liFreeFrames.empty())
		frameItem = liFreeFrames.back();
	else
		frameItem = new FrameItem();

	frameItem->width  = frame->ScaledWidth;
	frameItem->height = frame->ScaledHeight;
	frameItem->data = new uint8_t[frame->Linesize[0]*frame->ScaledHeight];

	memcpy(frameItem->data, frame->Data[0], frame->Linesize[0]*frame->ScaledHeight);

	std::cout << "Loading " << index << "\n";
	return frameItem;
}


FrameItem* VideoCacheThread::GetFrame(int index)
{
	SetPlayhead(index);

	auto it = m_lookup.find(index);
	if (it == m_lookup.end())
		return nullptr;

	return it->second->FrameItem;
}



