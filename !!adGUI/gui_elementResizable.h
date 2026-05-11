#ifndef GUI_ELEMENT_RESIZABLE_H
#define GUI_ELEMENT_RESIZABLE_H

#include "gui_element.h"
#include "../!!adGlobals/vector_math.h"

class GUI_ElementResizable : public GUI_Element
{
public:
	GUI_ElementResizable()
	{
		matrSliderNonInverted = Mat4MakeIdent();
	}

	virtual void Resize(int iWidth, int iHeight) {}

	// non inverted Matrix is used to scale down the range and navigate with greater precision
	// eg -300...300 can be scaled to -200...200 with greater resolution
	Matr4 matrSliderNonInverted;
};

#endif