#include "stdafx.h"
#include "glfont.h"
#include "../!!adGlobals/glut/glut.h"
#include "../!!adGlobals/adOpenGLUtilities.h"
#include "VideoSlider.h"
#include "VideoPositionMediator.h"
#include "TrackClip.h"

const int iSnapPxToKeyframe = 4;

extern GLFONT font;

VideoSlider::VideoSlider(int px, int py, int _height):
			             m_iValMax(4000), m_fPos01(0.0),
	                     m_iHeight(_height), m_iWidth(250)
{
	posx = px;
	posy = py;

	iHPosShift = px;
	iVPosShift = py;

	bMouseButtonPressed = false;

	bFocused = false;
	vColor_focused   = Vecc3(0.1, 0.8 ,0.1);
	vColor_defocused = Vecc3(0.1, 0.5, 0.1);

	hCurLeftRight     = LoadCursorFromFileW(L"Cursors\\aero_arrowLeftRight.cur");
}

void VideoSlider::SetPos0_1(double _val)
{
	m_fPos01 = _val;
}

void VideoSlider::SetPosInit(double _val0_1, int _v_max)
{
	m_fPos01  = _val0_1;
	m_iValMax = _v_max;
}

bool VideoSlider::DrawTicks(float count, float fThickness, float fCaliper, const Matr4& matTransform)
{
	if (count < 1) return false;
	
	float fTickStep = m_iWidth/count;

	float vNew = matTransform.m[0][0] * fTickStep;

	if ( vNew < 6) return false;

	glLineWidth(fThickness);
	glBegin(GL_LINES);
		float fCurrentVal = 0;
		for (int iTick = 0; iTick <= int(count); iTick++)
		{
			fCurrentVal = fTickStep*iTick;

			glVertex3f(posx + fCurrentVal,  posy + fCaliper/2.0*(m_iHeight/20.0f), -8);
			glVertex3f(posx + fCurrentVal,  posy - fCaliper/2.0*(m_iHeight/20.0f), -8);
		}
	glEnd();

	return true;
}


bool VideoSlider::DrawMinuteDigits(float count, const Matr4& matTransform)
{
	float fTickStep = m_iWidth/count;

	float vNew = matTransform.m[0][0] * fTickStep;

	if ( vNew < 50) return false;

	float fCurrentVal = 0;
	for (int iTick = 0; iTick <= int(count); iTick+=1)
	{
		fCurrentVal = fTickStep*iTick;

		float fTransX = (posx + fCurrentVal)*matTransform.m[0][0] + matTransform.m[3][0];

		glColor3f(0, 0, 0);
		glQuad(fTransX-6, posy - 7, 30, 14, -5);

		int iMinutes = iTick % 60;
		int iHours   = iTick / 60;
		static char strMinutes[8];
		if (iHours < 1)
			sprintf(strMinutes, "%dm", iMinutes);
		else
			sprintf(strMinutes, "%dh%dm", iHours, iMinutes);

		glColor3f(0.1, 0.8, 0.1);

		glFontBegin(&font);
		glFontTextOut(strMinutes, fTransX-3, posy-6, -4, /*size*/ 5);
			glFontEnd();
		glDisable(GL_TEXTURE_2D);
	}

	return true;
}

void VideoSlider::Draw()
{
	GUI_Element::Draw();

	Matr4 matrSliderInverted;
	glGetFloatv(GL_MODELVIEW_MATRIX,  &matrSliderInverted.m[0][0]);

	// clear screen under control
	glColor3f(0, 0, 0);
	glQuad(posx, posy-m_iHeight/2, m_iWidth, m_iHeight, -10);


	float fHighlight = 1.0f;
	if (bFocused && bEnabled)
		fHighlight = 1.25f;

	glColor3f(0.1*fHighlight, 0.5*fHighlight, 0.1*fHighlight);

	// Draw ticks
	{
		// hours
		DrawTicks(m_iValMax/(100.0*3600.0f), 2, 18, matrSliderInverted);

		// minutes
		glColor3f(0.2*fHighlight, 0.6*fHighlight, 0.2*fHighlight);
		DrawTicks(m_iValMax/(100.0*60.0f), 1, 8, matrSliderInverted);

		// seconds
		glColor3f(0.1*fHighlight, 0.6*fHighlight, 0.1*fHighlight);
		DrawTicks(m_iValMax/100.0, 1, 3, matrSliderInverted);

		// 10ms ticks
		glColor3f(0.7*fHighlight, 0.7*fHighlight, 0.0*fHighlight);
		DrawTicks(m_iValMax/1.0f, 1, 1, matrSliderInverted);

	}

	// draw slider line
	//
	// Slider coordinates are world coordinates in the range (eg -300...300)
	// scaled to a range (eg -200...200) during its capture in the mouse handler.
	// Current modelview matrix brings them back to -300...300, but the value now
	// is not with granularity +1, but with granularity 1/scale
	{
		float m_fSliderX = m_fPos01*m_iWidth;

		glColor3f(1,0,0);

		// triangle headshape simulated with lines
		glLineWidth(1);
		glLine( posx + m_fSliderX, posy,
				posx + m_fSliderX, posy - m_iHeight/2,     5);
		glLineWidth(3);
		glLine (posx + m_fSliderX, posy + m_iHeight/2 - 4,
				posx + m_fSliderX, posy - m_iHeight/2 + 3, 5);
		glLineWidth(5);
		glLine( posx + m_fSliderX, posy + m_iHeight/2 - 4,
				posx + m_fSliderX, posy - m_iHeight/2 + 5, 5);
	}

	glLoadIdentity();
	DrawMinuteDigits(m_iValMax/(100.0*60.0f), matrSliderInverted);

}

void VideoSlider::Resize(int iWidth, int iHeight)
{
	m_iWidth  = iWidth;
	m_iHeight = iHeight;
}

bool VideoSlider::Clicked(int button, int state, int x, int y)
{
	GUI_Element::Clicked(button, state, x, y);

	// special case, component is symmetric along zero
	if ((x >= posx)               && (x <= posx + m_iWidth) &&
		(y >  posy - m_iHeight/2) && (y <  posy + m_iHeight/2))
	{
		// Transform-scale input world coords of a slider using matrix from HorScrollBar.
		// The matrix is specially organized in a way the world coordinates (-300...300)
		// become a peephole with N times greater precision than 1/(600)
		Vec3 ptPeep = matrSliderNonInverted*Vecc3(x,y);

		if ((ptPeep.X > posx) && (ptPeep.X < posx + m_iWidth))
		{
			if (!bEnabled) return false;

			if (state==GLUT_DOWN)
			{
				//          float      int
				m_fPos01 = (ptPeep.X + m_iWidth/2) / m_iWidth;

				if (OnChange != NULL) OnChange(m_fPos01);

				bMouseButtonPressed = true;
				return true;
			}
		}
	}

	bMouseButtonPressed = false;

	return false;
}


bool VideoSlider::Drag(int x, int y)
{
	GUI_Element::Drag(x, y);

	if ((x >= posx) && (x <= posx + m_iWidth))
	{
		// Transform-scale input world coords of a slider using matrix from HorScrollBar.
		// The matrix is specially organized in a way the world coordinates (-300...300)
		// become a peephole with N times greater precision than 1/(600)
		Vec3 ptPeep = matrSliderNonInverted*Vecc3(x,y);

		if ((bMouseButtonPressed) && (ptPeep.X >= posx) && (ptPeep.X <= posx + m_iWidth) )
		{
			//          float      int
			m_fPos01 = (ptPeep.X + m_iWidth/2) / m_iWidth;

			TryToSnapPositionToKeyframe(m_fPos01);

			if (OnChange != NULL) OnChange(m_fPos01);

			return true;
		}
	}
	return false;
}


bool VideoSlider::Hover(int x, int y)
{
	GUI_Element::Hover(x, y);

	// special case, component is symmetric along zero
	if ((x >= posx)               && (x <= posx + m_iWidth) &&
		(y >  posy - m_iHeight/2) && (y <  posy + m_iHeight/2))
	{
		bFocused = bEnabled;
		if (bEnabled)
		{
			if (GetCursor() != hCurLeftRight)
			{
				glutSetCursor(200);
				SetCursor(hCurLeftRight);
			}

			return true;
		}

		return false;
	}

	bFocused = false;

	return false;
}


void VideoSlider::TryToSnapPositionToKeyframe(double& fPos0_1)
{
	TrackClip* clip = TrackClip::GetSelectedClip();
	if (!clip) return;

	PositionMediator* mediator = PositionMediator::Get();

	// Pixels per 10ms
	float fPPU = float(m_iWidth)/mediator->Duration10msUnits();

	for (const auto& item : clip->animatedTRSTransformPtr->liKeys)
	{
		if (abs(item.time*100.0 + clip->m_iStartPos10msUnits - fPos0_1*mediator->Duration10msUnits()) <
		   (iSnapPxToKeyframe*matrSliderNonInverted.m[0][0])/fPPU )
		{
			fPos0_1 = (item.time*100.0 + clip->m_iStartPos10msUnits)/mediator->Duration10msUnits();
			return;
		}
	}

	for (const auto& item : clip->animatedPolylineDstPtr->liKeys)
	{
		if (abs(item.time*100.0 + clip->m_iStartPos10msUnits - fPos0_1*mediator->Duration10msUnits()) <
		   (iSnapPxToKeyframe*matrSliderNonInverted.m[0][0])/fPPU )
		{
			fPos0_1 = (item.time*100.0 + clip->m_iStartPos10msUnits)/mediator->Duration10msUnits();
			return;
		}
	}

	if (abs(clip->m_iStartPos10msUnits - fPos0_1*mediator->Duration10msUnits()) <
	   (iSnapPxToKeyframe*matrSliderNonInverted.m[0][0])/fPPU )
	{
		fPos0_1 = double(clip->m_iStartPos10msUnits)/mediator->Duration10msUnits();
		return;
	}

	if (abs(clip->m_iStartPos10msUnits + clip->m_iLength10msUnits - fPos0_1*mediator->Duration10msUnits()) <
		(iSnapPxToKeyframe*matrSliderNonInverted.m[0][0])/fPPU )
	{
		fPos0_1 = double(clip->m_iStartPos10msUnits + clip->m_iLength10msUnits)/mediator->Duration10msUnits();
		return;
	}

}