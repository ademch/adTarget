#ifndef HORSCROLLBAR_H
#define HORSCROLLBAR_H

#include "gui_element.h"
#include "../!!adGlobals/vector_math.h"
#include <functional>


class HorScrollBar : public GUI_Element
{
public:
	int  m_iHeight;
	int  m_iWidth;

	Vec3 m_ptHandleStartWorldCoords;
	Vec3 m_ptHandleEndWorldCoords;

	bool bFocused;

	Vec4 vColor_focused;
	Vec4 vColor_defocused;

	float  fZoomFactor;
	int8_t iZoomIndex;

	HorScrollBar(std::string caption, int px, int py);
	~HorScrollBar() {}

	std::function<void(Matr4)> OnChange;

	void Draw() override;
	bool Hover(int x, int y) override;
	bool Clicked(int button, int state, int x, int y) override;
	bool Wheel(int state, int delta, int x, int y) override;
	bool Drag(int x, int y) override;

	void Resize(int iWidth, int iHeight);

	// because slider visualisation window has slightly different width we need to compensate for it
	int iBorder;
	void ScrollToMakePlayheadVisible(double fVal);

protected:

	int iGUIpushed;

	Vec3 vUserSceneTranslation;
	bool bMouseSceneDragInProgress;
	int  iBeginDragX, iBeginDragY;

	Matr4 matrUserScale;

};

#endif