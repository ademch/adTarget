#ifndef LABEL_H
#define LABEL_H

#include "glfont.h"
#include "gui_element.h"
#include "../!!adGlobals/vector_math.h"


class Label: public GUI_Element
{
public:

   Label(std::string caption, int px, int py, float size);
   ~Label();

   std::string _text;

   Vec4 vColor;

   void UpdateCaption(std::string caption);

   void Draw() override;

protected:

   float _size;

};


#endif