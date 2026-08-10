#ifndef VIDEOSLIDER_H
#define VIDEOSLIDER_H

#include "../!!adGlobals/vector_math.h"
#include "gui_elementResizable.h"
#include <functional>


class VideoSlider : public GUI_ElementResizable
{
public:

	VideoSlider(int px, int py, int _height);

	std::function<void(double)>      OnChange;
	std::function<void(double)>      OnChangeSpaceScroller;

	void SetPos0_1(double _val);
	void SetPosInit(double _val, int _v_max);

	void Resize(int iWidth, int iHeight) override;

	void Draw() override;

	bool Clicked(int button, int state, int x, int y) override;
	bool Drag(int x, int y) override;
	bool Hover(int x, int y) override;

	void MoveByScreenPixels(int dx);

protected:

	int m_iWidth;
	int m_iHeight;

	int m_iValMax;

	double m_fPos01;

	Vec3 vColor_focused;
	Vec3 vColor_defocused;

	bool bFocused;
	bool bMouseButtonPressed;

	bool DrawTicks(float count, float fThickness, float fHeight, const Matr4& matTransform);
	bool DrawMinuteDigits(float count, const Matr4& matTransform);

	bool TryToSnapPositionToKeyframe(double& fPos0_1);

	bool bSpaceSliderSnapped;
	int  iSpaceSliderAccum;

private:

	HCURSOR hCurLeftRight;
};


#endif