#ifndef SLIDER_H
#define SLIDER_H

#include "../!!adGlobals/glut/glut.h"
#include "glfont.h"
#include <assert.h>
#include "../!!adGlobals/vector_math.h"
#include "gui_element.h"
#include <string>
#include <functional>

extern GLFONT font;

typedef float float4[4];

enum SliderType {SL_FLOAT, SL_INT, SL_TIME};

template <SliderType SlType>
class Slider : public GUI_Element
{
public:

	bool bDrawComment;
	bool bDrawMinMaxValues;

	float fTickGranularity;
	float fValueGranularity;

	float fBoxHeightScale;

	std::function<bool()>      OnClick;
	std::function<bool(float)> OnDrawValue;
	std::function<bool()>      OnClickDrag;

	void SetValue(float _val, float _v_min, float _v_max)
	{
		if (_v_min < _v_max)
		{
			m_fVal_min = _v_min;
			m_fVal_max = _v_max;
		}
		//if ((_val >= _v_min) && (_val <= _v_max)) assign the value no matter what
		*ptr_fVal_cur = _val;
	}

	Slider(std::string strCaption, int px, int py, float _v_min, float _v_max, float* _v_cur, float scale):
	       _text(strCaption), size(scale), ptr_fVal_cur(_v_cur),m_fVal_min(_v_min),m_fVal_max(_v_max)
	{
		assert(_v_min < _v_max);
		assert(_v_max >= *_v_cur);
		assert(_v_min <= *_v_cur);

		posx = px;
		posy = py;

		iHPosShift = px;
		iVPosShift = py;

		float _text_width;
		glFontGetLength(strCaption.c_str(), &font, _text_width, m_iHeight, scale);

		m_iWidth    = 250;
		m_iBox_sep  = 20;

		bMouseButtonPressed = false;
		bDrawComment	  = true;
		bDrawMinMaxValues = true;

		fTickGranularity  = 1.0f;
		fValueGranularity = 1.0f;	// changed to match POLYItan branch

		bFocused = false;
		vColor_focused   = Vecc4(0.1,0.8,0.1,0.7);
		vColor_defocused = Vecc4(0.1,0.5,0.1,0.7);

		fBoxHeightScale = 0.7;
	}
	
	void Draw()
	{
		GUI_Element::Draw();

		char text_buf[300];

		if (bFocused && bEnabled)
			glColor4fv(&vColor_focused.X);
		else
			glColor4fv(&vColor_defocused.X);

		glEnable(GL_TEXTURE_2D);

		if (bDrawComment)
		{
			float fValue = *ptr_fVal_cur;
			if (OnDrawValue)
				fValue = OnDrawValue(fValue);

			if (SlType == SL_FLOAT)
				sprintf(text_buf, "%6.2f  %s", fValue, _text.c_str());
			else if (SlType == SL_INT)
				sprintf(text_buf, "%5d  %s", int(fValue), _text.c_str());
			else if (SlType == SL_TIME)
			{
				// Pull out HH:MM:SS
				int Count = int(*ptr_fVal_cur);
				int Hours = Count / 3600;
				if (Hours > 0) Count = Count - (3600 * Hours);
				int Minutes = Count / 60;
				int Seconds = Count % 60;
				sprintf(text_buf, "%02d:%02d:%02d    %s", Hours, Minutes, Seconds, _text.c_str());
			}
			// Render the value
			glFontBegin(&font);
				glFontTextOut(text_buf, posx + m_iWidth + m_iBox_sep, posy, 4, size - 1);
			glFontEnd();
		}
		
		if (bDrawMinMaxValues)
		{
			float fValueMin = m_fVal_min;
			if (OnDrawValue)
				fValueMin = OnDrawValue(fValueMin);

			float fValueMax = m_fVal_max;
			if (OnDrawValue)
				fValueMax = OnDrawValue(fValueMax);

			glFontBegin(&font);
				if (SlType == SL_INT)
				{
					sprintf(text_buf, "%d", int(fValueMin));
					glFontTextOut(text_buf, posx, posy + 16*fBoxHeightScale, 5, size - 2);
					sprintf(text_buf, "%d", int(fValueMax));
					glFontTextOut(text_buf, posx + m_iWidth - 20, posy + 16*fBoxHeightScale, 5, size - 2);
				}
				if (SlType == SL_FLOAT)
				{
					sprintf(text_buf, "%.1f", fValueMin);
					glFontTextOut(text_buf, posx, posy + 16*fBoxHeightScale, 5, size - 2);
					sprintf(text_buf, "%.1f", fValueMax);
					glFontTextOut(text_buf, posx + m_iWidth - 20, posy + 16*fBoxHeightScale, 5, size - 2);
				}
			glFontEnd();
		}

		glDisable(GL_TEXTURE_2D);

		// draw border
		glLineWidth(1);
		glBegin(GL_LINE_LOOP);
			glVertex3f(posx,            posy,                             4);
			glVertex3f(posx + m_iWidth, posy,                             4);
			glVertex3f(posx + m_iWidth, posy + m_iHeight*fBoxHeightScale, 4);
			glVertex3f(posx,            posy + m_iHeight*fBoxHeightScale, 4);
		glEnd();

		// draw "mercury"
		float fullness= (*ptr_fVal_cur - m_fVal_min)/(m_fVal_max - m_fVal_min);

		// limit fullness when ref_fValue is set from outside
		if (fullness > 1.0) fullness = 1.0;
		if (fullness < 0.0) fullness = 0.0;
		
		glBegin(GL_QUADS);
			glVertex3f(posx,                    posy+3,                             4);
			glVertex3f(posx+ fullness*m_iWidth, posy+3,                             4);
			glVertex3f(posx+ fullness*m_iWidth, posy + m_iHeight*fBoxHeightScale-2, 4);
			glVertex3f(posx,                    posy + m_iHeight*fBoxHeightScale-2, 4);
		glEnd();


		// Draw ticks
		glBegin(GL_LINES);
			float iTickCount = (m_fVal_max - m_fVal_min)/fTickGranularity;
			float fCurrentVal = m_fVal_min;
			unsigned int iTickStep = int(ceil(iTickCount/100.0f));	// decimate ticks to have tidy look
			for (unsigned int iTick = 0; iTick <= iTickCount; iTick+= iTickStep)
			{
				fullness = (fCurrentVal - m_fVal_min) / (m_fVal_max - m_fVal_min);
				glVertex3f(posx + fullness*m_iWidth, posy, 4);
				glVertex3f(posx + fullness*m_iWidth, posy - 5, 4);
				fCurrentVal = fCurrentVal + float(iTickStep*fTickGranularity);
			}
		glEnd();
	}

	bool Clicked(int button, int state, int x, int y)
	{
		GUI_Element::Clicked(button, state, x, y);

		if ((state==GLUT_DOWN) && (x<posx + m_iWidth + 1) && (x>posx - 1) && (y<posy+m_iHeight) && (y>posy) && bEnabled)
		{
			float fullness = float(x - posx)/float((posx + m_iWidth) - posx);

			*ptr_fVal_cur = fullness*(m_fVal_max-m_fVal_min) + m_fVal_min;

			// force value granularity
			if (SlType==SL_INT)
			{
				*ptr_fVal_cur = int(round(*ptr_fVal_cur));

				float fRemainder = fmod(*ptr_fVal_cur, fValueGranularity);
				if (abs(fRemainder - fValueGranularity) < 1e-6) fRemainder = 0.0f; // fix float representation aspect 0.1000000001
				*ptr_fVal_cur = (*ptr_fVal_cur) - fRemainder;
			}
			else if ((SlType == SL_FLOAT))// && (fValueGranularity > 0.0f)) removed to match POLYItan branch
			{
				float fRemainder = fmod(*ptr_fVal_cur, fValueGranularity);
				if (abs(fRemainder - fValueGranularity) < 1e-6) fRemainder = 0.0f; // fix float representation aspect 0.1000000001
				*ptr_fVal_cur = (*ptr_fVal_cur) - fRemainder;
			}

			bMouseButtonPressed = true;
			return true;
		}
		
		if (bMouseButtonPressed)
		{
			if (OnClick) OnClick();
			bMouseButtonPressed = false;

			return true;
		}

		return false;
	}

	bool Drag(int x, int y)
	{
		GUI_Element::Drag(x,y);

		if (bMouseButtonPressed && (x<posx + m_iWidth + 1) && (x>posx - 1))
		{
			float fullness= float(x - posx)/float((posx + m_iWidth) - posx);

			*ptr_fVal_cur =fullness*(m_fVal_max-m_fVal_min) + m_fVal_min;
			
			// force value granularity
			if (SlType==SL_INT)
			{
				*ptr_fVal_cur = int(round(*ptr_fVal_cur));

				float fRemainder = fmod(*ptr_fVal_cur, fValueGranularity);
				if (abs(fRemainder - fValueGranularity) < 1e-6) fRemainder = 0.0f; // fix float representation aspect 0.1000000001
				*ptr_fVal_cur = (*ptr_fVal_cur) - fRemainder;
			}
			else if ((SlType == SL_FLOAT))// && (fValueGranularity > 0.0f)) removed to match POLYItan branch
			{
				float fRemainder = fmod(*ptr_fVal_cur, fValueGranularity);
				if (abs(fRemainder - fValueGranularity) < 1e-6) fRemainder = 0.0f; // fix float representation aspect 0.1000000001
				*ptr_fVal_cur = (*ptr_fVal_cur) - fRemainder;
			}

			if (OnClickDrag) OnClickDrag();

			return true;
		}
		return false;
	}

	bool Hover(int x, int y)
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

	bool Wheel(int state,int delta,int x,int y)
	{ 
		GUI_Element::Wheel(state, delta, x, y);

		if (!bFocused) return false;

		float fDelta = float(delta)/120.0;
		
		*ptr_fVal_cur += fValueGranularity*fDelta;

		if (*ptr_fVal_cur > m_fVal_max )
			*ptr_fVal_cur = m_fVal_max;

		if (*ptr_fVal_cur < m_fVal_min )
			*ptr_fVal_cur = m_fVal_min;

		if (OnClick) OnClick();

		return true;
	}

	void SetBoxWidth(int _w)
	{
		m_iWidth = _w;  
	}

	void SetBoxSeparation(int _separation)
	{	
		m_iBox_sep = _separation;	
	}

protected:

	std::string _text;
	float size;

	float m_iHeight;
	int   m_iWidth;
	int   m_iBox_sep;

	float* ptr_fVal_cur;
	float m_fVal_min;
	float m_fVal_max;

	Vec4 vColor_focused;
	Vec4 vColor_defocused;

	bool bFocused;
	bool bMouseButtonPressed;

};


#endif