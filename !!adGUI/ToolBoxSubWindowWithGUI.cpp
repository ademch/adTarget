#include "stdafx.h"
#include "ToolBoxSubWindowWithGUI.h"
#include "../!!adGlobals/adOpenGLUtilities.h"


ToolBoxSubWindowWithGUI::ToolBoxSubWindowWithGUI(int iParentWidth, int iParentHeight,
												 float fBottomLeftXperc, float fBottomLeftYperc,
												 float fWidthPerc, float fHeightPerc) :
	OpenGLSubWindowWithGUI(iParentWidth, iParentHeight, 
		                   fBottomLeftXperc, fBottomLeftYperc, fWidthPerc, fHeightPerc)
{
}


ToolBoxSubWindowWithGUI::~ToolBoxSubWindowWithGUI()
{
}

void ToolBoxSubWindowWithGUI::Draw()
{
	OpenGLSubWindowWithGUI::Draw();

	// propagate common values
	for (auto iterElement : liTools)
		iterElement->fUserScale = fUserScale;

	SetupGraphicsPipeline();

		for (auto iterElement : liTools)
			iterElement->Render();

	SetupGraphicsPipelineWithIdentityModelViewMatrix();

		for (auto iterElement : liTools)
			iterElement->RenderGUI();

}


void ToolBoxSubWindowWithGUI::Reshape(int iBottomLeftX, int iBottomLeftY, int iWidth, int iHeight)
{
	OpenGLSubWindowWithGUI::Reshape(iBottomLeftX, iBottomLeftY, iWidth, iHeight);

	for (auto iterElement : liTools)
		iterElement->Reshape(iBottomLeftX, iBottomLeftY, iWidth, iHeight);
}


// Passive motion is special, global window cares about all windows
// to make sure focus, cursor is updated correcly. We do not check for boundaries
bool ToolBoxSubWindowWithGUI::PassiveMotionFunc(int x, int y)
{
	bool bResult = OpenGLSubWindowWithGUI::PassiveMotionFunc(x, y);
	// if some GUI element of Editor handled the event then do not traverse further
	if (bResult) return true;

	// Tools are hendled before their GUI
	SetupGraphicsPipeline();

	Vec3d v3DCoords;
	gluUnProjectFriendly(x, y, 0, v3DCoords.X, v3DCoords.Y, v3DCoords.Z);

	for (auto iterElement : liTools)
	{
		bResult = iterElement->PassiveMotionFunc(Vecc3(v3DCoords));
		// if some Tool handled the event then do not traverse further
		if (bResult) return true;
	}

	// Tools' GUI are hendled after tools itself
	SetupGraphicsPipelineWithIdentityModelViewMatrix();

	gluUnProjectFriendly(x, y, 0, v3DCoords.X, v3DCoords.Y, v3DCoords.Z);

	for (auto iterElement : liTools)
	{
		bResult = iterElement->PassiveMotionFuncGUI(Vecc3(v3DCoords));
		// if some Tool GUI handled the event then do not traverse further
		if (bResult) return true;
	}

	return false;
}


// Mouse button clicked callback
bool ToolBoxSubWindowWithGUI::MouseFunc(int button, int state, int x, int y)
{
	if (OpenGLSubWindowWithGUI::MouseFunc(button, state, x, y)) return true;

	if ((x > m_iBottomLeftX) && (x < m_iBottomLeftX + m_iWidth) &&
		(y > m_iBottomLeftY) && (y < m_iBottomLeftY + m_iHeight))
	{
		SetupGraphicsPipeline();

		Vec3d v3DCoords;
		gluUnProjectFriendly(x, y, 0, v3DCoords.X, v3DCoords.Y, v3DCoords.Z);

		for (auto iterElement : liTools)
		{
			if (iterElement->MouseFunc(button, state, Vecc3(v3DCoords))) return true;
		}

		SetupGraphicsPipelineWithIdentityModelViewMatrix();

		gluUnProjectFriendly(x, y, 0, v3DCoords.X, v3DCoords.Y, v3DCoords.Z);

		for (auto iterElement : liTools)
		{
			if (iterElement->MouseFuncGUI(button, state, Vecc3(v3DCoords))) return true;
		}
	}

	return false;

}

// Motion with button pressed
void ToolBoxSubWindowWithGUI::MotionFunc(int x, int y)
{
	OpenGLSubWindow::MotionFunc(x, y);

	if ((x > m_iBottomLeftX) && (x < m_iBottomLeftX + m_iWidth) &&
		(y > m_iBottomLeftY) && (y < m_iBottomLeftY + m_iHeight))
	{
		// Process GUI first
		MotionFuncGUI(x, y);

		SetupGraphicsPipeline();

		Vec3d v3DCoords;
		gluUnProjectFriendly(x, y, 0, v3DCoords.X, v3DCoords.Y, v3DCoords.Z);

		for (auto iterElement : liTools)
			iterElement->MotionFunc(Vecc3(v3DCoords));

		SetupGraphicsPipelineWithIdentityModelViewMatrix();

		gluUnProjectFriendly(x, y, 0, v3DCoords.X, v3DCoords.Y, v3DCoords.Z);

		for (auto iterElement : liTools)
			iterElement->MotionFuncGUI(Vecc3(v3DCoords));
	}

}

bool ToolBoxSubWindowWithGUI::KeyboardFunc(unsigned char key, int x, int y)
{
	if (OpenGLSubWindow::KeyboardFunc(key, x, y)) return true;

	return false;
}
