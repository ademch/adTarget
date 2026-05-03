#include "stdafx.h"

#include "../!!adGlobals/glut/glut.h"
#include "../!!adGlobals/adOpenGLUtilities.h"
#include "glfont.h"
#include <assert.h>
#include "VideoSlider.h"
#include "../!!adGlobals/vector_math.h"
#include "gui_element.h"
#include <string>
#include <functional>

extern GLFONT font;


VideoSlider::VideoSlider(std::string strCaption, int px, int py, float _v_min, float _v_max, float& _v_cur, int _height):
	                     _text(strCaption), ref_fValue(_v_cur), m_fVal_min(_v_min), m_fVal_max(_v_max), m_iHeight(_height)
{
	assert(_v_min < _v_max);
	assert(_v_max >= _v_cur);
	assert(_v_min <= _v_cur);

	posx = px;
	posy = py;

	iHPosShift = px;
	iVPosShift = py;

	m_iWidth  = 250;

	m_fSliderX = 0.0;

	bMouseButtonPressed = false;
	bEnabled = true;

	bFocused = false;
	vColor_focused   = Vecc3(0.1, 0.8 ,0.1);
	vColor_defocused = Vecc3(0.1, 0.5, 0.1);

	matrSliderNonInverted = Mat4MakeIdent();

	fPointerPokeHeight = 40;

}

void VideoSlider::SetValue(float _val, float _v_min, float _v_max)
{
	if (_v_min < _v_max)
	{
		m_fVal_min = _v_min;
		m_fVal_max = _v_max;
	}
	//if ((_val >= _v_min) && (_val <= _v_max)) assign the value no matter what
	ref_fValue = _val;
}

int VideoSlider::GetValue()
{
	float t = (m_fSliderX + m_iWidth/2.0) / m_iWidth;

	return m_fVal_max*t;
}

void VideoSlider::DrawTicks(float count, int iStep, float fThickness, float fCaliper, const Matr4& matTransform)
{
	float fTickStep = m_iWidth/count;

	float vNew = matTransform.m[0][0] * fTickStep*iStep;

	if ( vNew < 8) return;

	glLineWidth(fThickness);
	glBegin(GL_LINES);
		float fCurrentVal = 0;
		for (int iTick = 0; iTick <= int(count); iTick+=iStep)
		{
			fCurrentVal = fTickStep*iTick;

			glVertex3f(posx + fCurrentVal,  posy + m_iHeight/2.0 + fCaliper/2.0, 4);
			glVertex3f(posx + fCurrentVal,  posy + m_iHeight/2.0 - fCaliper/2.0, 4);
		}
	glEnd();
}

void VideoSlider::Draw()
{
	GUI_Element::Draw();

	Matr4 matrSliderInverted;
	glGetFloatv(GL_MODELVIEW_MATRIX,  &matrSliderInverted.m[0][0]);

	// clear screen under button
	glColor3f(0, 0, 0);
	glQuad(posx, posy, m_iWidth, m_iHeight, 0);


	//if (bFocused && bEnabled)
	//	glColor3fv(&vColor_focused.X);
	//else
	glColor3fv(&vColor_defocused.X);


	// draw lower border
	glLineWidth(1);
	glLine(posx, posy, posx + m_iWidth, posy, 4);

	// Draw ticks
	{
		unsigned int Count = int(m_fVal_max);

		// hours
		DrawTicks(Count/3600.0f, 1, 2, 20, matrSliderInverted);

		// quarters of hour
		glColor3f(0.86, 0.86, 0.0);
		DrawTicks(Count/60.0f, 15, 2, 16, matrSliderInverted);

		// minutes
		glColor3f(0.2, 0.6, 0.2);
		DrawTicks(Count/60.0f, 1, 2, 10, matrSliderInverted);

		// quarters of seconds
		glColor3f(0.7, 0.7, 0.0);
		DrawTicks(Count/1.0f, 15, 2, 6, matrSliderInverted);

		// seconds
		glColor3f(0.1, 0.6, 0.1);
		DrawTicks(Count/1.0f, 1, 2, 3, matrSliderInverted);
	}

	// draw slider line
	//
	// Slider coordinates are world coordinates in the range (eg -300...300)
	// scaled to a range (eg -200...200) during its capture in the mouse handler.
	// Current modelview matrix brings them back to -300...300, but the value now
	// is not with granularity +1, but with granularity 1/scale
	glLineWidth(1);
	glColor3f(1,0,0);
	glBegin(GL_LINES);
		glVertex3f(m_fSliderX,  posy + m_iHeight + 3, 4);
		glVertex3f(m_fSliderX,  posy + m_iHeight - fPointerPokeHeight, 4);
	glEnd();
}

void VideoSlider::Resize(int iWidth, int iHeight)
{
	m_iWidth = iWidth;
	fPointerPokeHeight = iHeight;
}

bool VideoSlider::Clicked(int button, int state, int x, int y)
{
	GUI_Element::Clicked(button, state, x, y);

	if ((x > posx - 1) && (x < posx + m_iWidth + 1) &&
		(y > posy)     && (y < posy + m_iHeight))
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


bool VideoSlider::Drag(int x, int y)
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


bool VideoSlider::Hover(int x, int y)
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

bool VideoSlider::Wheel(int state,int delta,int x,int y)
{ 
	GUI_Element::Wheel(state, delta, x, y);

	if (!bFocused) return false;

	float fDelta = float(delta)/120.0;

	if (ref_fValue > m_fVal_max )
		ref_fValue = m_fVal_max;

	if (ref_fValue < m_fVal_min )
		ref_fValue = m_fVal_min;

	if (OnClick != NULL) OnClick();

	return true;
}
