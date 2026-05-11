#ifndef VIDEOSLIDER_H
#define VIDEOSLIDER_H

#include "../!!adGlobals/vector_math.h"
#include "gui_elementResizable.h"
#include <functional>
#include "VideoPositionMediator.h"


class VideoSlider : public GUI_ElementResizable
{
public:
	bool bEnabled;

	VideoSlider(int px, int py, int _height, PositionMediator* mediator);

	std::function<void(float)>      OnChange;

	void SetPos(float _val);
	void SetPosInit(float _val, float _v_min, float _v_max);

	float GetValue();

	void Resize(int iWidth, int iHeight) override;

	void Draw() override;

	bool Clicked(int button, int state, int x, int y) override;
	bool Drag(int x, int y) override;
	bool Hover(int x, int y) override;
	bool Wheel(int state,int delta,int x,int y) override;

protected:

	int m_iWidth;
	int m_iHeight;

	// those are values in seconds
	int m_iValMin;
	int m_iValMax;

	// that is the value in internal World coordinates like -300...300
	float m_fSliderX;

	Vec3 vColor_focused;
	Vec3 vColor_defocused;

	bool bFocused;
	bool bMouseButtonPressed;

	void DrawTicks(float count, int iStep, float fThickness, float fHeight, const Matr4& matTransform);
};


#endif