
#ifndef TOOLBASE_H
#define TOOLBASE_H

#include "../../!!adGlobals/adOpenGLUtilities.h"
#include "../!!adGlobals/vector_math.h"
#include "gui_element.h"

#include <vector>

class ToolBase
{
public:

	ToolBase()
	{ 
		fUserScale = 1.0f;
	}

	virtual ~ToolBase()	{ }

	float fUserScale;

	void Reshape(int iBottomLeftX, int iBottomLeftY, int iWidth, int iHeight)
	{
		ReshapeGUI(iWidth, iHeight);
	}

	virtual void Render() {};

	virtual void MotionFunc(Vec3 ptMouse) {};
	virtual void PassiveMotionFunc(Vec3 ptMouse) {};
	virtual bool MouseFunc(int button, int state, Vec3 ptMouse) { return false; };
	virtual bool KeyboardFunc(unsigned char key, Vec3 ptMouse)  { return false; };
	virtual void MouseWheelFunc(int state, int delta, Vec3 ptMouse) {};

	std::vector<GUI_Element*> liGUI_Elements;

	void RenderGUI()
	{
		for (auto iterElement : liGUI_Elements)
			iterElement->Draw();
	}

	void PassiveMotionFuncGUI(Vec3 ptMouse)
	{
		for (auto iterElement : liGUI_Elements)
			iterElement->Hover(int(ptMouse.X), int(ptMouse.Y));
	}

	bool MouseFuncGUI(int button, int state, Vec3 ptMouse)
	{
		bool result = false;

		for (auto iterElement : liGUI_Elements)
		{
			if (iterElement->Clicked(button, state, int(ptMouse.X), int(ptMouse.Y)))
				return true;
		}

		return result;
	}

	void MotionFuncGUI(Vec3 ptMouse)
	{
		for (auto iterElement : liGUI_Elements)
			iterElement->Drag(int(ptMouse.X), int(ptMouse.Y));
	}

	void ReshapeGUI(int iWidth, int iHeight)
	{
		for (auto iterElement : liGUI_Elements)
		{
			int px, py;
			iterElement->GetPosition(px, py);

			if (iterElement->hAlign == HALIGN_LEFT)
				px = -iWidth / 2 + iterElement->iHPosShift;
			else if (iterElement->hAlign == HALIGN_CENTER)
				px = iterElement->iHPosShift;
			else if (iterElement->hAlign == HALIGN_RIGHT)
				px = iWidth / 2 + iterElement->iHPosShift;

			if (iterElement->vAlign == VALIGN_BOTTOM)
				py = -iHeight / 2 + iterElement->iVPosShift;
			else if (iterElement->vAlign == VALIGN_CENTER)
				py = iterElement->iVPosShift;
			else if (iterElement->vAlign == VALIGN_TOP)
				py = iHeight / 2 + iterElement->iVPosShift;

			iterElement->Reposition(px, py);
		}
	}


protected:
private:

};


#endif