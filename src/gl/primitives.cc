#include "primitives.hh"

namespace GL {

// ============================================================================

void drawRectangle (float x0, float y0, float x1, float y1,
                    float u0, float v0, float u1, float v1)
{

    // Vertex coord data [x,y,u,v]
    const GLfloat vertices[] = {
        x0, y0, u0, v0,
        x1, y0, u1, v0,
        x0, y1, u0, v1,
        x1, y1, u1, v1,
    };

    // Setup rendering
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, vertices);

    // Render
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

    // Cleanup
    glDisableVertexAttribArray(0);
}

void drawFullscreenRect () {
    drawRectangle(-1.0f, -1.0f, +1.0f, +1.0f,
                   0.0f,  0.0f,  1.0f,  1.0f);
}

// ============================================================================

}; // GL
