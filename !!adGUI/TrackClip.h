#ifndef TRACKCLIP_H
#define TRACKCLIP_H

#include "../!!adGlobals/vector_math.h"
#include "gui_elementResizable.h"
#include "../!!adGlobals/TextureDescriptor.h"
#include "SubWindowWithGUI.h"
#include <functional>

enum ClipType  {
	CLIP_IMAGE,
	CLIP_VIDEO
};

//class ClipMedia
//{
//public:
//	ClipType type;
//
//	void* window;
//};

enum StateClipDrag_enum {
	STATE_CLIP_IDLE,
	STATE_CLIP_DRAG_POS,
	STATE_CLIP_DRAG_HEAD,
	STATE_CLIP_DRAG_TAIL
};

class TrackClip : public GUI_ElementResizable
{
public:

	TrackClip(int _id, int px, int py, int _width, int _height);

	void Resize(int iWidth, int iHeight) override;

	void SetAttr(int _iStart, int _iLength)
	{
		m_iStartPos = _iStart;
		m_iLength   = _iLength;
	}

	void Draw() override;

	bool Clicked(int button, int state, int x, int y) override;
	bool Drag(int x, int y) override;
	bool Hover(int x, int y) override;

	static int iSelected;
	int		 id;

	// the track this ckip belongs to
	int      iTrack;

	ClipType type;
	int      m_iLength;
	int      m_iStartPos;

	OpenGLSubWindowWithGUI*    windowTool;
	TextureDescriptor*         extern_textureIcon;

	std::function<bool()>      OnClick;
	std::function<bool(float)> OnDrawValue;
	std::function<bool()>      OnClickDrag;

	std::function<void(OpenGLSubWindowWithGUI*)> OnClipChange;

protected:

	int m_iWidth;
	int m_iHeight;

	Vec3 vColor_focused;
	Vec3 vColor_defocused;

	bool bFocused;

	float iBeginDragX, iBeginDragY;

	float xImmTransl;
	float xImmBeg;
	float xImmEnd;

	StateClipDrag_enum stateClip;
};


#endif