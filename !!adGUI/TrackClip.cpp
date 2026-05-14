#include "stdafx.h"
#include "../!!adGlobals/glut/glut.h"
#include "../!!adGlobals/adOpenGLUtilities.h"
#include "TrackClip.h"
#include "../!!adGlobals/vector_math.h"
#include "VideoPositionMediator.h"


// By delaut the first track is active
int TrackClip::iSelected = 1;


TrackClip::TrackClip(int _id, int px, int py, int _width, int _height) :
	                 id(_id), m_iWidth(_width), m_iHeight(_height)
{
	posx = px;
	posy = py;

	iHPosShift = px;
	iVPosShift = py;

	bMouseButtonPressed = false;

	bFocused = false;
	vColor_focused   = Vecc3(0.1, 0.8 ,0.1);	// 0.04, 0.18, 0.04
	vColor_defocused = Vecc3(0.1, 0.5, 0.1);

	m_iStartPos = 0.0;
	m_iLength   = 0.0;

	xImmediateTranslate = 0.0f;
}



void TrackClip::Draw()
{
	GUI_ElementResizable::Draw();

	// Pixels per Second
	float fPPS = float(m_iWidth)/PositionMediator::Get()->Duration();

	// clear screen under control
	if ((iSelected == id) && bEnabled)
		glColor3f(0.69, 0.69, 0.069);
	else
		glColor3f(0.1, 0.1, 0.1);
	glQuad(posx + m_iStartPos*fPPS + xImmediateTranslate, posy, m_iLength*fPPS, m_iHeight, 10);

	if (bFocused)
	{
		glLineWidth(2.0);
		glColor3f(0.92, 0.8, 0.0);
		glWireRectangle(posx + m_iStartPos*fPPS + xImmediateTranslate, posy, m_iLength*fPPS, m_iHeight, 11);
	}

}

void TrackClip::Resize(int iWidth, int iHeight)
{
	m_iWidth  = iWidth;
	//m_iHeight = iHeight;
}


bool TrackClip::Hover(int x, int y)
{
	GUI_ElementResizable::Hover(x, y);

	float fPPS = float(m_iWidth)/PositionMediator::Get()->Duration();

	Vec3 ptPeep = matrSliderNonInverted*Vecc3(x,y);

	if ((ptPeep.X > posx + m_iStartPos*fPPS - 1) && (ptPeep.X < posx + (m_iStartPos + m_iLength)*fPPS + 1) &&
		(ptPeep.Y > posy)                        && (ptPeep.Y < posy + m_iHeight))
	{
		bFocused = bEnabled;

		if (abs(ptPeep.X - posx - m_iStartPos*fPPS) < 3.0*matrSliderNonInverted.m[0][0])
			glutSetCursor(GLUT_CURSOR_LEFT_RIGHT);
		else
		if (abs(ptPeep.X - posx - (m_iStartPos + m_iLength)*fPPS) < 3.0*matrSliderNonInverted.m[0][0])
			glutSetCursor(GLUT_CURSOR_LEFT_RIGHT);
		else
			glutSetCursor(GLUT_CURSOR_INHERIT);

		return true;
	}

	bFocused = false;

	return false;
}



bool TrackClip::Clicked(int button, int state, int x, int y)
{
	GUI_ElementResizable::Clicked(button, state, x, y);

	float fPPS = float(m_iWidth)/PositionMediator::Get()->Duration();

	Vec3 ptPeep = matrSliderNonInverted*Vecc3(x,y);

	if ((ptPeep.X > posx + m_iStartPos*fPPS) && (ptPeep.X < posx + (m_iStartPos + m_iLength)*fPPS) &&
		(ptPeep.Y > posy)                    && (ptPeep.Y < posy + m_iHeight))
	{
		if (!bEnabled) return false;

		if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN))
		{
			iBeginDragX = x;
			iBeginDragY = y;

			bMouseButtonPressed = true;

			iSelected = id;

			return true;
		}
	}

	if (bMouseButtonPressed)
	{
		m_iStartPos += round(xImmediateTranslate/fPPS);
		xImmediateTranslate = 0.0f;

		if (OnClick != NULL) OnClick();
		bMouseButtonPressed = false;

		return true;
	}

	return false;
}


bool TrackClip::Drag(int x, int y)
{
	GUI_ElementResizable::Drag(x, y);

	float fPPS = float(m_iWidth)/PositionMediator::Get()->Duration();

	if (bMouseButtonPressed)
	{
		if (abs(x - iBeginDragX) < 1) return false;

		// Transform-scale input world coords of a slider using matrix from HorScrollBar.
		// The matrix is specially organized in a way the world coordinates (-300...300)
		// become a peephole with N times greater precision than 1/(600)
		float fDeltaX = matrSliderNonInverted.m[0][0] * (x - iBeginDragX);

		// Precalculate how far handle goes out of the window after current drag and move it back
		{
			float fUnderflow = -m_iWidth/2.0 - (posx + m_iStartPos*fPPS + fDeltaX);
			float fOverflow  = (posx + (m_iStartPos + m_iLength)*fPPS + fDeltaX) - m_iWidth/2.0;

			// Accumulate translation in float type param and then move it on mouseUp into int
			if (fUnderflow > 0)
				xImmediateTranslate = fDeltaX + fUnderflow;
			else if (fOverflow > 0)
				xImmediateTranslate = fDeltaX - fOverflow;
			else
				xImmediateTranslate = fDeltaX;

			//printf("xImm=%5.3f\n", xImmediateTranslate);
		}

		if (OnClickDrag != NULL) OnClickDrag();

		return true;
	}
	return false;
}

