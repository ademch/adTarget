
#ifndef OPENGLSUBWINDOW_WITH_GUI_H
#define OPENGLSUBWINDOW_WITH_GUI_H

#include "SubWindow.h"
#include "gui_element.h"

#include <vector>

class OpenGLSubWindowWithGUI : public OpenGLSubWindow
{
public:

	OpenGLSubWindowWithGUI(int iBottomLeftX, int iBottomLeftY, int iWidth, int iHeight);

	OpenGLSubWindowWithGUI(int iParentWidth, int iParentHeight,
		                   float _fBottomLeftXperc, float _fBottomLeftYperc,
		                   float _fWidthPerc, float _fHeightPerc);

	virtual ~OpenGLSubWindowWithGUI() {}

	void Reshape(int iBottomLeftX, int iBottomLeftY, int iWidth, int iHeight);

	float fBottomLeftXperc;
	float fBottomLeftYperc;
	float fWidthPerc;
	float fHeightPerc;

protected:

	std::vector<GUI_Element*> liGUI_Elements;

	void RenderGUI();
	void PassiveMotionFuncGUI(int x, int y);
	bool MouseFuncGUI(int button, int state, int x, int y);
	void MotionFuncGUI(int x, int y);
	void ReshapeGUI(int iWidth, int iHeight);
	void MouseWheelFuncGUI(int state, int delta, int x, int y);


private:

};


#endif