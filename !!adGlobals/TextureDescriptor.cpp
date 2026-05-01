#include "stdafx.h"
#include "TextureDescriptor.h"
#include <gl\gl.h>


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


TextureDescriptor* LoadTextureWinAPI(const char* filename)
{
	// Load image
 	HBITMAP hbm = (HBITMAP)LoadImageA( NULL, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION );

	if (!hbm) return 0;

	BITMAP bm;
	GetObject(hbm, sizeof(bm), &bm);

		// bm.bmBits is valid because of LR_CREATEDIBSECTION
		void* bits = bm.bmBits;

		if (!bits)
		{
			DeleteObject(hbm);
			return 0;
		}

		GLuint iTexture;
		glGenTextures(1, &iTexture);
		glBindTexture(GL_TEXTURE_2D, iTexture);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
					 bm.bmWidth, bm.bmHeight, 0,
					 GL_BGR_EXT, GL_UNSIGNED_BYTE, bits);

	DeleteObject(hbm);

	return new TextureDescriptor(iTexture, bm.bmWidth, bm.bmHeight);
}