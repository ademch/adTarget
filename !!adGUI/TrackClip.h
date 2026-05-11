#ifndef TRACKCLIP_H
#define TRACKCLIP_H

#include "../!!adGlobals/vector_math.h"
#include "gui_elementResizable.h"
#include <functional>

enum ClipType  {
	CLIP_IMAGE,
	CLIP_VIDEO
};

class TrackClip : public GUI_ElementResizable
{
public:
	bool bEnabled;

	std::function<bool()>      OnClick;
	std::function<bool(float)> OnDrawValue;
	std::function<bool()>      OnClickDrag;

	float m_fHandleStartTX;
	float m_fHandleEndTX;

	TrackClip(int _id, int px, int py, int _width, int _height);

	void Resize(int iWidth, int iHeight) override;

	void SetPos(float _fStart, float _fEnd)
	{
		m_fHandleStartTX = _fStart;
		m_fHandleEndTX   = _fEnd;
	}

	void Draw() override;

	bool Clicked(int button, int state, int x, int y) override;
	bool Drag(int x, int y) override;
	bool Hover(int x, int y) override;

	static int iSelected;

	int iTrack;
	ClipType type;

protected:

	// 1 based unique id
	int id;

	int m_iWidth;
	int m_iHeight;

	Vec3 vColor_focused;
	Vec3 vColor_defocused;

	bool bFocused;
	bool bMouseButtonPressed;
};


#endif