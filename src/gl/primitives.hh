#ifndef GL_PRIMITIVES_HH
#define GL_PRIMITIVES_HH

#include "gl.hh"

namespace GL {

// ============================================================================

void drawRectangle          (float x0, float y0, float x1, float y1,
                             float u0, float v0, float u1, float v1);

void drawFullscreenRect     ();

// ============================================================================

}; // GL
#endif // GL_FULLSCREEN_QUAD_HH
