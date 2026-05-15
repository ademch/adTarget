#ifndef POSITONMEDIATOR_H
#define POSITONMEDIATOR_H

class PositionMediator
{
	unsigned int iDuration;

	float fPos0_1;
	float fPosSelectionStart;
	float fPosSelectionEnd;

	std::vector<std::function<void(void*, float)>> listenersPos;
	std::vector<std::function<void(void*, float, unsigned int)>> listenersPosInit;

	PositionMediator()
	{
		iDuration   = 0;

		fPos0_1 = 0;
		fPosSelectionStart = 0;
		fPosSelectionEnd   = 0;
	}

public:

	static PositionMediator* Get()
	{
		static PositionMediator mediator;
		return &mediator;
	}

	void SetPos(void* origin, float _fPos)
	{
		if ( fabs(_fPos - fPos0_1) < 1e-6)
			return;

		fPos0_1 = _fPos;

		for (auto& listener : listenersPos)
			listener(origin, _fPos);
	}

	void SetSelection(void* origin, float _fSelectionStart, float _fSelectionEnd)
	{
		if ( fabs(_fSelectionStart - fPosSelectionStart) < 1e-6 &&
			 fabs(_fSelectionEnd   - fPosSelectionEnd) < 1e-6 )
			return;

		//for (auto& listener : listenersPos)
		//	listener(origin, _fPos);
	}

	void Init(void* origin, float _fPos, unsigned int _iDuration)
	{
		fPos0_1   = _fPos;
		iDuration = _iDuration;

		for (auto& listener : listenersPosInit)
			listener(origin, _fPos, _iDuration);
	}

	float Pos01() { return fPos0_1; }

	int Duration()  { return iDuration; }

	void subscribeForPos(std::function<void(void*, float)> cb)
	{
		listenersPos.push_back(std::move(cb));
	}

	void subscribeForPosInit(std::function<void(void*, float, unsigned int)> cb)
	{
		listenersPosInit.push_back(std::move(cb));
	}

};


#endif
