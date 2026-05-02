
#ifndef GLOBALTOOLTIP_H
#define GLOBALTOOLTIP_H


class ToolTip
{
public:

	ToolTip();
	~ToolTip() {}

	static ToolTip* Get()
	{
		static ToolTip instance;
		return &instance;
	}

	void Schedule(const char* text);
	void UpdateTimer();
	void Finish();

private:
	HWND g_hwndToolTip;

	bool bHintHasBeenShown;

	int iToolTipX;
	int iToolTipY;

	LARGE_INTEGER ticksPerSecond;
	LARGE_INTEGER T0;

	bool bShowTimerIsRunning;

	void _RestartShowTimer();
	void _ShowWindow();
	void _HideWindow();

};

#endif
