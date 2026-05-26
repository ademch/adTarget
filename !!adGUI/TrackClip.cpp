#include "stdafx.h"
#include "TrackClip.h"
#include "../!!adGlobals/glut/glut.h"
#include "../!!adGlobals/adOpenGLUtilities.h"
#include "../!!adGlobals/vector_math.h"
#include "VideoPositionMediator.h"

TrackClip* dragNdrop_Clip = NULL;

int TrackClip::iSelected = 0;
std::vector<TrackClip*> TrackClip::liClips;

constexpr int const_iSnapPx       = 8;
constexpr int const_iDragRadiusPx = 5;


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

	m_iStartPosFrame = 0.0;
	m_iLengthFrames   = 0.0;

	xImmTransl = 0.0f;
	xImmBeg    = 0.0f;
	xImmEnd    = 0.0f;

	stateClip = STATE_CLIP_IDLE;

	windowTool      = NULL;
	extern_textureIcon = NULL;

	fPPF = 1.0;
}



void TrackClip::Draw()
{
	GUI_ElementResizable::Draw();

	float fStartX = posx + m_iStartPosFrame*fPPF + xImmTransl + xImmBeg;

	// draw control rectangle
	if (iSelected == id)
		glColor3f(0.92, 0.8, 0.0);
	else
		glColor3f(0.46, 0.4, 0.0);
	glQuad(fStartX, posy, m_iLengthFrames*fPPF - xImmBeg + xImmEnd, m_iHeight, 10);


	float f1PxCorrection = matrSliderNonInverted.m[0][0];
	if (bFocused && (iSelected == id))
	{
		glColor3f(0.69, 0.0, 0.0);
		glLineWidth(3.0);
		glLine( fStartX + f1PxCorrection,											 posy,
				fStartX + f1PxCorrection,											 posy + m_iHeight, 12);
		glLine( fStartX + m_iLengthFrames*fPPF - xImmBeg + xImmEnd - f1PxCorrection, posy,
				fStartX + m_iLengthFrames*fPPF - xImmBeg + xImmEnd - f1PxCorrection, posy + m_iHeight, 12);
	}

	// Draw icon
	float fIconAspRatio = float(extern_textureIcon->m_width)/float(extern_textureIcon->m_height);
	RenderTexturedQuad(extern_textureIcon->m_uiTextureID,				// id
					   fStartX + 1*matrSliderNonInverted.m[0][0],		// x
					   posy + 1,										// y
					   24*fIconAspRatio*matrSliderNonInverted.m[0][0],	// width
					   24,												// height
					   11 );											// z

}

void TrackClip::Resize(int iWidth, int iHeight)
{
	m_iWidth  = iWidth;
	//m_iHeight = iHeight;

	// Pixels per Frame
	fPPF = float(m_iWidth)/PositionMediator::Get()->DurationFrames();
}


float TrackClip::FindClipOnTrackBeforeFrame_TailPx(int iTrack, int iPosFrame)
{
	float iMax = -m_iWidth/2.0;
	for (auto iterEl : liClips)
	{
		if (iterEl->iTrack != iTrack) continue;		// skip clip from the other track
		if (iterEl == this)           continue;		// skip ourselves
		
		// skip tracks that start after iPosFrame (iPosFrame can be already inside the body, thats why check against start, not end)
		if (iterEl->m_iStartPosFrame /*+ iterEl->m_iLengthFrames*fPPF*/ > iPosFrame) continue;

		iMax = max(iMax, posx + iterEl->m_iStartPosFrame*fPPF + iterEl->m_iLengthFrames*fPPF);
	}

	return iMax;
}


float TrackClip::FindClipOnTrackAfterFrame_HeadPx(int iTrack, int iPosFrame)
{
	float iMin = m_iWidth/2.0;
	for (auto iterEl : liClips)
	{
		// Skip clip from the other track
		if (iterEl->iTrack != iTrack) continue;
		// skip ourselves
		if (iterEl == this) continue;
		if (iterEl->m_iStartPosFrame + iterEl->m_iLengthFrames - 1 < iPosFrame) continue;

		iMin = min(iMin, posx + iterEl->m_iStartPosFrame*fPPF);
	}

	return iMin;
}

float TrackClip::ClipsFitsIntoGapOnTrackImmediate(int iTrack)
{
	float fLeftWallPix  = dragNdrop_Clip->FindClipOnTrackBeforeFrame_TailPx(iTrack, m_iStartPosFrame + xImmTransl/fPPF);
	float fRightWallPix = dragNdrop_Clip->FindClipOnTrackAfterFrame_HeadPx(iTrack, m_iStartPosFrame + xImmTransl/fPPF);

	if (fRightWallPix - fLeftWallPix - m_iLengthFrames*fPPF < -0.0001)
		return false;

	return true; 
}


bool TrackClip::Hover(int x, int y)
{
	GUI_ElementResizable::Hover(x, y);

	Vec3 ptPeep = matrSliderNonInverted*Vecc3(x,y);

	if ((ptPeep.X > posx + m_iStartPosFrame*fPPF - 1) && (ptPeep.X < posx + (m_iStartPosFrame + m_iLengthFrames)*fPPF + 1) &&
		(ptPeep.Y > posy)                             && (ptPeep.Y < posy + m_iHeight))
	{
		if (iSelected != id) return false;
		
		bFocused = bEnabled;

		if (abs(ptPeep.X - posx - m_iStartPosFrame*fPPF) < const_iDragRadiusPx*matrSliderNonInverted.m[0][0])
		{
			glutSetCursor(GLUT_CURSOR_LEFT_RIGHT);
			return true;
		}
		else
		if (abs(ptPeep.X - posx - (m_iStartPosFrame + m_iLengthFrames)*fPPF) < const_iDragRadiusPx*matrSliderNonInverted.m[0][0])
		{
			glutSetCursor(GLUT_CURSOR_LEFT_RIGHT);
			return true;
		}
		else
			glutSetCursor(GLUT_CURSOR_INHERIT);

		return false;
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
		if ((ptPeep.X > posx + m_iStartPosFrame*fPPF) && (ptPeep.X < posx + (m_iStartPosFrame + m_iLengthFrames)*fPPF) &&
			(ptPeep.Y > posy)					      && (ptPeep.Y < posy + m_iHeight))
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
		if ((abs(ptPeep.X - posx - m_iStartPosFrame*fPPF) < const_iDragRadiusPx*matrSliderNonInverted.m[0][0]) &&
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
		else if ((abs(ptPeep.X - posx - (m_iStartPosFrame + m_iLengthFrames)*fPPF) < const_iDragRadiusPx*matrSliderNonInverted.m[0][0]) &&
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
		else if ((ptPeep.X > posx + m_iStartPosFrame*fPPF) && (ptPeep.X < posx + (m_iStartPosFrame + m_iLengthFrames)*fPPF) &&
				 (ptPeep.Y > posy)						   && (ptPeep.Y < posy + m_iHeight))
		{
			OnClipChange(windowTool);

			iSelected = id;

			return true;
		}
	}

	if (stateClip == STATE_CLIP_DRAG_POS)
	{
		m_iStartPosFrame += round(xImmTransl/fPPF);
		xImmTransl = 0.0f;

		if (OnClick != NULL) OnClick();
		stateClip = STATE_CLIP_IDLE;

		dragNdrop_Clip = NULL;

		return true;
	}

	if (stateClip == STATE_CLIP_DRAG_HEAD)
	{
		m_iStartPosFrame += round(xImmBeg/fPPF);
		m_iLengthFrames  -= round(xImmBeg/fPPF);
		xImmBeg = 0.0f;

		if (OnClick != NULL) OnClick();
		stateClip = STATE_CLIP_IDLE;

		return true;
	}
	if (stateClip == STATE_CLIP_DRAG_TAIL)
	{
		m_iLengthFrames += round(xImmEnd/fPPF);
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
		// Transform-scale input world coords of a slider using matrix from HorScrollBar.
		// The matrix is specially organized in a way the world coordinates (-300...300)
		// become a peephole with N times greater precision than 1/(600)
		float fDeltaX = matrSliderNonInverted.m[0][0] * (x - iBeginDragX);

		// Precalculate how far handle goes out of the window after current drag and move it back
		{
			float fLeftWallPix  = FindClipOnTrackBeforeFrame_TailPx(iTrack, m_iStartPosFrame + fDeltaX/fPPF);
			float fRightWallPix = FindClipOnTrackAfterFrame_HeadPx(iTrack,  m_iStartPosFrame + fDeltaX/fPPF);

			if (fRightWallPix - fLeftWallPix - m_iLengthFrames*fPPF < -0.0001)
			{	
				// not enough space to fit
				return true;
			}

			float fUnderflow = fLeftWallPix - (posx + m_iStartPosFrame*fPPF + fDeltaX);
			float fOverflow  =                (posx + m_iStartPosFrame*fPPF + m_iLengthFrames*fPPF + fDeltaX) - fRightWallPix;

			// Accumulate translation in float type param and then move it on mouseUp into int
			if (fUnderflow > -const_iSnapPx * matrSliderNonInverted.m[0][0])
				xImmTransl = fDeltaX + fUnderflow;
			else if (fOverflow > -const_iSnapPx * matrSliderNonInverted.m[0][0])
				xImmTransl = fDeltaX - fOverflow;
			else
				xImmTransl = fDeltaX;

			//printf("xImmTransl=%5.3f\n\n", xImmTransl);
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
			float fLeftWallPix = FindClipOnTrackBeforeFrame_TailPx(iTrack, m_iStartPosFrame);

			float fUnderflow = fLeftWallPix - (posx + m_iStartPosFrame*fPPF + fDeltaX);
			float fOverflow  =                 fDeltaX - m_iLengthFrames*fPPF;

			// Accumulate translation in float type param and then move it on mouseUp into int
			if (fUnderflow > -const_iSnapPx * matrSliderNonInverted.m[0][0])
				xImmBeg = fDeltaX + fUnderflow;
			else if (fOverflow > -fPPF)	// limit to 1 frame
				xImmBeg = fDeltaX - fOverflow - fPPF;
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

		float fRightWallPix = FindClipOnTrackAfterFrame_HeadPx(iTrack, m_iStartPosFrame*fPPF + m_iLengthFrames*fPPF);

		// Precalculate how far handle goes out of the window after current drag and move it back
		{
			float fUnderflow =                                -m_iLengthFrames*fPPF - fDeltaX;	// tail should not be earlier than head
			float fOverflow  = (posx + m_iStartPosFrame*fPPF + m_iLengthFrames*fPPF + fDeltaX) - fRightWallPix;

			// Accumulate translation in float type param and then move it on mouseUp into int
			if (fUnderflow > -fPPF)	// limit to 1 frame
				xImmEnd = fDeltaX + fUnderflow + fPPF;
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

