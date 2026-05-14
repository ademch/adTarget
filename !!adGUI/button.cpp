#include "stdafx.h"
#include "button.h"
#include <gl/gl.h>
#include "../!!adGlobals/glut/glut.h"
#include "../!!adGlobals/adOpenGLUtilities.h"
#include "../!!adGlobals/globalToolTip.h"

extern GLFONT font;

Button::Button(std::string caption, int px, int py, int width, float size)
{
	posx = px;
	posy = py;

	iHPosShift = px;
	iVPosShift = py;

	glFontGetLength(caption.c_str(),&font, m_TextWidth, m_Height, size);

	m_TextSize = size;
	_text      = caption;
	m_Width    = width;

	strHint = "";

	bFocused = false;
	iGUIpushed = 0;

	vColor_focused   = Vecc4(0.1, 0.8, 0.1, 0.7);
	vColor_defocused = Vecc4(0.1, 0.5, 0.1, 0.7);

}

Button::~Button()
{
}

void Button::Draw()
{
	GUI_Element::Draw();

	// clear screen under button
	glColor3f(0, 0, 0);
	glQuad(posx + iGUIpushed, posy - iGUIpushed, m_Width, m_Height, 3);

	if (bFocused && bEnabled)
		glColor4fv(&vColor_focused.X);
	else
		glColor4fv(&vColor_defocused.X);

	glFontBegin(&font);
		glFontTextOut(_text.c_str(), posx+ (m_Width-m_TextWidth)/2.0 + iGUIpushed,posy - iGUIpushed, 4,m_TextSize);
	glFontEnd();

	glDisable(GL_TEXTURE_2D);

	// draw frame
	glLineWidth(1);
	glWireRectangle(posx + iGUIpushed, posy - iGUIpushed, m_Width, m_Height, 4.5);
}


bool Button::Hover(int x, int y)
{
	GUI_Element::Hover(x, y);

	if ((x > posx)   && (x < posx + m_Width) && 
		(y > posy-1) && (y < posy + m_Height))
	{
		bFocused = bEnabled;

		#ifdef _ENABLE_TOOLTIP
			ToolTip::Get()->Schedule(strHint.c_str());
		#endif

		return true;
	}

	bFocused   = false;
	iGUIpushed = 0;

	return false;
}


bool Button::Clicked(int button, int state, int x, int y)
{
	if (GUI_Element::Clicked(button, state, x, y)) return true;

	if (!bEnabled) return false;

	if ((x < posx + m_Width)  && (x > posx) &&
		(y < posy + m_Height) && (y > posy-1))
	{
		if (button == GLUT_LEFT_BUTTON)
		{
			if (state == GLUT_DOWN)
			{
				iGUIpushed = 1;
				return true;
			}
			// OnClick is going to happen only if mouse is released within the button boundaries
			else
			{
				if (iGUIpushed)
				{
					if (OnClick != NULL) OnClick();

					iGUIpushed = 0;
					return true;
				}
			}
		}
	}

	iGUIpushed = 0;
	return false;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////

ButtonImage::ButtonImage(std::string caption, int px, int py, int width) :
	         Button(caption, px, py, width, 0)
{
	m_Height = width;
	texDescr = NULL;
	texDescrDownState = NULL;
	bDrawFrame = true;
	bDownState = false;
}

ButtonImage::~ButtonImage()
{
	delete texDescr;
	delete texDescrDownState;
}



void ButtonImage::Draw()
{
	// clear screen under button
	glColor3f(0, 0, 0);
	glQuad(posx + iGUIpushed, posy - iGUIpushed, m_Width, m_Height, 0);

	if (bFocused && bEnabled)
		glColor4fv(&vColor_focused.X);
	else
		glColor4fv(&vColor_defocused.X);

	if (texDescr && !bDownState)
	{
		glBindTexture(GL_TEXTURE_2D, texDescr->m_uiTextureID);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		glEnable(GL_TEXTURE_2D);
			glTexturedQuad(posx + iGUIpushed, posy - iGUIpushed, m_Width, m_Height, 6);
		glDisable(GL_TEXTURE_2D);
	}
	else if (texDescrDownState && bDownState)
	{
		glBindTexture(GL_TEXTURE_2D, texDescrDownState->m_uiTextureID);
		glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

		glEnable(GL_TEXTURE_2D);
		glTexturedQuad(posx + iGUIpushed, posy - iGUIpushed, m_Width, m_Height, 6);
		glDisable(GL_TEXTURE_2D);
	}

	if (bDrawFrame)
	{
		glLineWidth(1);
		glWireRectangle(posx + iGUIpushed, posy - iGUIpushed, m_Width, m_Height, 7);
	}

}

void ButtonImage::LoadImg(const char* filename)
{
	texDescr = LoadTextureWinAPI(filename);
}

void ButtonImage::LoadImgDownState(const char* filename)
{
	texDescrDownState = LoadTextureWinAPI(filename);
}


bool ButtonImage::Clicked(int button, int state, int x, int y)
{
	if (!bEnabled) return false;

	if ((x < posx + m_Width)  && (x > posx) &&
		(y < posy + m_Height) && (y > posy))
	{
		if (button == GLUT_LEFT_BUTTON)
		{
			if (state == GLUT_DOWN)
			{
				iGUIpushed = 1;
				return true;
			}
			// OnClick is going to happen only if mouse is released within the button boundaries
			else
			{
				if (iGUIpushed)
				{
					if (OnClick != NULL) OnClick();

					if (texDescrDownState) bDownState = !bDownState;

					iGUIpushed = 0;
					return true;
				}
			}
		}
	}

	iGUIpushed = 0;
	return false;
}
