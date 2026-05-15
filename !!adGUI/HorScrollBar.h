#ifndef HORSCROLLBAR_H
#define HORSCROLLBAR_H

#include "gui_element.h"
#include "../!!adGlobals/vector_math.h"
#include <functional>


class HorScrollBar : public GUI_Element
{
public:
	int  m_Height;
	int  m_Width;

	Vec3 m_ptHandleStartWorldCoords;
	Vec3 m_ptHandleEndWorldCoords;

	bool bFocused;

	Vec4 vColor_focused;
	Vec4 vColor_defocused;

	float fZoomFactor;
	int8_t iZoomIndex;

	HorScrollBar(std::string caption, int px, int py);
	~HorScrollBar() {}

	std::function<void(Matr4)> OnChange;

	virtual void Draw();
	virtual bool Hover(int x, int y);
	virtual bool Clicked(int button, int state, int x, int y);
	virtual bool Wheel(int state, int delta, int x, int y);
	virtual bool Drag(int x, int y);

	void Resize(int iWidth, int iHeight);

protected:

	int iGUIpushed;

	Vec3 vUserSceneTranslation;
	bool bMouseSceneDragInProgress;
	int  iBeginDragX, iBeginDragY;

	Matr4 matrUserScale;

};

#endif