#include "stdafx.h"
#include "HorScrollBar.h"
#include <gl/gl.h>
#include "../!!adGlobals/glut/glut.h"
#include "../!!adGlobals/adOpenGLUtilities.h"

const unsigned int g_minZoomOutIndex = -20;

HorScrollBar::HorScrollBar(std::string caption, int px, int py, int width, int height)
{
	posx = px;
	posy = py;

	iHPosShift = px;
	iVPosShift = py;

	m_Width  = width;
	m_Height = height;

	m_ptHandleStartWorldCoords = Vecc3(-width/2.0, 0);
	m_ptHandleEndWorldCoords   = Vecc3( width/2.0, 0);

	bEnabled = true;
	bFocused = false;
	iGUIpushed = 0;

	vUserSceneTranslation     = Vecc3();
	bMouseSceneDragInProgress = false;

	matrUserScale = Mat4MakeIdent();

	iZoomIndex  = 0;
	fZoomFactor = 0.75f;

	vColor_focused   = Vecc4(0.1, 0.8, 0.1, 0.7);
	vColor_defocused = Vecc4(0.1, 0.5, 0.1, 0.7);
}



void HorScrollBar::Resize(int iWidth, int iHeight)
{
	m_Width = iWidth;

	m_ptHandleStartWorldCoords = Vecc3(-iWidth/2.0, 0);
	m_ptHandleEndWorldCoords   = Vecc3( iWidth/2.0, 0);
}


bool HorScrollBar::Hover(int x, int y)
{
	GUI_Element::Hover(x, y);

	if ((x > posx) && (x < posx + m_Width) && 
		(y > posy) && (y < posy + m_Height + 1))
	{
		bFocused = bEnabled;
		return true;
	}

	bFocused = false;
	iGUIpushed = 0;

	return false;
}


void HorScrollBar::Draw()
{
	GUI_Element::Draw();

	// clear screen under element
	glColor3f(0, 0, 0);
	glQuad(posx + iGUIpushed, posy - iGUIpushed, m_Width, m_Height, 3);

	glColor4fv(&vColor_defocused.X);

	glLineWidth(1);
	glWireRectangle(posx, posy, m_Width, m_Height, 4.5);

	if (bFocused && bEnabled)
		glColor4fv(&vColor_focused.X);
	else
		glColor4fv(&vColor_defocused.X);

	// Handle
	{
		cpuLoadIdentity();

		cpuTranslatef(vUserSceneTranslation.X, 0, 0);
		cpuMultMatrixf(matrUserScale);

		Vec3 ptHandleStartTrans = cpuPipelineVertex3fv(m_ptHandleStartWorldCoords);
		Vec3 ptHandleEndTrans   = cpuPipelineVertex3fv(m_ptHandleEndWorldCoords);
		float fLength = ptHandleEndTrans.X - ptHandleStartTrans.X;

		float iForcedAddition = 0.0;
		if (fLength < 5)
			iForcedAddition = 5 - fLength;

		glQuad(       ptHandleStartTrans.X + iGUIpushed - 1,		// startx
			   posy + ptHandleStartTrans.Y - iGUIpushed + 3,	// starty
			   fLength + iForcedAddition,	                    // width
			   m_Height-5,										// height
			   5);												// zcoord
	}

}

bool HorScrollBar::Clicked(int button, int state, int x, int y)
{
	if (GUI_Element::Clicked(button, state, x, y)) return true;

	if (!bEnabled) return false;

	if ((x < posx + m_Width)  && (x > posx) &&
		(y < posy + m_Height) && (y > posy))
	{
		if (button == GLUT_LEFT_BUTTON)
		{
			if (state == GLUT_DOWN)
			{
				bMouseSceneDragInProgress = true;

				iBeginDragX = x;
				iBeginDragY = y;

				iGUIpushed = 1;
				return true;
			}
		}
	}
	
	bMouseSceneDragInProgress = false;
	iGUIpushed = 0;

	return false;
}

// Does not depend on any OpenGL matrix
// x,y window coordinates from (0,0) to (w,h) (having y flipped of cause)
bool HorScrollBar::Drag(int x, int y)
{
	if (bMouseSceneDragInProgress)
	{
		// Precalculate how far handle goes out of the window after current drag and move it back
		{
			cpuLoadIdentity();

			cpuTranslatef(vUserSceneTranslation.X + (x - iBeginDragX), 0, 0);
			cpuMultMatrixf(matrUserScale);
			Vec3 ptHandleStartTrans = cpuPipelineVertex3fv(m_ptHandleStartWorldCoords);
			Vec3 ptHandleEndTrans   = cpuPipelineVertex3fv(m_ptHandleEndWorldCoords);

			float fUnderflow = -m_Width/2.0 - ptHandleStartTrans.X;
			float fOverflow  = ptHandleEndTrans.X - m_Width/2.0;

			if (fUnderflow > 0)
				vUserSceneTranslation.X += (x - iBeginDragX) + fUnderflow;
			else if (fOverflow > 0)
				vUserSceneTranslation.X += (x - iBeginDragX) - fOverflow;
			else
				vUserSceneTranslation.X += (x - iBeginDragX);
		}

		vUserSceneTranslation.Y += (y - iBeginDragY);

		if (OnChange != NULL) OnChange(Mat4MakeTrans(vUserSceneTranslation.X, 0, 0)*matrUserScale);

		iBeginDragX = x;
		iBeginDragY = y;

		return true;
	}

	return false;
}


bool HorScrollBar::Wheel(int state, int delta, int x, int y)
{
	GUI_Element::Wheel(state, delta, x, y);

	if ((x > posx) && (x < posx + m_Width) &&
		(y > posy) && (y < posy + m_Height))
	{
		if (bEnabled)
		{
			float fDelta = float(delta) / 120.0;

			if (fDelta < 0)
			{
				if (iZoomIndex - 1 <= g_minZoomOutIndex) return false;
				iZoomIndex--;
			}
			else
			{
				if (iZoomIndex + 1 > 0) return false;
				iZoomIndex++;
			}


			// 0. mouse comes here in World coordinates
			Vec2 ptMouseWorld2D = Vecc2(x, y);

			// Nice idea from Cooledit software: scroller full width corresponds to handle contents, ie. handle becomes a peephole.
			// That means user scrolls in the window and its contents (peephole) remains centered around scroll position.
			// That does not come automatically as window controlled by this slider is zoomed in the opposite direction than the slider
			//
			{
				float t = float(x + m_Width/2) / float(m_Width);

				cpuLoadIdentity();

				cpuTranslatef(vUserSceneTranslation.X, 0, 0);
				cpuMultMatrixf(matrUserScale);

				Vec3 ptHandleStartTrans = cpuPipelineVertex3fv(m_ptHandleStartWorldCoords);
				Vec3 ptHandleEndTrans   = cpuPipelineVertex3fv(m_ptHandleEndWorldCoords);

				float fHandleCurrentWidth = ptHandleEndTrans.X - ptHandleStartTrans.X;

				ptMouseWorld2D.X = ptHandleStartTrans.X + fHandleCurrentWidth*t;
			}

			// 1. remove user translation (During Draw matrUserScale is applied after modelview matrix gets translation)
			matrUserScale = Mat4MakeTrans(vUserSceneTranslation.X, vUserSceneTranslation.Y, 0.0)*matrUserScale;
			// 2. move mouse scaling point to the center
			matrUserScale = Mat4MakeTrans(-ptMouseWorld2D.X, -ptMouseWorld2D.Y, 0.0)*matrUserScale;

			// 3. scale
			if (fDelta < 0)
				matrUserScale = Mat4MakeScale(fZoomFactor, 1.0f, 1.0f)*matrUserScale;
			else
				matrUserScale = Mat4MakeScale(1.0/fZoomFactor, 1.0, 1.0f)*matrUserScale;

			// 4. move back to the ptMouseWorld2D
			matrUserScale = Mat4MakeTrans(ptMouseWorld2D.X, ptMouseWorld2D.Y, 0.0)*matrUserScale;
			// 5. reapply user translation
			matrUserScale = Mat4MakeTrans(-vUserSceneTranslation.X, -vUserSceneTranslation.Y, 0.0)*matrUserScale;

			// Try our new scale right away and move slider into the boundaries
			{
				cpuLoadIdentity();

				cpuTranslatef(vUserSceneTranslation.X, 0, 0);
				cpuMultMatrixf(matrUserScale);

				Vec3 ptHandleStartTrans = cpuPipelineVertex3fv(m_ptHandleStartWorldCoords);
				Vec3 ptHandleEndTrans   = cpuPipelineVertex3fv(m_ptHandleEndWorldCoords);

				float fUnderflow = -m_Width/2.0 - ptHandleStartTrans.X;
				float fOverflow  = ptHandleEndTrans.X - m_Width/2.0;

				if (fUnderflow > 0)
					vUserSceneTranslation.X += fUnderflow;
				else if (fOverflow > 0)
					vUserSceneTranslation.X += -fOverflow;
			}

										   // multiply the same way like during Draw: Translation then Scaling
			if (OnChange != NULL) OnChange(Mat4MakeTrans(vUserSceneTranslation.X, 0, 0)*matrUserScale);

			return true;
		}
	}

	return false;
}