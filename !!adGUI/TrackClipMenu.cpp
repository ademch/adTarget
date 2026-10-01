#include "stdafx.h"
#include "TrackClipMenu.h"


static TrackMenuBase* Get()
{
	static TrackMenuBase instance;
	return &instance;
}

void TrackMenuBase::Init(HWND parent)
{
	m_parent = parent;

	static bool classRegistered = false;

	m_iWidth        = 200;
	m_iItemHeight	= 24;
	m_iHeight       = m_iItemHeight * m_liItems.size();

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

	m_hwnd = CreateWindowEx(	WS_EX_TOOLWINDOW | WS_EX_TOPMOST,
								TEXT("CustomMenuWindow"),
								TEXT(""),
								WS_POPUP,
								0, 0, m_iWidth, m_iHeight,
								parent, NULL, GetModuleHandle(NULL), this);
}

void TrackMenuBase::Show(int x, int y)
{
	m_hoverItem = -1;

	SetWindowPos(m_hwnd, HWND_TOPMOST, x, y, m_iWidth, m_iHeight, SWP_SHOWWINDOW);

	SetCapture(m_hwnd);

	InvalidateRect(m_hwnd, nullptr, TRUE);
}

void TrackMenuBase::Hide()
{
	if (!m_hwnd) return;

	ReleaseCapture();

	ShowWindow(m_hwnd, SW_HIDE);

	if (m_parent)
		SetFocus(m_parent);
}

bool TrackMenuBase::IsVisible() const
{
	return IsWindowVisible(m_hwnd) != FALSE;
}

RECT TrackMenuBase::GetItemRect(int index)
{
	RECT rc;

	rc.left   = 0;
	rc.right  = m_iWidth;
	rc.top    = index * m_iItemHeight;
	rc.bottom = rc.top + m_iItemHeight;

	return rc;
}

int TrackMenuBase::HitTest(int x, int y)
{
	if (x < 0 || x >= m_iWidth)
		return -1;

	if (y < 0 || y >= m_iHeight)
		return -1;

	return y / m_iItemHeight;
}

void TrackMenuBase::NotifySelection(int cmd)
{
	Hide();

	if (OnClick) OnClick(cmd);
}


void TrackMenuBase::DrawItem(HDC hdc, int index, LPCTSTR text)
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

LRESULT TrackMenuBase::WndProc(UINT msg, WPARAM wParam, LPARAM lParam)
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

			for (const auto& item : m_liItems)
			{
				DrawItem(hdc, item.first, item.second);
			}

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

LRESULT CALLBACK TrackMenuBase::StaticWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	TrackMenuBase* self = nullptr;

	if (msg == WM_NCCREATE)
	{
		CREATESTRUCT* cs = (CREATESTRUCT*)lParam;

		self = (TrackMenuBase*)cs->lpCreateParams;

		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)self);

		self->m_hwnd = hwnd;
	}
	else
	{
		self = (TrackMenuBase*)GetWindowLongPtr(hwnd, GWLP_USERDATA);
	}

	if (self)
		return self->WndProc(msg, wParam, lParam);

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

