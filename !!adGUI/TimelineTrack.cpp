#include "stdafx.h"

#include "../!!adGlobals/glut/glut.h"
#include "../!!adGlobals/adOpenGLUtilities.h"
#include "glfont.h"
#include <assert.h>
#include "TimelineTrack.h"
#include "../!!adGlobals/vector_math.h"
#include "gui_element.h"
#include <string>
#include <functional>

extern GLFONT font;


TimelineTrack::TimelineTrack(int px, int py, int _width, int _height):
	                         m_iWidth(_width), m_iHeight(_height)
{
	posx = px;
	posy = py;

	iHPosShift = px;
	iVPosShift = py;

	m_fSliderX = 0.0;

	bMouseButtonPressed = false;
	bEnabled = true;

	bFocused = false;
	vColor_focused   = Vecc3(0.1, 0.8 ,0.1);	// 0.04, 0.18, 0.04
	vColor_defocused = Vecc3(0.1, 0.5, 0.1);

	matrSliderNonInverted = Mat4MakeIdent();

}



void TimelineTrack::Draw()
{
	GUI_Element::Draw();

	Matr4 matrSliderInverted;
	glGetFloatv(GL_MODELVIEW_MATRIX,  &matrSliderInverted.m[0][0]);

	// clear screen under control
	glColor3f(0.023, 0.095, 0.023);
	glQuad(posx, posy, m_iWidth, m_iHeight, 0);


	//if (bFocused && bEnabled)
	//	glColor3fv(&vColor_focused.X);
	//else
	glColor3fv(&vColor_defocused.X);
	glWireRectangle(posx, posy, m_iWidth, m_iHeight, 3);


	// draw slider line
	//
	// Slider coordinates are world coordinates in the range (eg -300...300)
	// scaled to a range (eg -200...200) during its capture in the mouse handler.
	// Current modelview matrix brings them back to -300...300, but the value now
	// is not with granularity +1, but with granularity 1/scale
	{
		glColor3f(1,0,0);

		glLineWidth(1);
		//glLine( m_fSliderX, posy,
		//	    m_fSliderX, posy - m_iHeight,     5);
	}

}

void TimelineTrack::Resize(int iWidth, int iHeight)
{
	m_iWidth  = iWidth;
	m_iHeight = iHeight;
}

bool TimelineTrack::Clicked(int button, int state, int x, int y)
{
	GUI_Element::Clicked(button, state, x, y);

	// special case, component is symmetric along zero
	if ((x > posx - 1) && (x < posx + m_iWidth + 1) &&
		(y > posy - m_iHeight/2) && (y < posy + m_iHeight/2))
	{
		if (!bEnabled) return false;

		if (state==GLUT_DOWN)
		{
			// Transform-scale input world coords of a slider using matrix from HorScrollBar.
			// The matrix is specially organized in a way the world coordinates (-300...300)
			// become a peephole with N times greater precision than 1/(600)
			m_fSliderX = (matrSliderNonInverted * Vecc3(x)).X;
			//printf("%5.3f\n", m_fSliderX);

			bMouseButtonPressed = true;
			return true;
		}
	}

	if (bMouseButtonPressed)
	{
		if (OnClick != NULL) OnClick();
		bMouseButtonPressed = false;

		return true;
	}

	return false;
}


bool TimelineTrack::Drag(int x, int y)
{
	GUI_Element::Drag(x, y);

	if (bMouseButtonPressed && (x > posx - 1) && (x < posx + m_iWidth + 1) )
	{
		// Transform-scale input world coords of a slider using matrix from HorScrollBar.
		// The matrix is specially organized in a way the world coordinates (-300...300)
		// become a peephole with N times greater precision than 1/(600)
		m_fSliderX = (matrSliderNonInverted * Vecc3(x)).X;

		if (OnClickDrag != NULL) OnClickDrag();

		return true;
	}
	return false;
}


bool TimelineTrack::Hover(int x, int y)
{
	GUI_Element::Hover(x, y);

	if ((x < posx + m_iWidth + 1) && (x > posx - 1) &&
		(y < posy + m_iHeight)    && (y > posy))
	{
		bFocused = bEnabled;
		return true;
	}

	bFocused = false;

	return false;
}

bool TimelineTrack::Wheel(int state,int delta,int x,int y)
{ 
	GUI_Element::Wheel(state, delta, x, y);

	if (!bFocused) return false;

	float fDelta = float(delta)/120.0;

	if (OnClick != NULL) OnClick();

	return true;
}
