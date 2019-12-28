#include "primitives.hh"
#include "utils.hh"

namespace GL {

// ============================================================================

ScreenQuad::ScreenQuad () {

    // Initialize VAO and VBO
    GL_CHECK(glGenBuffers(1, &m_Vbo));
    GL_CHECK(glGenVertexArrays(1, &m_Vao));

    GL_CHECK(glBindVertexArray(m_Vao));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_Vbo));

    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, 4*4*sizeof(float), nullptr, GL_STREAM_DRAW));
    GL_CHECK(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, 0));
    GL_CHECK(glEnableVertexAttribArray(0));

    GL_CHECK(glBindVertexArray(0));    
}

ScreenQuad::~ScreenQuad () {

    // Free VBO and VAO
    glDeleteBuffers(1, &m_Vbo);
    glDeleteVertexArrays(1, &m_Vao);
}

// ============================================================================

void ScreenQuad::draw (float x0, float y0, float x1, float y1,
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
    GL_CHECK(glBindVertexArray(m_Vao));
    GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, m_Vbo));
    GL_CHECK(glBufferData(GL_ARRAY_BUFFER, 4*4*sizeof(float), vertices, GL_STREAM_DRAW));

    // Render
    GL_CHECK(glDrawArrays(GL_TRIANGLE_STRIP, 0, 4));

    // Cleanup
    GL_CHECK(glBindVertexArray(0));
}

void ScreenQuad::drawFullscreen () {
    draw(-1.0f, -1.0f, +1.0f, +1.0f,
          0.0f,  0.0f,  1.0f,  1.0f);
}

// ============================================================================

}; // GL
