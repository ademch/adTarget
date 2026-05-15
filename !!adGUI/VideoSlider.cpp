#include "stdafx.h"
#include "../!!adGlobals/glut/glut.h"
#include "../!!adGlobals/adOpenGLUtilities.h"
#include "VideoSlider.h"
#include "../!!adGlobals/vector_math.h"
#include "VideoPositionMediator.h"


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
}

void VideoSlider::SetPos0_1(float _val)
{
	m_fPos01 = _val;
}

void VideoSlider::SetPosInit(float _val, float _v_max)
{
	m_iValMax  = _v_max;
	m_fPos01     = _val;
}

bool VideoSlider::DrawTicks(float count, int iStep, float fThickness, float fCaliper, const Matr4& matTransform)
{
	if (count < 1) return false;
	
	float fTickStep = m_iWidth/count;

	float vNew = matTransform.m[0][0] * fTickStep*iStep;

	if ( vNew < 7) return false;

	glLineWidth(fThickness);
	glBegin(GL_LINES);
		float fCurrentVal = 0;
		for (int iTick = 0; iTick <= int(count); iTick+=iStep)
		{
			fCurrentVal = fTickStep*iTick;

			glVertex3f(posx + fCurrentVal,  posy + fCaliper/2.0*(m_iHeight/20.0f), 4);
			glVertex3f(posx + fCurrentVal,  posy - fCaliper/2.0*(m_iHeight/20.0f), 4);
		}
	glEnd();

	return true;
}

void VideoSlider::Draw()
{
	GUI_Element::Draw();

	Matr4 matrSliderInverted;
	glGetFloatv(GL_MODELVIEW_MATRIX,  &matrSliderInverted.m[0][0]);

	// clear screen under control
	glColor3f(0, 0, 0);
	glQuad(posx, posy, m_iWidth, m_iHeight, 0);


	float fHighlight = 1.0f;
	if (bFocused && bEnabled)
		fHighlight = 1.15f;

	glColor3f(0.1*fHighlight, 0.5*fHighlight, 0.1*fHighlight);

	// Draw ticks
	{
		unsigned int Count = int(m_iValMax);

		bool bDrawn;

		// hours
		DrawTicks(Count/3600.0f, 1, 3, 18, matrSliderInverted);

		// minutes
		glColor3f(0.2*fHighlight, 0.6*fHighlight, 0.2*fHighlight);
		bDrawn = DrawTicks(Count/60.0f, 1, 2, 10, matrSliderInverted);

		if (!bDrawn)
		{
			// quarters of hour
			glColor3f(0.5*fHighlight, 0.7*fHighlight, 0.4*fHighlight);
			DrawTicks(Count/60.0f, 15, 2, 14, matrSliderInverted);
		}


		// seconds
		glColor3f(0.1*fHighlight, 0.6*fHighlight, 0.1*fHighlight);
		bDrawn = DrawTicks(Count/1.0f, 1, 2, 3, matrSliderInverted);

		if (!bDrawn)
		{
			// quarters of seconds
			glColor3f(0.7*fHighlight, 0.7*fHighlight, 0.0*fHighlight);
			DrawTicks(Count/1.0f, 15, 2, 5, matrSliderInverted);
		}

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

}

void VideoSlider::Resize(int iWidth, int iHeight)
{
	m_iWidth  = iWidth;
	m_iHeight = iHeight;
}

bool VideoSlider::Clicked(int button, int state, int x, int y)
{
	GUI_Element::Clicked(button, state, x, y);

	// Transform-scale input world coords of a slider using matrix from HorScrollBar.
	// The matrix is specially organized in a way the world coordinates (-300...300)
	// become a peephole with N times greater precision than 1/(600)
	Vec3 ptPeep = matrSliderNonInverted*Vecc3(x,y);

	// special case, component is symmetric along zero
	if ((ptPeep.X > posx)               && (ptPeep.X < posx + m_iWidth) &&
		(ptPeep.Y > posy - m_iHeight/2) && (ptPeep.Y < posy + m_iHeight/2))
	{
		if (!bEnabled) return false;

		if (state==GLUT_DOWN)
		{
			m_fPos01 = (ptPeep.X + m_iWidth/2.0) / m_iWidth;

			if (OnChange != NULL) OnChange(m_fPos01);

			bMouseButtonPressed = true;
			return true;
		}
	}

	bMouseButtonPressed = false;

	return false;
}


bool VideoSlider::Drag(int x, int y)
{
	GUI_Element::Drag(x, y);

	// Transform-scale input world coords of a slider using matrix from HorScrollBar.
	// The matrix is specially organized in a way the world coordinates (-300...300)
	// become a peephole with N times greater precision than 1/(600)
	Vec3 ptPeep = matrSliderNonInverted*Vecc3(x,y);

	if (bMouseButtonPressed && (ptPeep.X > posx) && (ptPeep.X < posx + m_iWidth) )
	{
		m_fPos01 = (ptPeep.X + m_iWidth/2.0) / m_iWidth;

		if (OnChange != NULL) OnChange(m_fPos01);

		return true;
	}
	return false;
}


bool VideoSlider::Hover(int x, int y)
{
	GUI_Element::Hover(x, y);

	// Transform-scale input world coords of a slider using matrix from HorScrollBar.
	// The matrix is specially organized in a way the world coordinates (-300...300)
	// become a peephole with N times greater precision than 1/(600)
	Vec3 ptPeep = matrSliderNonInverted*Vecc3(x,y);

	// special case, component is symmetric along zero
	if ((ptPeep.X > posx)               && (ptPeep.X < posx + m_iWidth) &&
		(ptPeep.Y > posy - m_iHeight/2) && (ptPeep.Y < posy + m_iHeight/2))
	{
		bFocused = bEnabled;
		return true;
	}

	bFocused = false;

	return false;
}
