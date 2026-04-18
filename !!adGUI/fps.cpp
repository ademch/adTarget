#include "stdafx.h"
#include "fps.h"
#include <gl\gl.h>
#include "glfont.h"

extern GLFONT font;


FPS::FPS(int px, int py, float size): _size(size)
{
	posx = px;
	posy = py;
	vColor_focused = Vecc4(0.1f, 0.8f, 0.1f, 0.7f);

	QueryPerformanceFrequency(&ticksPerSecond);
	QueryPerformanceCounter(&T0);
	fFPS_avrg = 1;
	frames = 1;
}

FPS::~FPS()
{
}

void FPS::Draw()
{
	GUI_Element::Draw();

	LARGE_INTEGER T1;
	QueryPerformanceCounter(&T1);
	float elapsed_sec = float(T1.QuadPart - T0.QuadPart) / float(ticksPerSecond.QuadPart);

	frames++;
	if (elapsed_sec > 1.0)
	{
		fFPS_avrg = (fFPS_avrg + float(frames) / elapsed_sec) / 2;
		frames = 0;

		T0 = T1;
	}

	char strBuffer[80];
	sprintf(strBuffer, "Frame rate: %5.1f", fFPS_avrg);

	glColor4fv(&vColor_focused.X);
	glFontBegin(&font);
		glFontTextOut(strBuffer, float(posx), float(posy), 0, 6);
	glFontEnd();
	glDisable(GL_TEXTURE_2D);

}
