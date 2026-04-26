#ifndef COMBO_BOX_H
#define COMBO_BOX_H

#include "glfont.h"
#include "gui_element.h"
#include "../!!adGlobals/vector_math.h"
#include <vector>
#include <functional>

class ComboBox: public GUI_Element
{
public:
   float m_Height;
   int   m_Width;
   std::string _text;

   bool bFocused;
   bool bEnabled;
   bool bVisible;

   unsigned int iSelected;
   
   Vec4 vColor_focused;
   Vec4 vColor_defocused;

   ComboBox(std::string caption, int px, int py, int width, float size);
   ~ComboBox();

   std::function<bool()>    OnClick;
   std::function<bool(int)> OnSelect;

   void Draw();
   virtual bool Hover(int x, int y);
   virtual bool Clicked(int button, int state, int x, int y);

   bool SetSelected(std::string);
   std::string GetSelected();

   std::vector<std::string> items;

protected:
	float m_TextSize;

	bool  bExpanded;
	int   iGUIpushed;

	float m_TextWidth;

private:
	int iListPixelShift;
	unsigned int iHovered;
};

#endif