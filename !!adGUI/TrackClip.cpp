#include "stdafx.h"
#include "TrackClip.h"
#include "../!!adGlobals/glut/glut.h"
#include "../!!adGlobals/adOpenGLUtilities.h"
#include "../!!adGlobals/vector_math.h"
#include "VideoPositionMediator.h"
#include "../!!adVideo/FFMS_VIdeo.h"
#include "TrackClipMenu.h"


TrackClip* dragNdrop_Clip = NULL;

int TrackClip::iSelected = 0;
std::vector<TrackClip*> TrackClip::liClips;


constexpr int const_iSnapPx       = 8;	// tracks are snapped to each other and to track start end boundary
constexpr int const_iDragRadiusPx = 5;


TrackClip::TrackClip(int _id, int px, int py, int _width, int _height)
{
	id   = _id;

	posx = px;
	posy = py;

	iHPosShift = px;
	iVPosShift = py;

	m_iWidth  = _width;
	m_iHeight = _height;

	bFocused = false;
	vColor_focused   = Vecc3(0.1, 0.8 ,0.1);	// 0.04, 0.18, 0.04
	vColor_defocused = Vecc3(0.1, 0.5, 0.1);

	m_iStartPos10msUnits = 0;
	m_iLength10msUnits   = 0;

	xImmTransl = 0.0f;
	xImmBeg    = 0.0f;
	xImmEnd    = 0.0f;

	stateClip = STATE_CLIP_IDLE;

	windowTool  = NULL;
	textureIcon = NULL;

	fPPU = 1.0;

	video = NULL;

	liKeyframesTRS      = NULL;
	liKeyframesMorphDst = NULL;
}

TrackClip::~TrackClip()
{
	delete textureIcon;
	delete video;
}



void TrackClip::Draw()
{
	GUI_ElementResizable::Draw();

	float fStartX = posx + m_iStartPos10msUnits*fPPU + xImmTransl + xImmBeg;

	// draw control rectangle
	if (iSelected == id)
		glColor3f(0.92, 0.8, 0.0);
	else
		glColor3f(0.46, 0.4, 0.0);
	glQuad(fStartX, posy, m_iLength10msUnits*fPPU - xImmBeg + xImmEnd, m_iHeight, 10);


	float f1PxCorrection = matrSliderNonInverted.m[0][0];
	if (bFocused && (iSelected == id))
	{
		glColor3f(0.69, 0.0, 0.0);
		glLineWidth(3.0);
		glLine( fStartX + f1PxCorrection,												posy,
				fStartX + f1PxCorrection,												posy + m_iHeight, 12);
		glLine( fStartX + m_iLength10msUnits*fPPU - xImmBeg + xImmEnd - f1PxCorrection, posy,
				fStartX + m_iLength10msUnits*fPPU - xImmBeg + xImmEnd - f1PxCorrection, posy + m_iHeight, 12);
	}

	// Draw icon
	float fIconAspRatio = float(textureIcon->m_width)/float(textureIcon->m_height);
	RenderTexturedQuad( textureIcon->m_uiTextureID,						// id
					    fStartX + 1*matrSliderNonInverted.m[0][0],		// x
					    posy + 1,										// y
					    24*fIconAspRatio*matrSliderNonInverted.m[0][0],	// width
					    24,												// height
					    11 );											// z

	// Draw animated params timeline
	if (liKeyframesTRS)
	{
		if (iSelected == id)
			glColor3f(0.8, 0.0, 0.9);
		else
			glColor3f(0.6, 0.0, 0.7);
		glLineWidth(1.0);
		glBegin(GL_LINE_STRIP);
		for (const auto& item : *liKeyframesTRS)
		{
			Vec3 pt = Vecc3(fStartX + item.time*100.0*fPPU, posy + 0.25*m_iHeight, 13);
			glVertex3fv(&pt.X);
		}
		glEnd();

		glPointSize(7.0);
		glBegin(GL_POINTS);
			for (const auto& item : *liKeyframesTRS)
			{
				Vec3 pt = Vecc3(fStartX + item.time*100.0*fPPU, posy + 0.25*m_iHeight, 15);
				glVertex3fv(&pt.X);
			}
		glEnd();
	}

	if (liKeyframesMorphDst)
	{
		if (iSelected == id)
			glColor3f(0.0, 0.2, 1.0);
		else
			glColor3f(0.0, 0.15, 0.75);
		glLineWidth(1.0);
		glBegin(GL_LINES);

			for (size_t i = 0; i + 1 < liKeyframesMorphDst->size(); ++i)
			{
				const auto& a = (*liKeyframesMorphDst)[i];
				const auto& b = (*liKeyframesMorphDst)[i + 1];

				if (a.value.size() != b.value.size())
					continue;

				Vec3 pt1 = Vecc3(fStartX + a.time * 100.0 * fPPU, posy + 0.75 * m_iHeight, 13.0f);
				Vec3 pt2 = Vecc3(fStartX + b.time * 100.0 * fPPU, posy + 0.75 * m_iHeight, 13.0f);

				glVertex3fv(&pt1.X);
				glVertex3fv(&pt2.X);
			}

		glEnd();

		glPointSize(7.0);
		glBegin(GL_POINTS);
			for (const auto& item : *liKeyframesMorphDst)
			{
				Vec3 pt = Vecc3(fStartX + item.time*100.0*fPPU, posy + 0.75*m_iHeight, 15);
				glVertex3fv(&pt.X);
			}
		glEnd();
	}


}


void TrackClip::Resize(int iWidth, int iHeight)
{
	m_iWidth  = iWidth;
	//m_iHeight = iHeight;

	// Pixels per 10ms
	fPPU = float(m_iWidth)/PositionMediator::Get()->Duration10msUnits();
}


float TrackClip::FindClipOnTrackBefore_TailPx(int iTrack, int iPos10msUnits)
{
	float iMax = -m_iWidth/2;
	for (auto iterEl : liClips)
	{
		if (iterEl->iTrack != iTrack) continue;		// skip clip from the other track
		if (iterEl == this)           continue;		// skip ourselves
		
		// skip tracks that start after iPos10msUnits (iPos10msUnits can be already inside the body, thats why check against start, not end)
		if (iterEl->m_iStartPos10msUnits /*+ iterEl->m_iLength10msUnits*fPPU*/ > iPos10msUnits) continue;

		iMax = max(iMax, posx + iterEl->m_iStartPos10msUnits*fPPU + iterEl->m_iLength10msUnits*fPPU);
	}

	return iMax;
}


float TrackClip::FindClipOnTrackAfter_HeadPx(int iTrack, int iPos10msUnits)
{
	float iMin = m_iWidth/2;
	for (auto iterEl : liClips)
	{
		// Skip clip from the other track
		if (iterEl->iTrack != iTrack) continue;
		// skip ourselves
		if (iterEl == this) continue;
		if (iterEl->m_iStartPos10msUnits + iterEl->m_iLength10msUnits - 1 < iPos10msUnits) continue;

		iMin = min(iMin, posx + iterEl->m_iStartPos10msUnits*fPPU);
	}

	return iMin;
}


float TrackClip::ClipsFitsIntoGapOnTrackImmediate(int iTrack)
{
	float fLeftWallPix  = dragNdrop_Clip->FindClipOnTrackBefore_TailPx(iTrack, m_iStartPos10msUnits + xImmTransl/fPPU);
	float fRightWallPix = dragNdrop_Clip->FindClipOnTrackAfter_HeadPx(iTrack, m_iStartPos10msUnits + xImmTransl/fPPU);

	if (fRightWallPix - fLeftWallPix - m_iLength10msUnits*fPPU < -0.0001)
		return false;

	return true; 
}


bool TrackClip::Hover(int x, int y)
{
	GUI_ElementResizable::Hover(x, y);

	Vec3 ptPeep = matrSliderNonInverted*Vecc3(x,y);

	if ((ptPeep.X > posx + m_iStartPos10msUnits*fPPU - 1) && (ptPeep.X < posx + (m_iStartPos10msUnits + m_iLength10msUnits)*fPPU + 1) &&
		(ptPeep.Y > posy)                                 && (ptPeep.Y < posy + m_iHeight))
	{
		if (iSelected != id) return false;
		
		bFocused = bEnabled;

		if (abs(ptPeep.X - posx - m_iStartPos10msUnits*fPPU)                        < const_iDragRadiusPx*matrSliderNonInverted.m[0][0])
		{
			glutSetCursor(GLUT_CURSOR_LEFT_RIGHT);
			return true;
		}
		else
		if (abs(ptPeep.X - posx - (m_iStartPos10msUnits + m_iLength10msUnits)*fPPU) < const_iDragRadiusPx*matrSliderNonInverted.m[0][0])
		{
			glutSetCursor(GLUT_CURSOR_LEFT_RIGHT);
			return true;
		}
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

	Vec3 ptPeep = matrSliderNonInverted*Vecc3(x,y);

	if ((button == GLUT_RIGHT_BUTTON) && (state == GLUT_DOWN))
	{
		if ((ptPeep.X > posx + m_iStartPos10msUnits*fPPU) && (ptPeep.X < posx + (m_iStartPos10msUnits + m_iLength10msUnits)*fPPU) &&
			(ptPeep.Y > posy)							  && (ptPeep.Y < posy + m_iHeight))
		{
			//iBeginDragX = x;
			//iBeginDragY = y;

			//stateClip = STATE_CLIP_DRAG_POS;

			OnClipChange(windowTool);

			iSelected = id;

			// SHOW MENU
			{
				POINT pt;
				GetCursorPos(&pt);

				TrackClipMenu::Get()->Show(pt.x, pt.y);
			}

			return true;
		}
	}
	else if ((button == GLUT_LEFT_BUTTON) && (state == GLUT_DOWN))
	{
		if ((abs(ptPeep.X - posx - m_iStartPos10msUnits*fPPU) < const_iDragRadiusPx*matrSliderNonInverted.m[0][0]) &&
	        (ptPeep.Y > posy) && (ptPeep.Y < posy + m_iHeight))
		{
			if (iSelected != id) return false;

			iBeginDragX = x;
			iBeginDragY = y;

			stateClip = STATE_CLIP_DRAG_HEAD;
			
			OnClipChange(windowTool);

			iSelected = id;

			return true;
		}
		else if ((abs(ptPeep.X - posx - (m_iStartPos10msUnits + m_iLength10msUnits)*fPPU) < const_iDragRadiusPx*matrSliderNonInverted.m[0][0]) &&
				 (ptPeep.Y > posy) && (ptPeep.Y < posy + m_iHeight))
		{
			if (iSelected != id) return false;

			iBeginDragX = x;
			iBeginDragY = y;

			stateClip = STATE_CLIP_DRAG_TAIL;

			OnClipChange(windowTool);

			iSelected = id;

			return true;
		}
		else if ((ptPeep.X > posx + m_iStartPos10msUnits*fPPU) && (ptPeep.X < posx + (m_iStartPos10msUnits + m_iLength10msUnits)*fPPU) &&
				 (ptPeep.Y > posy)							   && (ptPeep.Y < posy + m_iHeight))
		{
			
			iBeginDragX = x;
			iBeginDragY = y;

			stateClip = STATE_CLIP_DRAG_POS;
			
			OnClipChange(windowTool);

			iSelected = id;

			return true;
		}
	}

	if (stateClip == STATE_CLIP_DRAG_POS)
	{
		m_iStartPos10msUnits += round(xImmTransl/fPPU);
		xImmTransl = 0.0f;

		if (OnClick != NULL) OnClick();
		stateClip = STATE_CLIP_IDLE;

		dragNdrop_Clip = NULL;

		return true;
	}

	if (stateClip == STATE_CLIP_DRAG_HEAD)
	{
		m_iStartPos10msUnits += round(xImmBeg/fPPU);
		m_iLength10msUnits   -= round(xImmBeg/fPPU);
		xImmBeg = 0.0f;

		if (OnClick != NULL) OnClick();
		stateClip = STATE_CLIP_IDLE;

		return true;
	}
	if (stateClip == STATE_CLIP_DRAG_TAIL)
	{
		m_iLength10msUnits += round(xImmEnd/fPPU);
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

	// Ignore under 1 pixel drag for all cases
	if (abs(x - iBeginDragX) < 1) return false;

	if (stateClip == STATE_CLIP_DRAG_POS)
	{
		dragNdrop_Clip = this;

		// Transform-scale input world coords of a slider using matrix from HorScrollBar.
		// The matrix is specially organized in a way the world coordinates (-300...300)
		// become a peephole with N times greater precision than 1/(600)
		float fDeltaX = matrSliderNonInverted.m[0][0] * (x - iBeginDragX);

		// Precalculate how far handle goes out of the window after current drag and move it back
		{
			float fLeftWallPix  = FindClipOnTrackBefore_TailPx(iTrack, m_iStartPos10msUnits + fDeltaX/fPPU);
			float fRightWallPix = FindClipOnTrackAfter_HeadPx(iTrack,  m_iStartPos10msUnits + fDeltaX/fPPU);

			if (fRightWallPix - fLeftWallPix - m_iLength10msUnits*fPPU < -0.0001)
			{	
				// not enough space to fit
				return true;
			}

			float fUnderflow = fLeftWallPix - (posx + m_iStartPos10msUnits*fPPU + fDeltaX);
			float fOverflow  =                (posx + m_iStartPos10msUnits*fPPU + m_iLength10msUnits*fPPU + fDeltaX) - fRightWallPix;

			// Accumulate translation in float type param and then move it on mouseUp into int
			if (fUnderflow > -const_iSnapPx * matrSliderNonInverted.m[0][0])
				xImmTransl = fDeltaX + fUnderflow;
			else if (fOverflow > -const_iSnapPx * matrSliderNonInverted.m[0][0])
				xImmTransl = fDeltaX - fOverflow;
			else
				xImmTransl = fDeltaX;

			//printf("xImmTransl=%5.3f\n\n", xImmTransl);
		}

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
			float fLeftWallPix = FindClipOnTrackBefore_TailPx(iTrack, m_iStartPos10msUnits);

			float fUnderflow = fLeftWallPix - (posx + m_iStartPos10msUnits*fPPU + fDeltaX);
			float fOverflow  =                 fDeltaX - m_iLength10msUnits*fPPU;

			// Accumulate translation in float type param and then move it on mouseUp into int
			if (fUnderflow > -const_iSnapPx * matrSliderNonInverted.m[0][0])
				xImmBeg = fDeltaX + fUnderflow;
			else if (fOverflow > -fPPU)	// limit to 1 unit
				xImmBeg = fDeltaX - fOverflow - fPPU;
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

		float fRightWallPix = FindClipOnTrackAfter_HeadPx(iTrack, m_iStartPos10msUnits + m_iLength10msUnits);

		// Precalculate how far handle goes out of the window after current drag and move it back
		{
			float fUnderflow =									 - m_iLength10msUnits*fPPU - fDeltaX;	// tail should not be earlier than head
			float fOverflow  = (posx + m_iStartPos10msUnits*fPPU + m_iLength10msUnits*fPPU + fDeltaX) - fRightWallPix;

			// Accumulate translation in float type param and then move it on mouseUp into int
			if (fUnderflow > -fPPU)	// limit to 1 unit
				xImmEnd = fDeltaX + fUnderflow + fPPU;
			else if (fOverflow > -const_iSnapPx * matrSliderNonInverted.m[0][0])
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

// ----------------ANIMATED PARAMS REGISTRATION----------------------------------

void TrackClip::RegisterTRSparam(std::vector<ParamKeyframeTRSTransform>* _liKeyframesTRS)
{
	liKeyframesTRS = _liKeyframesTRS;
}

void TrackClip::RegisterMorphDSTparam(std::vector<ParamKeyframePolyline2D>* _liKeyframesMorphDst)
{
	liKeyframesMorphDst = _liKeyframesMorphDst;
}


///////////////////////////////////////////////////////////////////////////////////////////////////

TrackClip* TrackClip::GetClip(OpenGLSubWindowWithGUI* wnd)
{
	for (auto iterClip : liClips)
	{
		if (iterClip->windowTool == wnd) return iterClip;
	}
	return NULL;
}


void TrackClip::RemoveSelectedClip()
{
	for (auto it = liClips.begin(); it != liClips.end(); ++it)
	{
		if ((*it)->id == iSelected)
		{
			//delete *it;				// if you own the pointer
			it = liClips.erase(it);		// erase returns next iterator

			iSelected = 0;

			break;
		}
	}
}

TrackClip* TrackClip::GetSelectedClip()
{
	for (auto it = liClips.begin(); it != liClips.end(); ++it)
	{
		if ((*it)->id == iSelected)
			return *it;
	}
	return NULL;
}


double TrackClip::GetSelectedClipLocalTimeS()
{
	TrackClip* clip = TrackClip::GetSelectedClip();
	if (!clip) return 0;	// eg welcome screen

	int iPlayhead10msTicks = PositionMediator::Get()->Pos10msUnits();

	return (iPlayhead10msTicks - clip->m_iStartPos10msUnits)/100.0;
}
