
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

	virtual ~OpenGLSubWindowWithGUI()
	{
		for (auto* iterElement : liGUI_Elements)
			delete iterElement;
	}

	virtual void Reshape(int iBottomLeftX, int iBottomLeftY, int iWidth, int iHeight);

	virtual bool PassiveMotionFunc(int x, int y);
	virtual bool MouseFunc(int button, int state, int x, int y);
	virtual void MotionFunc(int x, int y);
	virtual bool MouseWheelFunc(int state, int delta, int x, int y);
	virtual bool MouseHWheelFunc(int state, int delta, int x, int y);
	virtual	void Render() final;	// calls Draw to be able to process component before its GUI

	virtual	void Draw() {};

	// put here and not in OpenGLSubWindow to support legacy code that creates OpenGLSubWindow without this info
	float fBottomLeftXperc;
	float fBottomLeftYperc;
	float fWidthPerc;
	float fHeightPerc;

protected:

	std::vector<GUI_Element*> liGUI_Elements;

	virtual void PopulateGUI() = 0;
	virtual void RenderGUI();
	void ReshapeGUI(int iWidth, int iHeight);

	virtual bool PassiveMotionFuncGUI(int x, int y);
	virtual bool MouseFuncGUI(int button, int state, int x, int y);
	virtual void MotionFuncGUI(int x, int y);
	virtual bool MouseWheelFuncGUI(int state, int delta, int x, int y);
	virtual bool MouseHWheelFuncGUI(int state, int delta, int x, int y);


private:

};


#endif