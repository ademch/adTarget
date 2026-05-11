#ifndef TRACKPANEL_H
#define TRACKPANEL_H

#include "../!!adGlobals/vector_math.h"
#include "gui_elementResizable.h"
#include <functional>


class TimelineTrack : public GUI_ElementResizable
{
public:

	TimelineTrack(int _id, int px, int py, int _width, int _height);

	bool bEnabled;

	std::function<bool()>      OnClick;
	std::function<bool(float)> OnDrawValue;
	std::function<bool()>      OnClickDrag;

	void Resize(int iWidth, int iHeight) override;

	void Draw() override;

	bool Clicked(int button, int state, int x, int y) override;
	bool Drag(int x, int y) override;
	bool Hover(int x, int y) override;

	static int iSelected;

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