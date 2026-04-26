#include "stdafx.h"
#include "ListBox.h"
#include <gl\gl.h>
#include "../!!adGlobals/glut/glut.h"
#include "../!!adGlobals/adOpenGLUtilities.h"
#include <assert.h>

extern GLFONT font;

ListBox::ListBox(std::string caption, int px, int py, int width, int count, float size)
{
	assert(count >= 3);
	
	posx = px;
	posy = py;

	iHPosShift = px;
	iVPosShift = py;

	glFontGetLength(caption.c_str(), &font, m_TextWidth, m_Height, size);

	m_TextSize = size;
	_text = caption;
	m_Width = width;
	m_Count = count;

	fScrollBarWidth = 1.3f*m_Height;

	bEnabled = true;
	bVisible = true;

	iArrowDownPushed = 0;
	iArrowUpPushed   = 0;

	iSelected  =  0;
	iHovered   = -1;

	iPeepHoleShift = 0;

	bFocusedUp   = false;
	bFocusedDown = false;

	vColor_focused   = Vecc4(0.1, 0.8, 0.1, 0.7);
	vColor_defocused = Vecc4(0.1, 0.5, 0.1, 0.7);

	hCurGrab     = LoadCursorFromFileW(L"grab.cur");
	hCurGrabbing = LoadCursorFromFileW(L"grabbing.cur");
}

ListBox::~ListBox()
{
}


void ListBox::Draw()
{
	GUI_Element::Draw();

	if (!bVisible) return;

	// Imagine iSelectedRowInPeepHole is zero, it becomes -1, -2, -3 when scrolled down
	// When it is within 0 to m_Count-1 it becomes highlighted (visible)
	int iSelectedRowInPeepHole = iSelected - iPeepHoleShift;

	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);

	// clear area under listbox
	glColor3f(0, 0, 0);
	glQuad(posx, posy, m_Width, m_Height*m_Count + 1, 3);

	// draw border
	glColor4fv(&vColor_defocused.X);
	glLineWidth(1);
	glWireRectangle(posx, posy, m_Width, m_Height*m_Count + 1, 5);

	// down arrow
	if (bFocusedDown && bEnabled)
		glColor4fv(&vColor_focused.X);
	else
		glColor4fv(&vColor_defocused.X);

	glLineWidth(2);

	glBegin(GL_LINE_STRIP);
		glVertex3f(posx + m_Width - 1.0*m_Height + iArrowDownPushed, posy + 0.65f*m_Height - iArrowDownPushed, 5);
		glVertex3f(posx + m_Width - 0.7*m_Height + iArrowDownPushed, posy + 0.35f*m_Height - iArrowDownPushed, 5);
		glVertex3f(posx + m_Width - 0.4*m_Height + iArrowDownPushed, posy + 0.65f*m_Height - iArrowDownPushed, 5);
	glEnd();

	// up arrow
	if (bFocusedUp && bEnabled)
		glColor4fv(&vColor_focused.X);
	else
		glColor4fv(&vColor_defocused.X);

	glBegin(GL_LINE_STRIP);
		glVertex3f(posx + m_Width - 1.0*m_Height + iArrowUpPushed, posy + m_Height*m_Count - 0.65f*m_Height - iArrowUpPushed, 5);
		glVertex3f(posx + m_Width - 0.7*m_Height + iArrowUpPushed, posy + m_Height*m_Count - 0.35f*m_Height - iArrowUpPushed, 5);
		glVertex3f(posx + m_Width - 0.4*m_Height + iArrowUpPushed, posy + m_Height*m_Count - 0.65f*m_Height - iArrowUpPushed, 5);
	glEnd();

	// arrow connection line
	glColor4fv(&vColor_defocused.X);
	glBegin(GL_LINE_STRIP);
		glVertex3f(posx + m_Width - 0.7*m_Height, posy                    + m_Height, 5);
		glVertex3f(posx + m_Width - 0.7*m_Height, posy + m_Height*m_Count - m_Height, 5);
	glEnd();

	// Scroller handle
	{
		float fStepSizePerClick = m_Height*(m_Count-3) / (items.size() - m_Count);
		glColor3f(0, 0, 0);
		glQuad(posx + m_Width - 1.2*m_Height, posy + m_Height*m_Count - 2.0*m_Height - fStepSizePerClick*iPeepHoleShift, m_Height, m_Height, 6);

		// border
		glColor4fv(&vColor_defocused.X);
		glLineWidth(1);
		glWireRectangle(posx + m_Width - 1.2*m_Height, posy + m_Height*m_Count - 2.0*m_Height - fStepSizePerClick*iPeepHoleShift, m_Height, m_Height, 7);
	}
	
	// Scroll bar separator
	glLineWidth(1);
	glLine(posx + m_Width - fScrollBarWidth, posy, posx + m_Width - fScrollBarWidth, posy + m_Height*m_Count, 5);

	// selected item
	glColor4fv(&vColor_defocused.X);	// defocused is brighter than grey
	if ((iSelectedRowInPeepHole >= 0) && (iSelectedRowInPeepHole < m_Count))
	{
		glQuad(posx + 2, posy + m_Height*(m_Count - 1) - iSelectedRowInPeepHole*m_Height + 2, m_Width - fScrollBarWidth - 4, m_Height - 4, 3.5);
	}

	// hovered item
	if (iHovered >= 0)
	{
		glLineWidth(1);
		glColor4fv(&vColor_defocused.X);

		glWireRectangle(posx + 1, posy + m_Height*(m_Count-1) - iHovered*m_Height + 1, m_Width - fScrollBarWidth - 2, m_Height - 2, 3.5);
	}

	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);	// not sure why it works
	glEnable(GL_BLEND);

		glFontBegin(&font);

			int iListItem = 0;
			for (auto iter = items.begin() + iPeepHoleShift; (iter != items.end()) && (iListItem < m_Count); iter++, iListItem++)
			{
				if ((iSelectedRowInPeepHole >= 0) && (iSelectedRowInPeepHole < m_Count) && (iSelectedRowInPeepHole == iListItem))
					glColor4f(0, 0, 0, 1);
				else
					glColor4fv(&vColor_defocused.X);

				glFontTextOut(iter->substr(0,20).c_str(), posx + m_Height/2, posy + m_Height*(m_Count-1) - iListItem*m_Height, 4, m_TextSize);
			}

		glFontEnd();

	glDisable(GL_BLEND);

	glDisable(GL_TEXTURE_2D);

}

bool ListBox::Clicked(int button, int state, int x, int y)
{
	if (GUI_Element::Clicked(button, state, x, y)) return true;

	if (!bEnabled) return false;

	if (state == GLUT_DOWN)
	{
		// arrow down
		if ((x > posx + m_Width - m_Height) && (x < posx + m_Width) &&
			(y > posy)                      && (y < posy + m_Height))
		{
			iArrowDownPushed = 1;

			// if number of items is bigger than our last view element
			if (int32_t(items.size()) > m_Count + iPeepHoleShift) iPeepHoleShift++;

			return true;
		}

		// arrow up
		if ((x > posx + m_Width - m_Height)       && (x < posx + m_Width) &&
			(y > posy + m_Height * (m_Count - 1)) && (y < posy + m_Height * m_Count))
		{
			iArrowUpPushed = 1;

			if (iPeepHoleShift > 0) iPeepHoleShift--;

			return true;
		}

		// list
		if ((x > posx) && (x < posx + m_Width - fScrollBarWidth) &&
			(y > posy) && (y < posy + m_Height*m_Count))
		{
			int iYLocal = posy + m_Height*m_Count - y;
			unsigned int iRow = iYLocal / m_Height;

			if (iRow < items.size())
			{
				glutSetCursor(GLUT_CURSOR_NONE);
				SetCursor(hCurGrabbing);

				if (OnSelect != NULL) OnSelect(iRow);

				iSelected = iRow + iPeepHoleShift;

				return true;
			}
		}
	}

	iArrowDownPushed = 0;
	iArrowUpPushed   = 0;

	return false;
}


bool ListBox::Hover(int x, int y)
{
	GUI_Element::Hover(x, y);

	if ((x > posx) && (x < posx + m_Width) &&
		(y > posy) && (y < posy + m_Height*m_Count))
	{
		if (bEnabled)
		{
			if ((x > posx + m_Width - m_Height) && (x < posx + m_Width) &&
				(y > posy) && (y < posy + m_Height))
				bFocusedDown = true;
			else
				bFocusedDown = false;

			if ((x > posx + m_Width - m_Height)   && (x < posx + m_Width) &&
				(y > posy + m_Height*(m_Count-1)) && (y < posy + m_Height*m_Count))
				bFocusedUp = true;
			else
				bFocusedUp = false;

			if ((x > posx) && (x < posx + m_Width - fScrollBarWidth) &&
				(y > posy) && (y < posy + m_Height*m_Count))
			{
				int iYLocal = posy + m_Height*m_Count - y;
				unsigned int iRow = iYLocal / m_Height;

				if (iRow < items.size())
				{
					glutSetCursor(GLUT_CURSOR_NONE);
					SetCursor(hCurGrab);

					iHovered = iRow;
				}
				else
				{
					glutSetCursor(GLUT_CURSOR_INHERIT);	// lower than available items
					iHovered = -1;
				}
			}
			else
			{
				glutSetCursor(GLUT_CURSOR_INHERIT);	// in horizontal slider 
				iHovered = -1;
			}
		}

		return true;
	}

	iHovered = -1;
	bFocusedDown = false;
	bFocusedUp   = false;

	return false;
}


std::string ListBox::GetSelected()
{
	std::string result("");

	if (items.size() > 0)
		result = items[iSelected];

	return result;
}

bool ListBox::SetSelected(std::string _str)
{
	int iListItem = 0;
	std::vector<std::string>::iterator iter;
	for (iter = items.begin(); iter != items.end(); iter++, iListItem++)
	{
		if (iter->compare(_str) == 0)
		{
			iSelected = iListItem;
			return true;
		}
	}

	return false;
}

void ListBox::Wheel(int state, int delta, int x, int y)
{
	GUI_Element::Wheel(state, delta, x, y);

	if (!bEnabled) return;

	float fDelta = float(delta) / 120.0;

	if (fDelta < 0.0)
	{
		// if number of items is bigger than our last view element
		if (int32_t(items.size()) > m_Count + iPeepHoleShift) iPeepHoleShift++;
	}
	else
	{
		if (iPeepHoleShift > 0) iPeepHoleShift--;
	}

}