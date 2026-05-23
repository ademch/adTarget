#include <gl/GL.h>
#include <gl/glext.h>

void APIENTRY GLDebugCallback( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

void glDebugMessageControlDisableDeprecatedMessages();