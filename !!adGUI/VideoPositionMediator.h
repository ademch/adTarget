#ifndef POSITONMEDIATOR_H
#define POSITONMEDIATOR_H

class PositionMediator
{
	unsigned int iDurationFrames;

	float fPos0_1;
	float fPosSelectionStart;
	float fPosSelectionEnd;

	std::vector<std::function<void(void*, float)>> listenersPos;
	std::vector<std::function<void(void*, float, unsigned int)>> listenersPosInit;

	PositionMediator()
	{
		iDurationFrames   = 0;

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

	void SetPos0_1(void* origin, float _fPos)
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
		//	listener(origin, _fPos0_1);
	}

	void Init(void* origin, float _fPos0_1, unsigned int _iDurationFrames)
	{
		fPos0_1 = _fPos0_1;
		iDurationFrames = _iDurationFrames;

		for (auto& listener : listenersPosInit)
			listener(origin, _fPos0_1, _iDurationFrames);
	}

	float Pos01()
	{
		return fPos0_1;
	}

	int DurationFrames()
	{
		return iDurationFrames;
	}

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
