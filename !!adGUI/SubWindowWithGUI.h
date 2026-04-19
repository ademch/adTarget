
#ifndef OPENGLSUBWINDOW_WITH_GUI_H
#define OPENGLSUBWINDOW_WITH_GUI_H

#include "SubWindow.h"
#include "../../!!adGlobals/adOpenGLUtilities.h"
#include "../!!adGlobals/vector_math.h"
#include "gui_element.h"
#include <gl\gl.h>

#include <vector>

class OpenGLSubWindowWithGUI : public OpenGLSubWindow
{
public:

	OpenGLSubWindowWithGUI(int iBottomLeftX, int iBottomLeftY, int iWidth, int iHeight) :
		                   OpenGLSubWindow(iBottomLeftX, iBottomLeftY, iWidth, iHeight)
	{
	}
	virtual ~OpenGLSubWindowWithGUI() {}

protected:

	std::vector<GUI_Element*> liGUI_Elements;

	void RenderGUI()
	{
		SetupGraphicsPipelineWithIdentityModelViewMatrix();

		for (auto iterElement : liGUI_Elements)
			iterElement->Draw();
	}

	void PassiveMotionFuncGUI(int x, int y)
	{
		SetupGraphicsPipelineWithIdentityModelViewMatrix();

		Vec3d v3DCoords;
		gluUnProjectFriendly(x, y, 0, v3DCoords.X, v3DCoords.Y, v3DCoords.Z);

		for (auto iterElement : liGUI_Elements)
			iterElement->Hover(v3DCoords.X, v3DCoords.Y);
	}

	bool MouseFuncGUI(int button, int state, int x, int y)
	{
		bool result = false;

		SetupGraphicsPipelineWithIdentityModelViewMatrix();

		Vec3d v3DCoords;
		gluUnProjectFriendly(x, y, 0, v3DCoords.X, v3DCoords.Y, v3DCoords.Z);

		for (auto iterElement : liGUI_Elements)
		{
			if (iterElement->Clicked(button, state, v3DCoords.X, v3DCoords.Y))
				return true;
		}

		return result;
	}

	void MotionFuncGUI(int x, int y)
	{
		SetupGraphicsPipelineWithIdentityModelViewMatrix();

		Vec3d v3DCoords;
		gluUnProjectFriendly(x, y, 0, v3DCoords.X, v3DCoords.Y, v3DCoords.Z);

		for (auto iterElement : liGUI_Elements)
			iterElement->Drag(v3DCoords.X, v3DCoords.Y);
	}

	void ReshapeGUI(int iWidth, int iHeight)
	{
		for (auto iterElement : liGUI_Elements)
		{
			int px, py;
			iterElement->GetPosition(px, py);

			if		(iterElement->hAlign == HALIGN_LEFT)
				px = -iWidth/2 + iterElement->iHPosShift;
			else if (iterElement->hAlign == HALIGN_CENTER)
				px =             iterElement->iHPosShift;
			else if (iterElement->hAlign == HALIGN_RIGHT)
				px =  iWidth/2 + iterElement->iHPosShift;

			if		(iterElement->vAlign == VALIGN_BOTTOM)
				py = -iHeight/2 + iterElement->iVPosShift;
			else if (iterElement->vAlign == VALIGN_CENTER)
				py =              iterElement->iVPosShift;
			else if (iterElement->vAlign == VALIGN_TOP)
				py =  iHeight/2 + iterElement->iVPosShift;

			iterElement->Reposition(px, py);
		}
	}


private:
};


#endif