#ifndef TRACKCLIP_H
#define TRACKCLIP_H

#include "gui_elementResizable.h"
#include "../!!adGlobals/vector_math.h"
#include "../!!adGlobals/TextureDescriptor.h"
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

class FFMS_Video;
class OpenGLSubWindowWithGUI;


class TrackClip : public GUI_ElementResizable
{
public:

	TrackClip(int _id, int px, int py, int _width, int _height);
	~TrackClip();

	static TrackClip* GetClip(OpenGLSubWindowWithGUI* wnd);

	void Resize(int iWidth, int iHeight) override;

	void SetAttr(int _iStartPosFrame, int _iLengthFrames)
	{
		m_iStartPos10msTicks = _iStartPosFrame;
		m_iLength10msTicks   = _iLengthFrames;
	}

	void Draw() override;

	bool Clicked(int button, int state, int x, int y) override;
	bool Drag(int x, int y) override;
	bool Hover(int x, int y) override;

	static int iSelected;
	int		 id;

	// the track this clip belongs to
	int      iTrack;

	ClipType mediaType;
	int      m_iLength10msTicks;
	int      m_iStartPos10msTicks;

	OpenGLSubWindowWithGUI*    windowTool;
	TextureDescriptor*         textureIcon;
	FFMS_Video*                video;

	std::function<bool()>      OnClick;
	std::function<bool(float)> OnDrawValue;
	std::function<bool()>      OnClickDrag;

	std::function<void(OpenGLSubWindowWithGUI*)> OnClipChange;

	static std::vector<TrackClip*> liClips;

	float ClipsFitsIntoGapOnTrackImmediate(int iTrack);

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

	float fPPF;

	float FindClipOnTrackBeforeFrame_TailPx(int iTrack, int iFrame);
	float FindClipOnTrackAfterFrame_HeadPx(int iTrack, int iFrame);

};


#endif