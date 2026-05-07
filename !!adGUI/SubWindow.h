
#ifndef OPENGLSUBWINDOW_H
#define OPENGLSUBWINDOW_H

#include "../!!adGlobals/vector_math.h"
#include "../!!adGlobals/trackball.h"

#define ROTATION_ALLOWED_FALSE	0x01
#define DRAG_ALLOWED_FALSE		0x02
#define ZOOM_ALLOWED_FALSE		0x04
#define GUI_DECORATION_FALSE	0x08

class OpenGLSubWindow
{
public:
	char m_strCaption[255];

	Vec3 clrBackground;
	Vec3 clrFrame;

	OpenGLSubWindow(int iBottomLeftX, int iBottomLeftY, int iWidth, int iHeight)
	{
		m_iBottomLeftX = iBottomLeftX;
		m_iBottomLeftY = iBottomLeftY;
		m_iWidth  = iWidth;
		m_iHeight = iHeight;

		m_strCaption[0] = 0;
		clrBackground = Vecc3(0.1f, 0.1f, 0.1f);
		clrFrame      = Vecc3(0.9f, 0.9f, 0.9f);

		bMouseSceneRotationInProgress = false;

		vUserSceneTranslation     = Vecc3();
		bMouseSceneDragInProgress = false;

		bSceneRotationAllowed = true;
		bSceneDragAllowed     = true;
		bSceneZoomAllowed     = true;
		bRenderGUIdecoration  = true;

		fUserScale  = 1.f;
		fZoomFactor = 0.75f;	// 0.25, 0.5. 0.75 have perfect ieee representation

		trackball(curquat, 0.0, 0.0, 0.0, 0.0);

		matrUserScale = Mat4MakeIdent();
	}
	virtual ~OpenGLSubWindow() {}

	virtual void Render();

	virtual void MotionFunc(int x,int y);
	virtual bool PassiveMotionFunc(int x, int y) { return false; }
	virtual bool MouseFunc(int button,int state,int x,int y);
	virtual bool KeyboardFunc(unsigned char key, int x, int y) { return false; }
	virtual bool MouseWheelFunc(int state,int delta,int x,int y);
	virtual bool MouseHWheelFunc(int state,int delta,int x,int y)  { return false; }

	// params are in window coordinates
	virtual void Reshape(int iBottomLeftX,int iBottomLeftY, int iWidth,int iHeight)
	{
		m_iBottomLeftX = iBottomLeftX;
		m_iBottomLeftY = iBottomLeftY;

		m_iWidth  = iWidth;
		m_iHeight = iHeight;
	}
	virtual void SetupGraphicsPipeline();
	virtual void SetupGraphicsPipelineWithIdentityModelViewMatrix();

	void ResetView();

	bool bSceneRotationAllowed;
	bool bSceneDragAllowed;
	bool bSceneZoomAllowed;
	bool bRenderGUIdecoration;

	float fZoomFactor;

	void SetFlags(int flags)
	{
		if (flags & ROTATION_ALLOWED_FALSE)
			bSceneRotationAllowed = false;
		if (flags & DRAG_ALLOWED_FALSE)
			bSceneDragAllowed = false;
		if (flags & ZOOM_ALLOWED_FALSE)
			bSceneZoomAllowed = false;
		if (flags & GUI_DECORATION_FALSE)
			bRenderGUIdecoration = false;
	}

protected:
	
	int m_iBottomLeftX;	// in window coords
	int m_iBottomLeftY;	// in window coords

	int m_iWidth;		// in window coords
	int m_iHeight;		// in window coords

	float curquat[4];

	float fUserScale;
	Matr4 matrUserScale;

	Vec3 vUserSceneTranslation;

private:
	float lastquat[4];
	bool bMouseSceneRotationInProgress;			//used for quaternion camera rotation
	int  iBeginRotateX, iBeginRotateY;

	bool bMouseSceneDragInProgress;
	int  iBeginDragX, iBeginDragY;
};


#endif