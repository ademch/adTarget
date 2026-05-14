#include "stdafx.h"

#include "../!!adGlobals/glut/glut.h"
#include "../!!adGlobals/adOpenGLUtilities.h"
#include "TimelineTrack.h"
#include "../!!adGlobals/vector_math.h"


// By delaut the first track is active
int TimelineTrack::iSelected = 1;


TimelineTrack::TimelineTrack(int _id, int px, int py, int _width, int _height):
	                         id(_id), m_iWidth(_width), m_iHeight(_height)
{
	posx = px;
	posy = py;

	iHPosShift = px;
	iVPosShift = py;

	bMouseButtonPressed = false;

	bFocused = false;
	vColor_focused   = Vecc3(0.069, 0.35, 0.069);
	vColor_defocused = Vecc3(0.046, 0.2, 0.046);
}



void TimelineTrack::Draw()
{
	GUI_Element::Draw();

	// clear screen under control
	if ((iSelected == id) && bEnabled)
		glColor3fv(&vColor_focused.X);
	else
		glColor3fv(&vColor_defocused.X);
	glQuad(posx, posy, m_iWidth, m_iHeight, 0);

}

void TimelineTrack::Resize(int iWidth, int iHeight)
{
	m_iWidth  = iWidth;
	//m_iHeight = iHeight;
}

bool TimelineTrack::Clicked(int button, int state, int x, int y)
{
	GUI_Element::Clicked(button, state, x, y);

	Vec3 ptPeep = matrSliderNonInverted*Vecc3(x,y);

	if ((ptPeep.X < posx + m_iWidth)  && (ptPeep.X > posx) &&
		(ptPeep.Y < posy + m_iHeight) && (ptPeep.Y > posy))
	{
		if (!bEnabled) return false;

		if (state==GLUT_DOWN)
		{
			iSelected = id;

			return false;
		}
	}

	return false;
}


