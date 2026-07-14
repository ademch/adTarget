#ifndef TRACKCLIP_H
#define TRACKCLIP_H

#include "gui_elementResizable.h"
#include "../!!adGlobals/TextureDescriptor.h"
#include "../!!adGlobals/VideoAnimatedParams.h"
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
	static TrackClip* GetSelectedClip();
	static double     GetSelectedClipLocalTimeS();
	static void       RemoveSelectedClip();

	void Resize(int iWidth, int iHeight) override;

	void Draw() override;

	bool Clicked(int button, int state, int x, int y) override;
	bool Drag(int x, int y) override;
	bool Hover(int x, int y) override;

	static int iSelected;
	int		 id;

	static int iAutoIncrID;

	// the track this clip belongs to
	int      iTrack;

	std::vector<ParamKeyframeTRSTransform>* liKeyframesTRS;
	std::vector<ParamKeyframePolyline2D>*   liKeyframesMorphDst;

	void SetAttr(int _iStartPos10msTicks, int _iLength10msTicks)
	{
		m_iStartPos10msUnits = _iStartPos10msTicks;
		m_iLength10msUnits   = _iLength10msTicks;
	}

	ClipType mediaType;
	int      m_iLength10msUnits;
	int      m_iStartPos10msUnits;

	OpenGLSubWindowWithGUI*    windowTool;
	TextureDescriptor*         textureIcon;
	FFMS_Video*                video;

	std::function<bool()>      OnClick;
	std::function<bool()>      OnClickDrag;

	std::function<void(OpenGLSubWindowWithGUI*)> OnClipChange;

	static std::vector<TrackClip*> liClips;

	float ClipsFitsIntoGapOnTrackImmediate(int iTrack);

	void RegisterTRSparam(std::vector<ParamKeyframeTRSTransform>* _liKeyframesTRS);
	void RegisterMorphDSTparam(std::vector<ParamKeyframePolyline2D>* _liKeyframesMorphDst);

protected:

	int m_iWidth;
	int m_iHeight;

	Vec3 vColor_focused;
	Vec3 vColor_defocused;

	bool bFocused;

	float xImmTransl;
	float xImmBeg;
	float xImmEnd;

	StateClipDrag_enum stateClip;

	// pixels per 10ms unit
	float fPPU;

	float FindClipOnTrackBefore_TailPx(int iTrack, int iPos);
	float FindClipOnTrackAfter_HeadPx(int iTrack, int iPos);

private:

	float iBeginDragX, iBeginDragY;
	bool bDragInProgress;
};


#endif