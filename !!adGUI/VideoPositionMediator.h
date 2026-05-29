#ifndef POSITONMEDIATOR_H
#define POSITONMEDIATOR_H


class PositionMediator
{
	int iDuration10msUnit;

	double fPos0_1;
	double fPosSelectionStart;
	double fPosSelectionEnd;

	std::vector<std::function<void(void*, double)>> listenersPos;
	std::vector<std::function<void(void*, double, int)>> listenersPosInit;

	PositionMediator()
	{
		iDuration10msUnit   = 0;

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

	void SetPos0_1(void* origin, double _fPos)
	{
		if ( fabs(_fPos - fPos0_1) < 1e-6)
			return;

		fPos0_1 = _fPos;

		for (auto& listener : listenersPos)
			listener(origin, _fPos);
	}

	void SetSelection(void* origin, double _fSelectionStart, double _fSelectionEnd)
	{
		if ( fabs(_fSelectionStart - fPosSelectionStart) < 1e-6 &&
			 fabs(_fSelectionEnd   - fPosSelectionEnd) < 1e-6 )
			return;

		//for (auto& listener : listenersPos)
		//	listener(origin, _fPos0_1);
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

	void subscribeForPos(std::function<void(void*, double)> cb)
	{
		listenersPos.push_back(std::move(cb));
	}

	void subscribeForPosInit(std::function<void(void*, double, int)> cb)
	{
		listenersPosInit.push_back(std::move(cb));
	}

};


#endif
