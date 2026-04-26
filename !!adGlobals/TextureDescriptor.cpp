#include "stdafx.h"
#include "TextureDescriptor.h"
#include <gl\gl.h>
#include "../!!adGlobals/glut/glut.h"

TextureDescriptor* LoadTexture(int iWidth, int iHeight, unsigned char *data)
{
	unsigned int iTexture;
	glGenTextures(1, &iTexture);
	glBindTexture(GL_TEXTURE_2D, iTexture);

	// set the texture wrapping/filtering options (on the currently bound texture object)
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,     GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,	  GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	//           targ         mml  int frmt                brdr inc frmt inc data type   inc data
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iWidth, iHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	return new TextureDescriptor(iTexture, iWidth, iHeight);
}