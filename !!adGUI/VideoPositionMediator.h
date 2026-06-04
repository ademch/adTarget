#ifndef POSITONMEDIATOR_H
#define POSITONMEDIATOR_H


class PositionMediator
{
	int iDuration10msUnit;

	double fPos0_1;
	double fPosSelStart0_1;
	double fPosSelEnd0_1;
	double fPosMarker0_1;


	std::vector<std::function<void(void*, double)>>		 listenersPos;
	std::vector<std::function<void(void*, double, int)>> listenersPosInit;
	std::vector<std::function<void(void*, double)>>		 listenersPosUpdateFromTimer;
	std::vector<std::function<void(void*, double)>>		 listenersMarkerPos;

	PositionMediator()
	{
		iDuration10msUnit   = 0;

		fPos0_1 = 0;
		fPosSelStart0_1 = 0;
		fPosSelEnd0_1   = 0;
		fPosMarker0_1   = -1;
	}

public:

	static PositionMediator* Get()
	{
		static PositionMediator mediator;
		return &mediator;
	}

	void SetPos0_1(void* origin, double _fPos, bool bUpdateFromTimer = false)
	{
		if ( fabs(_fPos - fPos0_1) < 1e-6)
			return;

		fPos0_1 = _fPos;

		for (auto& listener : listenersPos)
			listener(origin, _fPos);

		if (bUpdateFromTimer)
		{
			for (auto& listener : listenersPosUpdateFromTimer)
				listener(origin, _fPos);
		}
	}

	void SetSelection(void* origin, double _fSelectionStart, double _fSelectionEnd)
	{
		if ( fabs(_fSelectionStart - fPosSelStart0_1) < 1e-6 &&
			 fabs(_fSelectionEnd   - fPosSelEnd0_1) < 1e-6 )
			return;

		//for (auto& listener : listenersPos)
		//	listener(origin, _fPos0_1);
	}

	void SetMarker(void* origin, double _fPosMarker0_1)
	{
		if ( fabs(fPosMarker0_1 - _fPosMarker0_1) < 1e-6 )
			return;

		fPosMarker0_1 = _fPosMarker0_1;

		for (auto& listener : listenersMarkerPos)
			listener(origin, _fPosMarker0_1);
	}

	void Init(void* origin, double _fPos0_1, unsigned int _iDurationIn10msUnit)
	{
		fPos0_1 = _fPos0_1;
		iDuration10msUnit = _iDurationIn10msUnit;

		for (auto& listener : listenersPosInit)
			listener(origin, _fPos0_1, _iDurationIn10msUnit);
	}

	int Duration10msUnits()
	{
		return iDuration10msUnit;
	}

	int Pos10msUnits()
	{
		return int(round(fPos0_1*iDuration10msUnit));
	}

	double Pos0_1()
	{
		return fPos0_1;
	}

	double PosMarker0_1()
	{
		return fPosMarker0_1;
	}

	void subscribeForPos(std::function<void(void*, double)> cb)
	{
		listenersPos.push_back(std::move(cb));
	}

	void subscribeForMarker(std::function<void(void*, double)> cb)
	{
		listenersMarkerPos.push_back(std::move(cb));
	}

	void subscribeForPosInit(std::function<void(void*, double, int)> cb)
	{
		listenersPosInit.push_back(std::move(cb));
	}

	void subscribeForPosUpdateFromTimer(std::function<void(void*, double)> cb)
	{
		listenersPosUpdateFromTimer.push_back(std::move(cb));
	}

};


#endif
