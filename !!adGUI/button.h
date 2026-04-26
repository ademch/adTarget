#ifndef BUTTON_H
#define BUTTON_H

#include "glfont.h"
#include "gui_element.h"
#include "../!!adGlobals/vector_math.h"
#include "SubWindow.h"
#include <functional>


class Button : public GUI_Element
{
public:
   float m_TextWidth;
   float m_Height;
   int   m_Width;

   bool bFocused;
   bool bEnabled;
   std::string _text;
   
   Vec4 vColor_focused;
   Vec4 vColor_defocused;

   Button(std::string caption, int px, int py, int width, float size);
   ~Button();

   std::function<bool()> OnClick;

   void Draw();
   virtual bool Hover(int x, int y);
   virtual bool Clicked(int button, int state, int x, int y);

protected:
	float m_TextSize;

	int iGUIpushed;

};

#endif