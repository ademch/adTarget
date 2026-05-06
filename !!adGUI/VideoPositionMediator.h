#ifndef POSITONMEDIATOR_H
#define POSITONMEDIATOR_H

class PositionMediator
{
	unsigned int iStartSec;
	unsigned int iEndSec;

	float fPos;

	std::vector<std::function<void(void*, float)>> listenersPos;
	std::vector<std::function<void(void*, float, unsigned int, unsigned int)>> listenersPosInit;

public:

	PositionMediator()
	{
		iStartSec = 0;
		iStartSec = 0;
		fPos = 0;
	}

	void SetPos(void* origin, float _fPos)
	{
		if ( fabs(_fPos - fPos) < 1e-6)
			return;

		for (auto& listener : listenersPos)
			listener(origin, _fPos);
	}

	void InitPos(void* origin, float _fPos, unsigned int _iStartSec, unsigned int _iEndSec)
	{
		fPos      = _fPos;
		iStartSec = _iStartSec;
		iEndSec   = _iEndSec;

		for (auto& listener : listenersPosInit)
			listener(origin, _fPos, _iStartSec, _iEndSec);
	}

//	int GetPos() const { return value; }

	void subscribeForPos(std::function<void(void*, float)> cb)
	{
		listenersPos.push_back(std::move(cb));
	}

	void subscribeForPosInit(std::function<void(void*, float, unsigned int, unsigned int)> cb)
	{
		listenersPosInit.push_back(std::move(cb));
	}


};

#endif
