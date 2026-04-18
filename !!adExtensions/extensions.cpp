#include "stdafx.h"
#include <gl/gl.h>
#include <gl/glext.h>
#include <GL/wglext.h>

PFNGLDRAWBUFFERSPROC              glDrawBuffersARB = NULL;
PFNGLDELETEOBJECTARBPROC          glDeleteObjectARB  = NULL;
//PFNGLGETHANDLEARBPROC			  glGetHandleARB  = NULL;
//PFNGLDETACHOBJECTARBPROC		  glDetachObjectARB  = NULL;
PFNGLCREATESHADEROBJECTARBPROC	  glCreateShaderObjectARB  = NULL;
PFNGLSHADERSOURCEARBPROC		  glShaderSourceARB  = NULL;
PFNGLCOMPILESHADERARBPROC		  glCompileShaderARB  = NULL;
PFNGLCREATEPROGRAMOBJECTARBPROC	  glCreateProgramObjectARB  = NULL;
PFNGLATTACHOBJECTARBPROC		  glAttachObjectARB  = NULL;
PFNGLLINKPROGRAMARBPROC			  glLinkProgramARB  = NULL;
PFNGLUSEPROGRAMOBJECTARBPROC	  glUseProgramObjectARB  = NULL;
PFNGLVALIDATEPROGRAMARBPROC		  glValidateProgramARB  = NULL;
PFNGLUNIFORM1FARBPROC			  glUniform1fARB  = NULL;
//PFNGLUNIFORM2FARBPROC			  glUniform2fARB  = NULL;
PFNGLUNIFORM3FARBPROC			  glUniform3fARB  = NULL;
PFNGLUNIFORM4FARBPROC			  glUniform4fARB  = NULL;
PFNGLUNIFORM1IARBPROC			  glUniform1iARB  = NULL;
PFNGLUNIFORM2IARBPROC			  glUniform2iARB  = NULL;
//PFNGLUNIFORM3IARBPROC			  glUniform3iARB  = NULL;
//PFNGLUNIFORM4IARBPROC			  glUniform4iARB  = NULL;
//PFNGLUNIFORM1FVARBPROC			  glUniform1fvARB  = NULL;
PFNGLUNIFORM2FVARBPROC			  glUniform2fvARB  = NULL;
//PFNGLUNIFORM3FVARBPROC			  glUniform3fvARB  = NULL;
//PFNGLUNIFORM4FVARBPROC			  glUniform4fvARB  = NULL;
//PFNGLUNIFORM1IVARBPROC			  glUniform1ivARB  = NULL;
//PFNGLUNIFORM2IVARBPROC			  glUniform2ivARB  = NULL;
//PFNGLUNIFORM3IVARBPROC			  glUniform3ivARB  = NULL;
//PFNGLUNIFORM4IVARBPROC			  glUniform4ivARB  = NULL;
//PFNGLUNIFORMMATRIX2FVARBPROC	  glUniformMatrix2fvARB  = NULL;
PFNGLUNIFORMMATRIX3FVARBPROC	  glUniformMatrix3fvARB  = NULL;
//PFNGLUNIFORMMATRIX4FVARBPROC	  glUniformMatrix4fvARB  = NULL;
//PFNGLGETOBJECTPARAMETERFVARBPROC  glGetObjectParameterfvARB  = NULL;
PFNGLGETOBJECTPARAMETERIVARBPROC  glGetObjectParameterivARB  = NULL;
PFNGLGETINFOLOGARBPROC			  glGetInfoLogARB  = NULL;
//PFNGLGETATTACHEDOBJECTSARBPROC	  glGetAttachedObjectsARB  = NULL;
PFNGLGETUNIFORMLOCATIONARBPROC	  glGetUniformLocationARB  = NULL;
//PFNGLGETACTIVEUNIFORMARBPROC	  glGetActiveUniformARB  = NULL;
//PFNGLGETUNIFORMFVARBPROC		  glGetUniformfvARB  = NULL;
//PFNGLGETUNIFORMIVARBPROC		  glGetUniformivARB  = NULL;
//PFNGLGETSHADERSOURCEARBPROC	  glGetShaderSourceARB  = NULL;

PFNGLGETATTRIBLOCATIONARBPROC    glGetAttribLocationARB  = NULL;
PFNGLVERTEXATTRIB2FVARBPROC		 glVertexAttrib2fvARB = NULL;

// ARB_vertex_buffer_object
PFNGLBINDBUFFERARBPROC           glBindBufferARB           = NULL;
PFNGLDELETEBUFFERSARBPROC        glDeleteBuffersARB        = NULL;
PFNGLGENBUFFERSARBPROC           glGenBuffersARB           = NULL;
PFNGLISBUFFERARBPROC             glIsBufferARB             = NULL;
PFNGLBUFFERDATAARBPROC           glBufferDataARB           = NULL;
PFNGLBUFFERSUBDATAARBPROC        glBufferSubDataARB        = NULL;
PFNGLGETBUFFERSUBDATAARBPROC     glGetBufferSubDataARB     = NULL;
PFNGLMAPBUFFERARBPROC            glMapBufferARB            = NULL;
PFNGLUNMAPBUFFERARBPROC          glUnmapBufferARB          = NULL;
PFNGLGETBUFFERPARAMETERIVARBPROC glGetBufferParameterivARB = NULL;
PFNGLGETBUFFERPOINTERVARBPROC    glGetBufferPointervARB    = NULL;

PFNGLMULTITEXCOORD2FARBPROC		glMultiTexCoord2fARB       = NULL;
PFNGLMULTITEXCOORD2FVARBPROC	glMultiTexCoord2fvARB      = NULL;
PFNGLACTIVETEXTUREARBPROC		glActiveTextureARB         = NULL;
PFNGLCLIENTACTIVETEXTUREARBPROC	glClientActiveTextureARB   = NULL;

// PBuffer extension
PFNWGLCREATEPBUFFERARBPROC					   wglCreatePbufferARB        = NULL;
PFNWGLGETPBUFFERDCARBPROC					   wglGetPbufferDCARB         = NULL;
PFNWGLRELEASEPBUFFERDCARBPROC				   wglReleasePbufferDCARB     = NULL;
PFNWGLDESTROYPBUFFERARBPROC					   wglDestroyPbufferARB       = NULL;
PFNWGLQUERYPBUFFERARBPROC					   wglQueryPbufferARB         = NULL;

PFNWGLGETPIXELFORMATATTRIBIVARBPROC			   wglGetPixelFormatAttribivARB = NULL;
PFNWGLGETPIXELFORMATATTRIBFVARBPROC			   wglGetPixelFormatAttribfvARB = NULL;
PFNWGLCHOOSEPIXELFORMATARBPROC				   wglChoosePixelFormatARB      = NULL;
											   
PFNGLPOINTPARAMETERFARBPROC					   glPointParameterfARB   = NULL;
PFNGLPOINTPARAMETERFVARBPROC				   glPointParameterfvARB  = NULL;

// FrameBufferObject
PFNGLISRENDERBUFFERPROC                        glIsRenderbufferEXT;
PFNGLBINDRENDERBUFFERPROC                      glBindRenderbufferEXT;
PFNGLDELETERENDERBUFFERSPROC                   glDeleteRenderbuffersEXT;
PFNGLGENRENDERBUFFERSPROC                      glGenRenderbuffersEXT;
PFNGLRENDERBUFFERSTORAGEPROC                   glRenderbufferStorageEXT;
PFNGLGETRENDERBUFFERPARAMETERIVPROC            glGetRenderbufferParameterivEXT;
PFNGLISFRAMEBUFFERPROC                         glIsFramebufferEXT;
PFNGLBINDFRAMEBUFFERPROC                       glBindFramebufferEXT;
PFNGLDELETEFRAMEBUFFERSPROC                    glDeleteFramebuffersEXT;
PFNGLGENFRAMEBUFFERSPROC                       glGenFramebuffersEXT;
PFNGLCHECKFRAMEBUFFERSTATUSPROC                glCheckFramebufferStatusEXT;
PFNGLFRAMEBUFFERTEXTURE1DPROC                  glFramebufferTexture1DEXT;
PFNGLFRAMEBUFFERTEXTURE2DPROC                  glFramebufferTexture2DEXT;
PFNGLFRAMEBUFFERTEXTURE3DPROC                  glFramebufferTexture3DEXT;
PFNGLFRAMEBUFFERRENDERBUFFERPROC               glFramebufferRenderbufferEXT;
PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC   glGetFramebufferAttachmentParameterivEXT;
PFNGLGENERATEMIPMAPPROC                        glGenerateMipmapEXT;

//UniformBufferObject
PFNGLGENBUFFERSPROC                            glGenBuffers;
PFNGLGETUNIFORMBLOCKINDEXPROC                  glGetUniformBlockIndex;


bool isExtensionSupported(const char *extension)
{
	const GLubyte *extensions = NULL;
	const GLubyte *start;
	GLubyte *where, *terminator;

	/* Extension names should not have spaces. */
	where = (GLubyte *) strchr(extension, ' ');

	if (where || *extension == '\0') return 0;

	extensions=glGetString(GL_EXTENSIONS);

	/* It takes a bit of care to be fool-proof about parsing the
	OpenGL extensions string. Don't be fooled by sub-strings, etc. */

	start=extensions;
	for (;;) 
	{
		where=(GLubyte *) strstr((const char *) start, extension);
		if (!where) break;
		terminator = where + strlen(extension);
		if (where == start || *(where - 1) == ' ')
			if (*terminator == ' ' || *terminator == '\0') return TRUE;
		start = terminator;
	}

	return FALSE;
}


bool isExtensionSupportedWGL(const char *extension)
{
	const GLubyte *wgl_extensions = NULL;
	const GLubyte *start;
	GLubyte *where, *terminator;

	/* Extension names should not have spaces. */
	where = (GLubyte *) strchr(extension, ' ');

	if (where || *extension == '\0') return 0;

	//  extensions=glGetString(GL_EXTENSIONS);
#ifdef _WIN32
	{
		PFNWGLGETEXTENSIONSSTRINGARBPROC wglGetExtensionsStringARB = 0;
		wglGetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)wglGetProcAddress("wglGetExtensionsStringARB");
		if(wglGetExtensionsStringARB)
			wgl_extensions = (const GLubyte *) wglGetExtensionsStringARB(wglGetCurrentDC());
	}
#endif

	/* It takes a bit of care to be fool-proof about parsing the
	OpenGL extensions string. Don't be fooled by sub-strings, etc. */

	start=wgl_extensions;
	for (;;) 
	{
		where=(GLubyte *) strstr((const char *) start, extension);
		if (!where) break;
		terminator = where + strlen(extension);
		if (where == start || *(where - 1) == ' ')
			if (*terminator == ' ' || *terminator == '\0') return TRUE;
		start = terminator;
	}

	return FALSE;
}

void linkExtensions()
{
	glDrawBuffersARB =          (PFNGLDRAWBUFFERSPROC)              wglGetProcAddress("glDrawBuffersARB");
	glGetInfoLogARB =           (PFNGLGETINFOLOGARBPROC)            wglGetProcAddress("glGetInfoLogARB");
	glGetObjectParameterivARB = (PFNGLGETOBJECTPARAMETERIVARBPROC)  wglGetProcAddress("glGetObjectParameterivARB");
	glDeleteObjectARB=          (PFNGLDELETEOBJECTARBPROC)          wglGetProcAddress("glDeleteObjectARB");
	glCompileShaderARB=         (PFNGLCOMPILESHADERARBPROC)         wglGetProcAddress("glCompileShaderARB");
	glShaderSourceARB=          (PFNGLSHADERSOURCEARBPROC)          wglGetProcAddress("glShaderSourceARB");
	glCreateShaderObjectARB=    (PFNGLCREATESHADEROBJECTARBPROC)    wglGetProcAddress("glCreateShaderObjectARB");
	glUseProgramObjectARB=      (PFNGLUSEPROGRAMOBJECTARBPROC)      wglGetProcAddress("glUseProgramObjectARB");
	glValidateProgramARB=       (PFNGLVALIDATEPROGRAMARBPROC)       wglGetProcAddress("glValidateProgramARB");
	glCreateProgramObjectARB=   (PFNGLCREATEPROGRAMOBJECTARBPROC)   wglGetProcAddress("glCreateProgramObjectARB");
	glLinkProgramARB=           (PFNGLLINKPROGRAMARBPROC)           wglGetProcAddress("glLinkProgramARB");
	glAttachObjectARB=          (PFNGLATTACHOBJECTARBPROC)          wglGetProcAddress("glAttachObjectARB");
	glUniform1iARB=             (PFNGLUNIFORM1IARBPROC)             wglGetProcAddress("glUniform1iARB");
	glUniform2iARB=             (PFNGLUNIFORM2IARBPROC)             wglGetProcAddress("glUniform2iARB");
	glGetUniformLocationARB=    (PFNGLGETUNIFORMLOCATIONARBPROC)    wglGetProcAddress("glGetUniformLocationARB");
	glUniform1fARB=             (PFNGLUNIFORM1FARBPROC)             wglGetProcAddress("glUniform1fARB");
	glUniform3fARB=             (PFNGLUNIFORM3FARBPROC)             wglGetProcAddress("glUniform3fARB");
	glUniform2fvARB=            (PFNGLUNIFORM2FVARBPROC)            wglGetProcAddress("glUniform2fvARB");
	glUniformMatrix3fvARB=      (PFNGLUNIFORMMATRIX3FVARBPROC)      wglGetProcAddress("glUniformMatrix3fvARB");
	glUniform4fARB=             (PFNGLUNIFORM4FARBPROC)             wglGetProcAddress("glUniform4fARB");

	glGetAttribLocationARB=		(PFNGLGETATTRIBLOCATIONARBPROC)     wglGetProcAddress("glGetAttribLocationARB");
	glVertexAttrib2fvARB=       (PFNGLVERTEXATTRIB2FVARBPROC)       wglGetProcAddress("glVertexAttrib2fvARB");

	glMultiTexCoord2fARB	 = (PFNGLMULTITEXCOORD2FARBPROC)		wglGetProcAddress("glMultiTexCoord2fARB");
	glMultiTexCoord2fvARB	 = (PFNGLMULTITEXCOORD2FVARBPROC)	    wglGetProcAddress("glMultiTexCoord2fvARB");
	glActiveTextureARB		 = (PFNGLACTIVETEXTUREARBPROC)		    wglGetProcAddress("glActiveTextureARB");
	glClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC)	wglGetProcAddress("glClientActiveTextureARB");

	glBindBufferARB           = (PFNGLBINDBUFFERARBPROC)            wglGetProcAddress("glBindBufferARB");
	glDeleteBuffersARB        = (PFNGLDELETEBUFFERSARBPROC)         wglGetProcAddress("glDeleteBuffersARB");
	glGenBuffersARB           = (PFNGLGENBUFFERSARBPROC)            wglGetProcAddress("glGenBuffersARB");
	glIsBufferARB             = (PFNGLISBUFFERARBPROC)              wglGetProcAddress("glIsBufferARB");
	glBufferDataARB           = (PFNGLBUFFERDATAARBPROC)            wglGetProcAddress("glBufferDataARB");
	glBufferSubDataARB        = (PFNGLBUFFERSUBDATAARBPROC)         wglGetProcAddress("glBufferSubDataARB");
	glGetBufferSubDataARB     = (PFNGLGETBUFFERSUBDATAARBPROC)      wglGetProcAddress("glGetBufferSubDataARB");
	glMapBufferARB            = (PFNGLMAPBUFFERARBPROC)             wglGetProcAddress("glMapBufferARB");
	glUnmapBufferARB          = (PFNGLUNMAPBUFFERARBPROC)           wglGetProcAddress("glUnmapBufferARB");
	glGetBufferParameterivARB = (PFNGLGETBUFFERPARAMETERIVARBPROC)  wglGetProcAddress("glGetBufferParameterivARB");
	glGetBufferPointervARB    = (PFNGLGETBUFFERPOINTERVARBPROC)     wglGetProcAddress("glGetBufferPointervARB");

	// PBuffer
	wglCreatePbufferARB       = (PFNWGLCREATEPBUFFERARBPROC)		wglGetProcAddress("wglCreatePbufferARB");
	wglGetPbufferDCARB        = (PFNWGLGETPBUFFERDCARBPROC)			wglGetProcAddress("wglGetPbufferDCARB");
	wglReleasePbufferDCARB    = (PFNWGLRELEASEPBUFFERDCARBPROC)		wglGetProcAddress("wglReleasePbufferDCARB");
	wglDestroyPbufferARB      = (PFNWGLDESTROYPBUFFERARBPROC)		wglGetProcAddress("wglDestroyPbufferARB");
	wglQueryPbufferARB        = (PFNWGLQUERYPBUFFERARBPROC)			wglGetProcAddress("wglQueryPbufferARB");

	wglGetPixelFormatAttribivARB = (PFNWGLGETPIXELFORMATATTRIBIVARBPROC) wglGetProcAddress("wglGetPixelFormatAttribivARB");
	wglGetPixelFormatAttribfvARB = (PFNWGLGETPIXELFORMATATTRIBFVARBPROC) wglGetProcAddress("wglGetPixelFormatAttribfvARB");
	wglChoosePixelFormatARB      = (PFNWGLCHOOSEPIXELFORMATARBPROC)      wglGetProcAddress("wglChoosePixelFormatARB");

	glPointParameterfARB  =     (PFNGLPOINTPARAMETERFARBPROC)		wglGetProcAddress("glPointParameterfARB");
	glPointParameterfvARB =     (PFNGLPOINTPARAMETERFVARBPROC)		wglGetProcAddress("glPointParameterfvARB");


	// FrameBufferObject
	glIsRenderbufferEXT             = (PFNGLISRENDERBUFFERPROC)             wglGetProcAddress("glIsRenderbufferEXT");                     
	glBindRenderbufferEXT			= (PFNGLBINDRENDERBUFFERPROC)			wglGetProcAddress("glBindRenderbufferEXT");					
	glDeleteRenderbuffersEXT		= (PFNGLDELETERENDERBUFFERSPROC)		wglGetProcAddress("glDeleteRenderbuffersEXT");
	glGenRenderbuffersEXT			= (PFNGLGENRENDERBUFFERSPROC)			wglGetProcAddress("glGenRenderbuffersEXT");
	glRenderbufferStorageEXT		= (PFNGLRENDERBUFFERSTORAGEPROC)		wglGetProcAddress("glRenderbufferStorageEXT");
	glGetRenderbufferParameterivEXT	= (PFNGLGETRENDERBUFFERPARAMETERIVPROC)	wglGetProcAddress("glGetRenderbufferParameterivEXT");
	glIsFramebufferEXT				= (PFNGLISFRAMEBUFFERPROC)				wglGetProcAddress("glIsFramebufferEXT");
	glBindFramebufferEXT			= (PFNGLBINDFRAMEBUFFERPROC)			wglGetProcAddress("glBindFramebufferEXT");
	glDeleteFramebuffersEXT			= (PFNGLDELETEFRAMEBUFFERSPROC)			wglGetProcAddress("glDeleteFramebuffersEXT");
	glGenFramebuffersEXT			= (PFNGLGENFRAMEBUFFERSPROC)			wglGetProcAddress("glGenFramebuffersEXT");
	glCheckFramebufferStatusEXT		= (PFNGLCHECKFRAMEBUFFERSTATUSPROC)		wglGetProcAddress("glCheckFramebufferStatusEXT");
	glFramebufferTexture1DEXT		= (PFNGLFRAMEBUFFERTEXTURE1DPROC)		wglGetProcAddress("glFramebufferTexture1DEXT");
	glFramebufferTexture2DEXT		= (PFNGLFRAMEBUFFERTEXTURE2DPROC)		wglGetProcAddress("glFramebufferTexture2DEXT");
	glFramebufferTexture3DEXT		= (PFNGLFRAMEBUFFERTEXTURE3DPROC)		wglGetProcAddress("glFramebufferTexture3DEXT");
	glFramebufferRenderbufferEXT	= (PFNGLFRAMEBUFFERRENDERBUFFERPROC)	wglGetProcAddress("glFramebufferRenderbufferEXT");
	glGetFramebufferAttachmentParameterivEXT= (PFNGLGETFRAMEBUFFERATTACHMENTPARAMETERIVPROC)   wglGetProcAddress("glGetFramebufferAttachmentParameterivEXT");
	glGenerateMipmapEXT				= (PFNGLGENERATEMIPMAPPROC)	            wglGetProcAddress("glGenerateMipmapEXT");
}


bool CheckExtensions()
{
	printf("\n");

	printf("Is GL_ARB_shading_language_100 supported...");
	if(isExtensionSupported("GL_ARB_shading_language_100"))
		printf("yes\n");
	else
	{
		printf("no\n");		//by default FALSE
		return false;
	}

	printf("Is GL_ARB_shader_objects supported...");
	if(isExtensionSupported("GL_ARB_shader_objects"))
		printf("yes\n");
	else
	{	printf("no\n");		//by default FALSE
		return false;
	}

	printf("Is GL_ARB_vertex_shader supported...");
	if(isExtensionSupported("GL_ARB_vertex_shader"))
		printf("yes\n");
	else
	{	printf("no\n");		//by default FALSE
		return false;
	}

	printf("Is GL_ARB_fragment_shader supported...");
	if(isExtensionSupported("GL_ARB_fragment_shader"))
		printf("yes\n");
	else
	{	printf("no\n");		//by default FALSE
		return false;
	}

	printf("Is GL_ARB_multitexture supported...");
	if(isExtensionSupported("GL_ARB_multitexture"))
		printf("yes\n");
	else
	{	printf("no\n");		//by default FALSE
		return false;
	}

	printf("Is GL_ARB_vertex_buffer_object supported...");
	if(isExtensionSupported("GL_ARB_vertex_buffer_object"))
		printf("yes\n");
	else
	{	printf("no\n");		//by default FALSE
		return false;
	}

	printf("Is GL_ARB_point_parameters supported...");
	if(isExtensionSupported("GL_ARB_point_parameters"))
		printf("yes\n");
	else
	{	printf("no\n");		//by default FALSE
		return false;
	}

	printf("Is GL_ARB_point_sprite supported...");
	if(isExtensionSupported("GL_ARB_point_sprite"))
		printf("yes\n");
	else
	{	printf("no\n");		//by default FALSE
		return false;
	}

	printf("Is GL_EXT_texture_filter_anisotropic supported...");
	if(isExtensionSupported("GL_EXT_texture_filter_anisotropic"))
	{	printf("yes\n");
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, 2.f);
	}
	else
	{	printf("no\n");		//by default FALSE
		return false;
	}

	printf("Is GL_EXT_framebuffer_object...");
	if (isExtensionSupported("GL_EXT_framebuffer_object"))
		printf("yes\n");
	else
	{
		printf("no\n");		//by default FALSE
		return false;
	}

	printf("Is GL_ARB_texture_non_power_of_two...");
	if (isExtensionSupported("GL_ARB_texture_non_power_of_two"))
		printf("yes\n");
	else
	{
		printf("no\n");		//by default FALSE
		return false;
	}

	printf("Is GL_ARB_draw_buffers...");
	if (isExtensionSupported("GL_ARB_draw_buffers"))
		printf("yes\n");
	else
	{
		printf("no\n");		//by default FALSE
		return false;
	}

return true;
}
