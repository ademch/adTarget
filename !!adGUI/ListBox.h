#ifndef LIST_BOX_H
#define LIST_BOX_H

#include "glfont.h"
#include "gui_element.h"
#include "../!!adGlobals/vector_math.h"
#include <vector>
#include <functional>

class ListBox : public GUI_Element
{
public:
	float m_Height;
	int   m_Width;

	std::string _text;

	int  m_Count;

	bool bFocusedUp;
	bool bFocusedDown;
	bool bEnabled;
	bool bVisible;

	// global selection in items
	unsigned int iSelected;

	Vec4 vColor_focused;
	Vec4 vColor_defocused;

	ListBox(std::string caption, int px, int py, int width, int count, float size);
	~ListBox();

	std::function<bool()>    OnClick;
	std::function<bool(int)> OnSelect;

	virtual void Draw();
	virtual bool Hover(int x, int y);
	virtual bool Clicked(int button, int state, int x, int y);
	virtual void Wheel(int state, int delta, int x, int y);

	bool SetSelected(std::string);
	std::string GetSelected();

	std::vector<std::string> items;

protected:
	float m_TextSize;

	int iArrowDownPushed;
	int iArrowUpPushed;

	float m_TextWidth;

private:
	int iHovered;

	float fScrollBarWidth;

	HCURSOR hCurGrab;
	HCURSOR hCurGrabbing;

	int iPeepHoleShift;
};

#endif