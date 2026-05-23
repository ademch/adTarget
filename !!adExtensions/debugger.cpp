#include "stdafx.h"
#include "debugger.h"
#include "extensions.h"


const char* _decodeSourceToString(GLenum source)
{
	switch (source)
	{
	case GL_DEBUG_SOURCE_API_ARB:			 return "API";
	case GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB:  return "Window System";
	case GL_DEBUG_SOURCE_SHADER_COMPILER_ARB:return "Shader Compiler";
	case GL_DEBUG_SOURCE_THIRD_PARTY_ARB:	 return "Third Party";
	case GL_DEBUG_SOURCE_APPLICATION_ARB:	 return "Application";
	case GL_DEBUG_SOURCE_OTHER_ARB:			 return "Other";
	default:								 return "Unknown";
	}
}

const char* _decodeTypeToString(GLenum type)
{
	switch (type)
	{
	case GL_DEBUG_TYPE_ERROR_ARB:				 return "Error";
	case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB:  return "Deprecated Behavior";
	case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB:	 return "Undefined Behavior";
	case GL_DEBUG_TYPE_PORTABILITY_ARB:			 return "Portability";
	case GL_DEBUG_TYPE_PERFORMANCE_ARB:			 return "Performance";
	case GL_DEBUG_TYPE_OTHER_ARB:				 return "Other";
	default:									 return "Unknown";
	}
}

const char* _decodeSeverityToString(GLenum severity)
{
	switch (severity)
	{
	case GL_DEBUG_SEVERITY_HIGH_ARB:   return "HIGH";
	case GL_DEBUG_SEVERITY_MEDIUM_ARB: return "MEDIUM";
	case GL_DEBUG_SEVERITY_LOW_ARB:    return "LOW";
	default:                           return "Unknown";
	}
}

void APIENTRY GLDebugCallback( GLenum source,
							   GLenum type,
							   GLuint id,
							   GLenum severity,
							   GLsizei length,
							   const GLchar* message,
							   const void* userParam)
{
	printf(	"[OpenGL]\n"
			"  Source   : %s\n"
			"  Type     : %s\n"
			"  Severity : %s\n"
			"  ID       : %u\n"
			"  Message  : %s\n\n",

			_decodeSourceToString(source), _decodeTypeToString(type), _decodeSeverityToString(severity), id, message );
}

void glDebugMessageControlDisableDeprecatedMessages()
{
	glDebugMessageControl(GL_DONT_CARE,								// origin
						  GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB,	// type
						  GL_DONT_CARE,								// severity
						  0, NULL,									// identifiers
						  GL_FALSE);								// disable
}