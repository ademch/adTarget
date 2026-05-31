#include "stdafx.h"
#include "OnOffFlipSwitch.h"
#include "../!!adGlobals/glut/glut.h"
#include "../!!adGlobals/adOpenGLUtilities.h"
#include "glfont.h"
#include "../!!adGlobals/globalToolTip.h"

extern GLFONT font;


OnOffFlipSwitch::OnOffFlipSwitch(std::string strCaption, int px, int py, float scale)
{
	posx = px;
	posy = py;

	iHPosShift = px;
	iVPosShift = py;

	glFontGetLength(strCaption.c_str(), &font, _text_width, m_Height, scale);

	_size = scale;
	_text = strCaption;

	// box width
	m_Width      = 40;
	m_iBox_sep   = 20;

	bON = false;

	bPushButton = false;

	vColor_focused   = Vecc4(0.1, 0.8, 0.1, 0.7);
	vColor_defocused = Vecc4(0.1, 0.5, 0.1, 0.7);
	
	bFocused   = false;
	iGUIpushed = 0;
}



bool OnOffFlipSwitch::Hover(int x, int y)
{
	if ((x < posx + m_Width)  && (x > posx) &&
		(y < posy + m_Height) && (y > posy))
	{
		
		#ifdef _ENABLE_TOOLTIP
			ToolTip::Get()->Schedule(strHint.c_str());
		#endif

		bFocused = bEnabled;
		return true;
	}

	bFocused=false;

	return false;
}


void OnOffFlipSwitch::Draw()
{
	GUI_Element::Draw();

	if (bFocused && bEnabled)
		glColor4fv(&vColor_focused.X);
	else
		glColor4fv(&vColor_defocused.X);

	glEnable(GL_TEXTURE_2D);
		glFontBegin(&font);
			glFontTextOut("I", posx + m_Width + 5, posy, 4, _size);
			glFontTextOut("0", posx - 14,          posy, 4, _size);
			glFontTextOut(_text.c_str(), posx + m_Width + m_iBox_sep, posy, 4, _size);
		glFontEnd();
	glDisable(GL_TEXTURE_2D);

	float fBoxHeightScale = 1.0;

	// draw frame
	glLineWidth(1);
	glWireRectangle(posx, posy, m_Width, m_Height*fBoxHeightScale, 4);


	if (bON)
	{
		glQuad( posx + 3 + m_Width/2 + iGUIpushed,
			    posy + 3 - iGUIpushed,
			    m_Width/2 - 7,
			    m_Height - 6,
				4 );

		glColor3f(0,0.3,0);
		glBegin(GL_LINES);
			glVertex3f(posx + (3 + m_Width/2 + m_Width - 4 )/2.0 + iGUIpushed, posy            + 3 - iGUIpushed, 5);
			glVertex3f(posx + (3 + m_Width/2 + m_Width - 4) /2.0 + iGUIpushed, posy + m_Height - 3 - iGUIpushed, 5);
		glEnd();
	}
	else
	{
		glQuad(	posx + 3 + iGUIpushed, 
			    posy + 3 - iGUIpushed,
			    m_Width/2 - 7,
			    m_Height - 6,
			    4 );

		glColor3f(0, 0.3, 0);
		glBegin(GL_LINES);
			glVertex3f(posx + (3 + m_Width*0.5 - 4) / 2.0 + iGUIpushed, posy + 3 - iGUIpushed, 5);
			glVertex3f(posx + (3 + m_Width*0.5 - 4) / 2.0 + iGUIpushed, posy + m_Height - 3 - iGUIpushed, 5);
		glEnd();
	}
}


bool OnOffFlipSwitch::Clicked(int button, int state, int x, int y)
{
	if (GUI_Element::Clicked(button, state, x, y)) return true;

	if (!bEnabled) return false;

	if ((state == GLUT_DOWN) &&
		(x < posx + m_Width)  && (x > posx) &&
		(y < posy + m_Height) && (y > posy))
	{
		iGUIpushed = 1;

		if (bPushButton)
		{
			if (OnPreClick)
			{
				if (OnPreClick(!bON)) bON = !bON;
			}
			else
				bON = !bON;
		}

		return true;
	}
	else
	{
		if (iGUIpushed)
		{
			// release inside or anywhere for pushbutton behaviour
			if (((x < posx + m_Width) && (x > posx) &&
				 (y < posy + m_Height) && (y > posy)) ||
				 bPushButton )
			{
				// on/off is triggered on mouse up

				if (OnPreClick)
				{
					if (OnPreClick(!bON)) bON = !bON;
				}
				else
					bON = !bON;
				
				iGUIpushed = 0;
				return true;
			}

			iGUIpushed = 0;
		}
	}

	return false;
}


// Called from outside with ability to trigger callback
void OnOffFlipSwitch::SetOnOff(bool _bOn, bool bTriggerAction)
{
	if (bTriggerAction)
	{
		if ((OnPreClick) && (_bOn != bON))
		{
			if (OnPreClick(_bOn)) bON = _bOn;
		}
	}
	else
		bON = _bOn;
}