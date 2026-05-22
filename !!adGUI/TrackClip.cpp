#include "stdafx.h"
#include "TrackClip.h"
#include "../!!adGlobals/glut/glut.h"
#include "../!!adGlobals/adOpenGLUtilities.h"
#include "../!!adGlobals/vector_math.h"
#include "VideoPositionMediator.h"

TrackClip* dragNdrop_Clip = NULL;

int TrackClip::iSelected = 0;


TrackClip::TrackClip(int _id, int px, int py, int _width, int _height) :
	                 id(_id), m_iWidth(_width), m_iHeight(_height)
{
	posx = px;
	posy = py;

	iHPosShift = px;
	iVPosShift = py;

	bFocused = false;
	vColor_focused   = Vecc3(0.1, 0.8 ,0.1);	// 0.04, 0.18, 0.04
	vColor_defocused = Vecc3(0.1, 0.5, 0.1);

	m_iStartPos = 0.0;
	m_iLength   = 0.0;

	xImmTransl = 0.0f;
	xImmBeg    = 0.0f;
	xImmEnd    = 0.0f;

	stateClip = STATE_CLIP_IDLE;

	windowTool      = NULL;
	extern_textureIcon = NULL;
}



void TrackClip::Draw()
{
	GUI_ElementResizable::Draw();

	// Pixels per Second
	float fPPS = float(m_iWidth)/PositionMediator::Get()->Duration();

	float fStartX = posx + m_iStartPos*fPPS + xImmTransl + xImmBeg;

	// draw control rectangle
	if (iSelected == id)
		glColor3f(0.92, 0.8, 0.0);
	else
		glColor3f(0.46, 0.4, 0.0);
	glQuad(fStartX, posy, m_iLength*fPPS - xImmBeg + xImmEnd, m_iHeight, 10);

	//if (iSelected == id)
	//{
	//	glColor3f(0.92, 0.8, 0.0);

	//	glLineWidth(1.0);
	//	glLine(fStartX,										posy,
	//		   fStartX+ m_iLength*fPPS - xImmBeg + xImmEnd,  posy, 12);
	//	glLine(fStartX,                                      posy + m_iHeight-1,
	//		   fStartX + m_iLength*fPPS - xImmBeg + xImmEnd, posy + m_iHeight-1, 12);

		if (bFocused)
		{
			glColor3f(0.69, 0.0, 0.0);
			glLineWidth(3.0);
			glLine(fStartX,										posy,
				   fStartX,										posy + m_iHeight, 12);
			glLine(fStartX + m_iLength*fPPS - xImmBeg + xImmEnd, posy,
				   fStartX + m_iLength*fPPS - xImmBeg + xImmEnd, posy + m_iHeight, 12);
		}
	//}

	float fIconAspect = float(extern_textureIcon->m_width)/float(extern_textureIcon->m_height);
	RenderTexturedQuad(extern_textureIcon->m_uiTextureID,				// id
					   fStartX + 1*matrSliderNonInverted.m[0][0],		// x
					   posy + 1,										// y
					   24*fIconAspect*matrSliderNonInverted.m[0][0],	// width
					   24,												// height
					   11 );											// z

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
	if (!bEnabled) return false;

	GUI_ElementResizable::Clicked(button, state, x, y);

	float fPPS = float(m_iWidth)/PositionMediator::Get()->Duration();

	Vec3 ptPeep = matrSliderNonInverted*Vecc3(x,y);

	if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN))
	{
		if ((ptPeep.X > posx + m_iStartPos*fPPS) && (ptPeep.X < posx + (m_iStartPos + m_iLength)*fPPS) &&
			(ptPeep.Y > posy)                    && (ptPeep.Y < posy + m_iHeight))
		{
			iBeginDragX = x;
			iBeginDragY = y;

			stateClip = STATE_CLIP_DRAG_POS;

			OnClipChange(windowTool);

			iSelected = id;

			return true;
		}
	}
	else if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
	{
		if ((abs(ptPeep.X - posx - m_iStartPos*fPPS) < 3.0*matrSliderNonInverted.m[0][0]) &&
	        (ptPeep.Y > posy) && (ptPeep.Y < posy + m_iHeight))
		{

			iBeginDragX = x;
			iBeginDragY = y;

			stateClip = STATE_CLIP_DRAG_HEAD;
			
			OnClipChange(windowTool);

			iSelected = id;

			return true;
		}
		else if ((abs(ptPeep.X - posx - (m_iStartPos + m_iLength)*fPPS) < 3.0*matrSliderNonInverted.m[0][0]) &&
				 (ptPeep.Y > posy) && (ptPeep.Y < posy + m_iHeight))
		{
			iBeginDragX = x;
			iBeginDragY = y;

			stateClip = STATE_CLIP_DRAG_TAIL;

			OnClipChange(windowTool);

			iSelected = id;

			return true;
		}
		else if ((ptPeep.X > posx + m_iStartPos*fPPS) && (ptPeep.X < posx + (m_iStartPos + m_iLength)*fPPS) &&
				 (ptPeep.Y > posy)                    && (ptPeep.Y < posy + m_iHeight))
		{
			OnClipChange(windowTool);

			iSelected = id;

			return true;
		}
	}

	if (stateClip == STATE_CLIP_DRAG_POS)
	{
		m_iStartPos += round(xImmTransl/fPPS);
		xImmTransl = 0.0f;

		if (OnClick != NULL) OnClick();
		stateClip = STATE_CLIP_IDLE;

		dragNdrop_Clip = NULL;

		return true;
	}

	if (stateClip == STATE_CLIP_DRAG_HEAD)
	{
		m_iStartPos += round(xImmBeg/fPPS);
		m_iLength   -= round(xImmBeg/fPPS);
		xImmBeg = 0.0f;

		if (OnClick != NULL) OnClick();
		stateClip = STATE_CLIP_IDLE;

		return true;
	}
	if (stateClip == STATE_CLIP_DRAG_TAIL)
	{
		m_iLength += round(xImmEnd/fPPS);
		xImmEnd = 0.0f;

		if (OnClick != NULL) OnClick();
		stateClip = STATE_CLIP_IDLE;

		return true;
	}

	return false;
}


bool TrackClip::Drag(int x, int y)
{
	GUI_ElementResizable::Drag(x, y);

	float fPPS = float(m_iWidth)/PositionMediator::Get()->Duration();

	// Ignore under 1 pixel drag for all cases
	if (abs(x - iBeginDragX) < 1) return false;

	if (stateClip == STATE_CLIP_DRAG_POS)
	{

		// Transform-scale input world coords of a slider using matrix from HorScrollBar.
		// The matrix is specially organized in a way the world coordinates (-300...300)
		// become a peephole with N times greater precision than 1/(600)
		float fDeltaX = matrSliderNonInverted.m[0][0] * (x - iBeginDragX);

		// Precalculate how far handle goes out of the window after current drag and move it back
		{
			float fUnderflow = -m_iWidth/2.0 - (posx + m_iStartPos*fPPS + fDeltaX);
			float fOverflow  =                 (posx + m_iStartPos*fPPS + m_iLength*fPPS + fDeltaX) - m_iWidth/2.0;

			// Accumulate translation in float type param and then move it on mouseUp into int
			if (fUnderflow > 0)
				xImmTransl = fDeltaX + fUnderflow;
			else if (fOverflow > 0)
				xImmTransl = fDeltaX - fOverflow;
			else
				xImmTransl = fDeltaX;

			//printf("xImm=%5.3f\n", xImmTransl);
		}

		dragNdrop_Clip = this;

		if (OnClickDrag != NULL) OnClickDrag();

		return true;
	}
	else if (stateClip == STATE_CLIP_DRAG_HEAD)
	{

		// Transform-scale input world coords of a slider using matrix from HorScrollBar.
		// The matrix is specially organized in a way the world coordinates (-300...300)
		// become a peephole with N times greater precision than 1/(600)
		float fDeltaX = matrSliderNonInverted.m[0][0] * (x - iBeginDragX);

		// Precalculate how far handle goes out of the window after current drag and move it back
		{
			float fUnderflow = -m_iWidth/2.0 - (posx + m_iStartPos*fPPS + fDeltaX);
			float fOverflow  =                 fDeltaX - m_iLength*fPPS;

			// Accumulate translation in float type param and then move it on mouseUp into int
			if (fUnderflow > 0)
				xImmBeg = fDeltaX + fUnderflow;
			else if (fOverflow > 0)
				xImmBeg = fDeltaX - fOverflow;
			else
				xImmBeg = fDeltaX;

			//printf("xImm=%5.3f\n", xImmTransl);
		}

		if (OnClickDrag != NULL) OnClickDrag();

		return true;
	}
	else if (stateClip == STATE_CLIP_DRAG_TAIL)
	{

		// Transform-scale input world coords of a slider using matrix from HorScrollBar.
		// The matrix is specially organized in a way the world coordinates (-300...300)
		// become a peephole with N times greater precision than 1/(600)
		float fDeltaX = matrSliderNonInverted.m[0][0] * (x - iBeginDragX);

		// Precalculate how far handle goes out of the window after current drag and move it back
		{
			float fUnderflow =                 -m_iLength*fPPS - fDeltaX;
			float fOverflow  =                 (posx + m_iStartPos*fPPS + m_iLength*fPPS + fDeltaX) - m_iWidth/2.0;

			// Accumulate translation in float type param and then move it on mouseUp into int
			if (fUnderflow > 0)
				xImmEnd = fDeltaX + fUnderflow;
			else if (fOverflow > 0)
				xImmEnd = fDeltaX - fOverflow;
			else
				xImmEnd = fDeltaX;

			//printf("xImm=%5.3f\n", xImmTransl);
		}

		if (OnClickDrag != NULL) OnClickDrag();

		return true;
	}

	return false;
}

