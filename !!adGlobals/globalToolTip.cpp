
#include "stdafx.h"
#include "globalToolTip.h"


char strTooTip[255];

LRESULT CALLBACK TipProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);


ToolTip::ToolTip()
{
	g_hwndToolTip = NULL;

	memset(strTooTip, 0, sizeof(strTooTip));

	iToolTipX = 0;
	iToolTipY = 0;

	bShowTimerIsRunning = false;

	bHintHasBeenShown  = false;

	WNDCLASSW wc = {0};
	wc.lpfnWndProc   = TipProc;
	wc.hInstance     = GetModuleHandle(NULL);;
	wc.lpszClassName = L"MyToolTip";

	RegisterClassW(&wc);

	g_hwndToolTip = CreateWindowExW(WS_EX_TOPMOST | WS_EX_TOOLWINDOW,
									L"MyToolTip",
									L"",
									WS_POPUP,
									0, 0, 0, 0,
									NULL, NULL, GetModuleHandle(NULL), NULL);

	QueryPerformanceFrequency(&ticksPerSecond);
	QueryPerformanceCounter(&T0);
}


void ToolTip::Schedule(const char* text)
{
	if (strlen(text) == 0) return;

	bool bSameTextHint = !strcmp(strTooTip, text);

	// it can be a component tries to schedule its hint, but functions that clear bHintHasBeenShown never received control
	if (!bSameTextHint) bHintHasBeenShown = false;

	POINT pt;
	GetCursorPos(&pt);

	// windows continues to strangely send mouse move even when mouse is disconnected
	if ((iToolTipX == pt.x) && (iToolTipY == pt.y) && bHintHasBeenShown) return;

	_HideWindow();

	// do not reshow hint with the same text if it has been shown
	if (bSameTextHint && bHintHasBeenShown) return;
	
	strcpy(strTooTip, text);

	iToolTipX = pt.x;
	iToolTipY = pt.y;

	// reset timer every time event happens
	_RestartShowTimer();
}


void ToolTip::_RestartShowTimer()
{
	QueryPerformanceCounter(&T0);
	bShowTimerIsRunning = true;
}


SIZE GetTextSize(HDC hdc, const char* text)
{
	RECT rc = {0, 0, 0, 0};

	DrawTextA(hdc, text, -1, &rc, DT_CALCRECT | DT_SINGLELINE);

	SIZE s;
	s.cx = rc.right  - rc.left;
	s.cy = rc.bottom - rc.top;

	return s;
}


void ToolTip::_ShowWindow()
{
	if (!g_hwndToolTip) return;

	HDC hdc = GetDC(NULL);

		SIZE sz = GetTextSize(hdc, strTooTip);

	ReleaseDC(NULL, hdc);

	int w = sz.cx + 10;	// + padding
	int h = sz.cy + 5;	// + padding

	SetWindowPos(g_hwndToolTip, HWND_TOPMOST, iToolTipX + 20, iToolTipY + 20, w, h, SWP_SHOWWINDOW | SWP_NOACTIVATE);

	bHintHasBeenShown = true;

	//InvalidateRect(g_hwndToolTip, NULL, TRUE);
}

void ToolTip::_HideWindow()
{
	bShowTimerIsRunning = false;

	if (!g_hwndToolTip) return;
	if (!IsWindowVisible(g_hwndToolTip)) return;

	ShowWindow(g_hwndToolTip, SW_HIDE);
}

void ToolTip::Finish()
{
	bShowTimerIsRunning = false;

	strTooTip[0] = '\0';

	bHintHasBeenShown = false;

	if (!g_hwndToolTip) return;
	if (!IsWindowVisible(g_hwndToolTip)) return;

	ShowWindow(g_hwndToolTip, SW_HIDE);
}


void ToolTip::UpdateTimer()
{
	LARGE_INTEGER T1;
	QueryPerformanceCounter(&T1);
	float elapsed_sec = float(T1.QuadPart - T0.QuadPart) / float(ticksPerSecond.QuadPart);

	if (bShowTimerIsRunning)
	{
		//printf("%f\n", elapsed_sec);
		if (elapsed_sec > 0.5)
		{
			_ShowWindow();
			bShowTimerIsRunning = false;
		}
	}


}


///////////////////////////////////////////////////////////////////////////////////


LRESULT CALLBACK TipProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		RECT rc;
		GetClientRect(hWnd, &rc);

		// Background color
		HBRUSH bg = CreateSolidBrush(RGB(18, 28, 22));
		FillRect(hdc, &rc, bg);
		DeleteObject(bg);

		// Border
		HBRUSH br = CreateSolidBrush(RGB(70, 120, 90));
		FrameRect(hdc, &rc, br);
		DeleteObject(br);

		// Text
		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, RGB(200, 220, 200));

		DrawTextA(hdc, strTooTip, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

		EndPaint(hWnd, &ps);
		return 0;
	}

	case WM_ERASEBKGND:
		return 1; // prevent flicker
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

