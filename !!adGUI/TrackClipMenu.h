#ifndef GLOBALMENU_H
#define GLOBALMENU_H

#include <windowsx.h>
#include <functional>



class TrackClipMenu
{
public:

	enum
	{
		ITEM_SPLIT  = 1,
		ITEM_DELETE = 2
	};

	std::function<void(int)> OnClick;

	static TrackClipMenu* Get()
	{
		static TrackClipMenu instance;
		return &instance;
	}

	void Create(HWND parent)
	{
		m_parent = parent;

		static bool classRegistered = false;

		iWidth      = 150;
		iItemHeight = 24;
		iHeight     = iItemHeight * 2;

		if (!classRegistered)
		{
			WNDCLASS wc = {};

			wc.lpfnWndProc   = StaticWndProc;
			wc.hInstance     = GetModuleHandle(NULL);
			wc.lpszClassName = TEXT("CustomMenuWindow");
			wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
			wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

			RegisterClass(&wc);

			classRegistered = true;
		}

		m_hwnd = CreateWindowEx(WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
								TEXT("CustomMenuWindow"),
								TEXT(""),
								WS_POPUP,
								0, 0, iWidth, iHeight,
								parent, NULL, GetModuleHandle(NULL), this);
	}

	void Show(int x, int y)
	{
		m_hoverItem = -1;

		SetWindowPos(	m_hwnd, HWND_TOPMOST, x, y, iWidth, iHeight, SWP_SHOWWINDOW);

		SetCapture(m_hwnd);

		InvalidateRect(m_hwnd, nullptr, TRUE);
	}

	void Hide()
	{
		if (!m_hwnd) return;

		ReleaseCapture();

		ShowWindow(m_hwnd, SW_HIDE);

		if (m_parent)
			SetFocus(m_parent);
	}

	bool IsVisible() const
	{
		return IsWindowVisible(m_hwnd) != FALSE;
	}

private:

	int iWidth;
	int iItemHeight;
	int iHeight;

	HWND m_hwnd   = nullptr;
	HWND m_parent = nullptr;

	int m_hoverItem = -1;

	RECT GetItemRect(int index)
	{
		RECT rc;

		rc.left   = 0;
		rc.right  = iWidth;
		rc.top    = index * iItemHeight;
		rc.bottom = rc.top + iItemHeight;

		return rc;
	}

	int HitTest(int x, int y)
	{
		if (x < 0 || x >= iWidth)
			return -1;

		if (y < 0 || y >= iHeight)
			return -1;

		return y / iItemHeight;
	}

	void NotifySelection(int item)
	{
		int cmd = 0;

		switch (item)
		{
		case 0: cmd = ITEM_SPLIT;  break;
		case 1: cmd = ITEM_DELETE; break;
		}

		Hide();

		if (cmd && OnClick)
			OnClick(cmd);
	}


	void DrawItem(HDC hdc, int index, LPCTSTR text)
	{
		RECT rc = GetItemRect(index);

		if (index == m_hoverItem)
		{
			HBRUSH br = CreateSolidBrush(RGB(70, 120, 90));
			FillRect(hdc, &rc, br);
			DeleteObject(br);
		}

		// Text
		SetBkMode(hdc, TRANSPARENT);
		SetTextColor(hdc, RGB(200, 220, 200));

		rc.left += 20;

		DrawText(hdc, text, -1, &rc, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	}

	LRESULT WndProc(UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(m_hwnd, &ps);

			RECT rc;
			GetClientRect(m_hwnd, &rc);

			// Background color
			HBRUSH bg = CreateSolidBrush(RGB(18, 28, 22));
			FillRect(hdc, &rc, bg);
			DeleteObject(bg);

			DrawItem(hdc, 0, TEXT("Split"));
			DrawItem(hdc, 1, TEXT("Delete"));

			// Border
			HBRUSH br = CreateSolidBrush(RGB(70, 120, 90));
			FrameRect(hdc, &rc, br);
			DeleteObject(br);

			EndPaint(m_hwnd, &ps);

			return 0;
		}
		case WM_MOUSEMOVE:
		{
			int item = HitTest(	GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));

			if ((item >= 0) &&
				(item != m_hoverItem))
			{
				m_hoverItem = item;
				InvalidateRect(	m_hwnd,	nullptr, TRUE);
			}

			return 0;
		}
		case WM_ERASEBKGND:
			return 1; // prevent flicker

		case WM_MBUTTONDOWN:
		case WM_RBUTTONDOWN:
		case WM_LBUTTONDOWN:
		{
			int item = HitTest(GET_X_LPARAM(lParam),GET_Y_LPARAM(lParam));

			if (item >= 0)
				NotifySelection(item);
			else
				Hide();

			return 0;
		}

		case WM_CAPTURECHANGED:
			Hide();
			return 0;

		case WM_KILLFOCUS:
			Hide();
			return 0;
		}

		return DefWindowProc(m_hwnd, msg, wParam, lParam);
	}

	static LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT msg, WPARAM wParam,	LPARAM lParam)
	{
		TrackClipMenu* self = nullptr;

		if (msg == WM_NCCREATE)
		{
			CREATESTRUCT* cs = (CREATESTRUCT*)lParam;

			self = (TrackClipMenu*)cs->lpCreateParams;

			SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)self);

			self->m_hwnd = hwnd;
		}
		else
		{
			self = (TrackClipMenu*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
		}

		if (self)
			return self->WndProc(msg, wParam, lParam);

		return DefWindowProc(hwnd, msg, wParam, lParam);
	}
};

#endif
