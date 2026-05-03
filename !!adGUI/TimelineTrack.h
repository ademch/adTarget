#ifndef TRACKPANEL_H
#define TRACKPANEL_H

#include "../!!adGlobals/vector_math.h"
#include "gui_element.h"
#include <functional>


class TimelineTrack : public GUI_Element
{
public:
	bool bEnabled;

	std::function<bool()>      OnClick;
	std::function<bool(float)> OnDrawValue;
	std::function<bool()>      OnClickDrag;

	// non inverted Matrix is used to scale down the range and navigate with greater precision
	// eg -300 300 can be scaled to -200 200 with greater resolution
	Matr4 matrSliderNonInverted;

	TimelineTrack(int px, int py, int _width, int _height);

	void Resize(int iWidth, int iHeight);

	void Draw() override;

	bool Clicked(int button, int state, int x, int y) override;
	bool Drag(int x, int y) override;
	bool Hover(int x, int y) override;
	bool Wheel(int state,int delta,int x,int y) override;

protected:

	int m_iWidth;
	int m_iHeight;

	float m_fSliderX;

	Vec3 vColor_focused;
	Vec3 vColor_defocused;

	bool bFocused;
	bool bMouseButtonPressed;
};


#endif