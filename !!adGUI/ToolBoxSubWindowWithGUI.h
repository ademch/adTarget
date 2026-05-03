#ifndef TOOLBOXSUBWINDOWWITHGUI_H
#define TOOLBOXSUBWINDOWWITHGUI_H

#include "SubWindowWithGUI.h"
#include <vector>
#include "ToolBase.h"


class ToolBoxSubWindowWithGUI : public OpenGLSubWindowWithGUI
{
public:
	ToolBoxSubWindowWithGUI(int iParentWidth, int iParentHeight,
							float fBottomLeftXperc, float fBottomLeftYperc,
							float fWidthPerc, float fHeightPerc);
	~ToolBoxSubWindowWithGUI();

	virtual	void Draw();

	void Reshape(int iBottomLeftX, int iBottomLeftY, int iWidth, int iHeight);

	virtual bool PassiveMotionFunc(int x, int y);
	virtual bool MouseFunc(int button, int state, int x, int y);
	virtual void MotionFunc(int x, int y);
	virtual bool KeyboardFunc(unsigned char key, int x, int y);

protected:

	std::vector<ToolBase*> liTools;

	virtual void RenderGUI() override;
	virtual bool PassiveMotionFuncGUI(int x, int y) override;
	virtual bool MouseFuncGUI(int button, int state, int x, int y) override;
	virtual void MotionFuncGUI(int x, int y) override;

private:

};

#endif