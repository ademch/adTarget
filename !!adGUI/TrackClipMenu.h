#ifndef GLOBALMENU_H
#define GLOBALMENU_H

#include <windowsx.h>
#include <functional>
#include <vector>


class TrackMenuBase
{
public:

	std::function<void(int)> OnClick;

	virtual void Init(HWND parent);

	void Show(int x, int y);
	void Hide();

	bool IsVisible() const;

protected:
	std::vector<std::pair<int, LPCTSTR>> m_liItems;

private:

	int m_iWidth;
	int m_iHeight;
	int m_iItemHeight;

	HWND m_hwnd   = nullptr;
	HWND m_parent = nullptr;

	int m_hoverItem = -1;

	RECT GetItemRect(int index);
	int  HitTest(int x, int y);
	void NotifySelection(int cmd);

	void DrawItem(HDC hdc, int index, LPCTSTR text);

	LRESULT WndProc(UINT msg, WPARAM wParam, LPARAM lParam);

	static LRESULT CALLBACK StaticWndProc(HWND hwnd, UINT msg, WPARAM wParam,	LPARAM lParam);
};


class TrackClipMenu : public TrackMenuBase
{
public:

	enum
	{
		ITEM_DELETE = 0
	};

	void Init(HWND parent) override
	{
		m_liItems.emplace_back(0, TEXT("Delete clip"));

		TrackMenuBase::Init(parent);
	}

	static TrackClipMenu* Get()
	{
		static TrackClipMenu instance;
		return &instance;
	}
};

class TrackTRSkeyframeMenu : public TrackMenuBase
{
public:

	enum
	{
		ITEM_DELETE = 0
	};

	void Init(HWND parent) override
	{
		m_liItems.emplace_back(0, TEXT("Delete TRS key"));

		TrackMenuBase::Init(parent);
	}

	static TrackTRSkeyframeMenu* Get()
	{
		static TrackTRSkeyframeMenu instance;
		return &instance;
	}
};


class Track2DpolylineKeyframeMenu : public TrackMenuBase
{
public:

	enum
	{
		ITEM_DELETE = 0
	};

	void Init(HWND parent) override
	{
		m_liItems.emplace_back(0, TEXT("Delete morph key"));

		TrackMenuBase::Init(parent);
	}

	static Track2DpolylineKeyframeMenu* Get()
	{
		static Track2DpolylineKeyframeMenu instance;
		return &instance;
	}
};

#endif
