#include "stdafx.h"

#include "../!!adGlobals/glut/glut.h"
#include "../!!adGlobals/adOpenGLUtilities.h"
#include "TrackClip.h"
#include "../!!adGlobals/vector_math.h"
#include "gui_element.h"
#include <functional>


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
	bEnabled = true;

	bFocused = false;
	vColor_focused   = Vecc3(0.1, 0.8 ,0.1);	// 0.04, 0.18, 0.04
	vColor_defocused = Vecc3(0.1, 0.5, 0.1);

	m_fHandleStartTX = 0.0;
	m_fHandleEndTX   = 0.0;

}



void TrackClip::Draw()
{
	GUI_Element::Draw();

	Matr4 matrSliderInverted;
	glGetFloatv(GL_MODELVIEW_MATRIX,  &matrSliderInverted.m[0][0]);

	// clear screen under control
	if ((iSelected == id) && bEnabled)
		glColor3f(0.69, 0.69, 0.069);
	else
		glColor3f(0.46, 0.2, 0.046);
	glQuad(posx + m_fHandleStartTX*m_iWidth, posy, (m_fHandleEndTX - m_fHandleStartTX)*m_iWidth, m_iHeight, 10);


}

void TrackClip::Resize(int iWidth, int iHeight)
{
	m_iWidth  = iWidth;
	//m_iHeight = iHeight;
}

bool TrackClip::Clicked(int button, int state, int x, int y)
{
	GUI_Element::Clicked(button, state, x, y);

	Vec3 vCoord = matrSliderNonInverted*Vecc3(x,y);

	if ((vCoord.X < posx + m_iWidth)  && (vCoord.X > posx) &&
		(vCoord.Y < posy + m_iHeight) && (vCoord.Y > posy))
	{
		if (!bEnabled) return false;

		if (state==GLUT_DOWN)
		{
			// Transform-scale input world coords of a slider using matrix from HorScrollBar.
			// The matrix is specially organized in a way the world coordinates (-300...300)
			// become a peephole with N times greater precision than 1/(600)
			//m_fSliderX = (matrSliderNonInverted * Vecc3(x)).X;
			//printf("%5.3f\n", m_fSliderX);

			//bMouseButtonPressed = true;

			iSelected = id;

			return true;
		}
	}

	//if (bMouseButtonPressed)
	//{
	//	if (OnClick != NULL) OnClick();
	//	bMouseButtonPressed = false;

	//	return true;
	//}

	return false;
}


bool TrackClip::Drag(int x, int y)
{
	GUI_Element::Drag(x, y);

	Vec3 vCoord = matrSliderNonInverted*Vecc3(x,y);

	//if (bMouseButtonPressed && (vCoord.X < posx + m_iWidth)  && (vCoord.X > posx))
	//{
	//	// Transform-scale input world coords of a slider using matrix from HorScrollBar.
	//	// The matrix is specially organized in a way the world coordinates (-300...300)
	//	// become a peephole with N times greater precision than 1/(600)
	//	//m_fSliderX = (matrSliderNonInverted * Vecc3(x)).X;

	//	if (OnClickDrag != NULL) OnClickDrag();

	//	return true;
	//}
	return false;
}


bool TrackClip::Hover(int x, int y)
{
	GUI_Element::Hover(x, y);

	Vec3 vCoord = matrSliderNonInverted*Vecc3(x,y);

	if ((vCoord.X < posx + m_iWidth)  && (vCoord.X > posx) &&
		(vCoord.Y < posy + m_iHeight) && (vCoord.Y > posy))
	{
		//bFocused = bEnabled;
		return true;
	}

	//bFocused = false;

	return false;
}
