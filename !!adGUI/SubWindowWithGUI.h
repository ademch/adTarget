
#ifndef OPENGLSUBWINDOW_WITH_GUI_H
#define OPENGLSUBWINDOW_WITH_GUI_H

#include "SubWindow.h"
#include "../../!!adGlobals/adOpenGLUtilities.h"
#include "../!!adGlobals/vector_math.h"
#include "gui_element.h"

#include <vector>

class OpenGLSubWindowWithGUI : public OpenGLSubWindow
{
public:

	OpenGLSubWindowWithGUI(int iBottomLeftX, int iBottomLeftY, int iWidth, int iHeight) :
		   OpenGLSubWindow(iBottomLeftX, iBottomLeftY, iWidth, iHeight)
	{
		fBottomLeftXperc = 0.0f;
		fBottomLeftYperc = 0.0f;

		fWidthPerc  = 1.0f;
		fHeightPerc = 1.0f;
	}
	OpenGLSubWindowWithGUI(int iParentWidth, int iParentHeight,
		                   float _fBottomLeftXperc, float _fBottomLeftYperc,
		                   float _fWidthPerc, float _fHeightPerc) :
		   OpenGLSubWindow(int(iParentWidth*_fBottomLeftXperc),
			               int(iParentHeight*_fBottomLeftYperc),
			               int(iParentWidth*_fWidthPerc),
			               int(iParentHeight*_fHeightPerc))
	{
		fBottomLeftXperc = _fBottomLeftXperc;
		fBottomLeftYperc = _fBottomLeftYperc;

		fWidthPerc  = _fWidthPerc;
		fHeightPerc = _fHeightPerc;
	}
	virtual ~OpenGLSubWindowWithGUI() {}

	void Reshape(int iBottomLeftX, int iBottomLeftY, int iWidth, int iHeight)
	{
		OpenGLSubWindow::Reshape(iBottomLeftX, iBottomLeftY, iWidth, iHeight);

		ReshapeGUI(iWidth, iHeight);
	}

	float fBottomLeftXperc;
	float fBottomLeftYperc;
	float fWidthPerc;
	float fHeightPerc;

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
			iterElement->Hover(int(v3DCoords.X), int(v3DCoords.Y));
	}

	bool MouseFuncGUI(int button, int state, int x, int y)
	{
		bool result = false;

		SetupGraphicsPipelineWithIdentityModelViewMatrix();

		Vec3d v3DCoords;
		gluUnProjectFriendly(x, y, 0, v3DCoords.X, v3DCoords.Y, v3DCoords.Z);

		for (auto iterElement : liGUI_Elements)
		{
			if (iterElement->Clicked(button, state, int(v3DCoords.X), int(v3DCoords.Y)))
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
			iterElement->Drag(int(v3DCoords.X), int(v3DCoords.Y));
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