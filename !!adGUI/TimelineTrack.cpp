#include "stdafx.h"

#include "../!!adGlobals/glut/glut.h"
#include "../!!adGlobals/adOpenGLUtilities.h"
#include "TimelineTrack.h"
#include "../!!adGlobals/vector_math.h"
#include "TrackClip.h"


extern const int g_iClipPadding;
extern TrackClip* dragNdrop_Clip;


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
	vColor_focused   = Vecc3(0.069, 0.4, 0.069);
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
	if (!bEnabled) return false;

	GUI_Element::Clicked(button, state, x, y);

	Vec3 ptPeep = matrSliderNonInverted*Vecc3(x,y);

	if ((ptPeep.X < posx + m_iWidth)  && (ptPeep.X > posx) &&
		(ptPeep.Y < posy + m_iHeight) && (ptPeep.Y > posy))
	{
		if (state==GLUT_DOWN)
		{
			iSelected = id;

			return false;
		}
	}

	return false;
}


bool TimelineTrack::Drag(int x, int y)
{
	if (!bEnabled) return false;

	GUI_Element::Drag(x, y);

	Vec3 ptPeep = matrSliderNonInverted*Vecc3(x,y);

	if ((ptPeep.X < posx + m_iWidth)  && (ptPeep.X > posx) &&
		(ptPeep.Y < posy + m_iHeight) && (ptPeep.Y > posy))
	{
		if (dragNdrop_Clip)
		{
			if (dragNdrop_Clip->iTrack != id)
			{
				dragNdrop_Clip->iTrack = id;
				iSelected = id;

				dragNdrop_Clip->iHPosShift = iHPosShift;
				dragNdrop_Clip->iVPosShift = iVPosShift + g_iClipPadding;

				int px, py;
				dragNdrop_Clip->GetPosition(px, py);
				dragNdrop_Clip->Reposition(px, posy + g_iClipPadding);
			}

			return false;
		}
	}

	return false;
}


