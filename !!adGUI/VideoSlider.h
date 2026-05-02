#ifndef VIDEOSLIDER_H
#define VIDEOSLIDER_H

#include "../!!adGlobals/vector_math.h"
#include "gui_element.h"
#include <string>
#include <functional>


class VideoSlider : public GUI_Element
{
public:
	bool bEnabled;

	std::function<bool()>      OnClick;
	std::function<bool(float)> OnDrawValue;
	std::function<bool()>      OnClickDrag;

	Matr4 matrSliderNonInverted;

	void SetValue(float _val, float _v_min, float _v_max);

	VideoSlider(std::string strCaption, int px, int py, float _v_min, float _v_max, float& _v_cur, int _height);

	void Resize(int iWidth, int iHeight);

	void Draw() override;

	bool Clicked(int button, int state, int x, int y) override;
	bool Drag(int x, int y) override;
	bool Hover(int x, int y) override;
	bool Wheel(int state,int delta,int x,int y) override;

protected:

	std::string _text;

	int m_iWidth;
	int m_iHeight;

	float& ptr_fVal_cur;
	float m_fVal_min;
	float m_fVal_max;

	float m_fSliderXcoord;

	Vec3 vColor_focused;
	Vec3 vColor_defocused;

	bool bFocused;
	bool bMouseButtonPressed;

	float fPointerPokeHeight;

	void DrawTicks(float count, int iStep, float fThickness, float fHeight, const Matr4& matTransform);
};


#endif