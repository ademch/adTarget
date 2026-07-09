#ifndef POSITONMEDIATOR_H
#define POSITONMEDIATOR_H

#include <algorithm>

class PositionMediator
{
	int iDuration10msUnit;

	double fPos0_1;
	double fPosSelStart0_1;
	double fPosSelEnd0_1;
	double fPosMarker0_1;

	std::vector< std::pair<void*, std::function<void(void*, double)>> >		 listenersPos;
	std::vector< std::pair<void*, std::function<void(void*, double, int)>> > listenersPosInit;
	std::vector< std::pair<void*, std::function<void(void*, double)>> >		 listenersPosUpdateFromTimer;
	std::vector< std::pair<void*, std::function<void(void*, double)>> >		 listenersMarkerPos;

	PositionMediator()
	{
		iDuration10msUnit   = 0;

		fPos0_1			= 0;
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
			listener.second(origin, _fPos);

		if (bUpdateFromTimer)
		{
			for (auto& listener : listenersPosUpdateFromTimer)
				listener.second(origin, _fPos);
		}
	}

	void SetSelection0_1(void* origin, double _fPosSelStart0_1, double _fPosSelEnd0_1)
	{
		if ( fabs(_fPosSelStart0_1 - fPosSelStart0_1) < 1e-6 &&
			 fabs(_fPosSelEnd0_1   - fPosSelEnd0_1)   < 1e-6 )
			return;

		fPosSelStart0_1 = _fPosSelStart0_1;
		fPosSelEnd0_1   = _fPosSelEnd0_1;

		//for (auto& listener : listenersPos)
		//	listener.second(origin, _fPos0_1);
	}

	void SetMarker(void* origin, double _fPosMarker0_1)
	{
		if ( fabs(fPosMarker0_1 - _fPosMarker0_1) < 1e-6 )
			return;

		fPosMarker0_1 = _fPosMarker0_1;

		for (auto& listener : listenersMarkerPos)
			listener.second(origin, _fPosMarker0_1);
	}

	void Init(void* origin, double _fPos0_1, unsigned int _iDurationIn10msUnit)
	{
		fPos0_1 = _fPos0_1;
		iDuration10msUnit = _iDurationIn10msUnit;

		for (auto& listener : listenersPosInit)
			listener.second(origin, _fPos0_1, _iDurationIn10msUnit);
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

	void PosSel10msUnits(int& iStart10msUnits, int& iEnd10msUnits)
	{
		iStart10msUnits = int(round(fPosSelStart0_1*iDuration10msUnit));
		iEnd10msUnits   = int(round(fPosSelEnd0_1  *iDuration10msUnit));
	}

	double PosSelStart0_1()
	{
		return fPosSelStart0_1;
	}

	// ----------------SUBSCRIPTION FUNCTIONS----------------------------------------

	void subscribeForPos(void* owner, std::function<void(void*, double)> cb)
	{
		listenersPos.emplace_back(owner, std::move(cb));
	}

	void subscribeForMarker(void* owner, std::function<void(void*, double)> cb)
	{
		listenersMarkerPos.emplace_back(owner, std::move(cb));
	}

	void subscribeForPosInit(void* owner, std::function<void(void*, double, int)> cb)
	{
		listenersPosInit.emplace_back(owner, std::move(cb));
	}

	void subscribeForPosUpdateFromTimer(void* owner, std::function<void(void*, double)> cb)
	{
		listenersPosUpdateFromTimer.emplace_back(owner, std::move(cb));
	}

	// --------------UNSUBSCRIPTION FUNCTIONS--------------------------------------------

	void unsubscribeForPos(void* owner)
	{
		listenersPos.erase( std::remove_if(	listenersPos.begin(),
											listenersPos.end(),
											[owner](const auto& l)
											{
												return l.first == owner;
											}),
											listenersPos.end() );
	}

};


#endif
