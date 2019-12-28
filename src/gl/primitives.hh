#ifndef GL_PRIMITIVES_HH
#define GL_PRIMITIVES_HH

#include "gl.hh"

namespace GL {

// ============================================================================

class ScreenQuad
{
public:

    // Constructor / desctructor
    ~ScreenQuad ();
     ScreenQuad ();

    /// Draws the quad
    void draw (float x0, float y0, float x1, float y1,
               float u0, float v0, float u1, float v1);

    /// Draws the quad as a fullscreen rectangle
    void drawFullscreen ();

protected:

    // VAO && VBO
    GLuint m_Vao = 0;
    GLuint m_Vbo = 0;
};

// ============================================================================

}; // GL
#endif // GL_PRIMITIVES_HH
