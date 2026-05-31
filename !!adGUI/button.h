#ifndef BUTTON_H
#define BUTTON_H

#include "gui_element.h"
#include "../!!adGlobals/vector_math.h"
#include "../!!adGlobals/TextureDescriptor.h"
#include <functional>


class Button : public GUI_Element
{
public:
   bool bFocused;

   std::string _text;
   std::string strHint;

   Vec4 vColor_focused;
   Vec4 vColor_defocused;

   Button(std::string caption, int px, int py, int width, float size);
   virtual ~Button() {};

   std::function<bool()> OnClick;

   void Draw() override;
   virtual bool Hover(int x, int y);
   virtual bool Clicked(int button, int state, int x, int y);

protected:
	float m_TextWidth;
	float m_Height;
	int   m_Width;

	float m_TextSize;

	int iGUIpushed;

private:

};



class ButtonImage : public Button
{
public:

	ButtonImage(std::string caption, int px, int py, int width);
	~ButtonImage();

	void LoadImg(const char* filename);
	void LoadImgDownState(const char* filename);

	TextureDescriptor* texDescr;
	TextureDescriptor* texDescrDownState;

	void Draw() override;
	bool Clicked(int button, int state, int x, int y) override;

	bool bDrawFrame;

	bool bDownState;

protected:

};


class PushButtonImage : public Button
{
public:

	PushButtonImage(std::string caption, int px, int py, int width);
	~PushButtonImage();

	void LoadImg(const char* filename);

	TextureDescriptor* texDescr;

	void Draw() override;
	bool Clicked(int button, int state, int x, int y) override;

	bool bPushed;

protected:

};


#endif