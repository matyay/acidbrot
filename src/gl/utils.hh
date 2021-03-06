#ifndef GL_UTILS_HH
#define GL_UTILS_HH

#include "gl.hh"

#include <string>

namespace GL {

// ============================================================================

#define GL_STRINGIFY1(x) GL_STRINGIFY2(x)
#define GL_STRINGIFY2(x) #x

/// Checks if a call to a GL function succeeded. If not throws an exception
#define GL_CHECK(call)  \
{   \
    GL::checkPreCall(__FILE__ ":" GL_STRINGIFY1(__LINE__) " " GL_STRINGIFY1(call));  \
    call;    \
    GL::checkPostCall(__FILE__ ":" GL_STRINGIFY1(__LINE__) " " GL_STRINGIFY1(call));  \
}   \

// ============================================================================

/// Retrieves OpenGL error string
std::string getErrorString (const GLenum err);

/// Checks for OpenGL error before a call. Allows to detect unhandled error.
void checkPreCall  (const char* a_Call);
/// Checks for OpenGL error using glGetError. Throws an exception if found one.
void checkPostCall (const char* a_Call);

// ============================================================================

/// Dumps renderer info
void dumpRendererInfo   ();

/// Dumps rendered caps
void dumpRendererCaps   ();

// ============================================================================


}; // GL
#endif // GL_UTILS_HH
