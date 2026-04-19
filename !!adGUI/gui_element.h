#ifndef GUI_ELEMENT_H
#define GUI_ELEMENT_H

enum HorizontalAlignment {
	HALIGN_CUSTOM,
	HALIGN_LEFT,
	HALIGN_CENTER,
	HALIGN_RIGHT
};

enum VerticalAlignment {
	VALIGN_CUSTOM,
	VALIGN_TOP,
	VALIGN_CENTER,
	VALIGN_BOTTOM
};

class GUI_Element
{
public:
	GUI_Element()
	{
		posx = 0;
		posy = 0;
		
		hAlign = HALIGN_CUSTOM;
		vAlign = VALIGN_CUSTOM;

		iHPosShift = 0;
		iVPosShift = 0;
	}
	virtual ~GUI_Element() {}

	virtual void Draw() {}

	virtual bool Clicked(int button, int state, int x, int y) { return false; }
	virtual bool Drag(int x, int y) { return false; }
	virtual void Hover(int x, int y) { }
	virtual void Wheel(int state,int delta,int x,int y) { }

	HorizontalAlignment hAlign;
	VerticalAlignment   vAlign;

	int iHPosShift;
	int iVPosShift;

	// 3d scene coordinates within [-w/2; w/2] [h/2; -h/2]
	void Reposition(int px, int py)
    {
	    posx = px;
	    posy = py;
    }

	void GetPosition(int &px, int &py)
	{
		px = posx;
		py = posy;
	}

	void SetAlignment(HorizontalAlignment _hAlign, VerticalAlignment _vAlign)
	{
		hAlign = _hAlign;
		vAlign = _vAlign;
	}

protected:

	// 3d scene coordinates within eg: [-w/2; w/2] [h/2; -h/2]
	// Coords provide shift relatively to alignment mode unless legacy Custom alignment is specified
	int posx;
	int posy;

};

#endif