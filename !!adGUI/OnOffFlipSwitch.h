#ifndef ONOFFFLIPSWITCH_H
#define ONOFFFLIPSWITCH_H

#include "gui_element.h"
#include "../!!adGlobals/vector_math.h"
#include <functional>

class OnOffFlipSwitch : public GUI_Element
{
public:
   bool bON;
   bool bPushButton;

   std::string _text;
   std::string strHint;
   
   Vec4 vColor_focused;
   Vec4 vColor_defocused;

   OnOffFlipSwitch(std::string strCaption, int px, int py, float size);
   ~OnOffFlipSwitch() {}

   void Draw();
   bool Hover(int x, int y) override;
   bool Clicked(int button, int state, int x, int y) override;

   std::function<bool(bool bON_Request)> OnPreClick;

   void SetOnOff(bool _bOn, bool bCallCallback);

protected:
	float _text_width;
	float m_Height;

	float _size;
	int   m_Width;
	int   m_iBox_sep;

	bool  bFocused;
	int   iGUIpushed;
};

#endif
