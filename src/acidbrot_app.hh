#ifndef ACIDBROT_APP_HH
#define ACIDBROT_APP_HH

#include <gl/gl.hh>
#include <gl/font.hh>
#include <gl/shader.hh>
#include <gl/texture.hh>
#include <gl/framebuffer.hh>

#include "glfw_app.hh"

#include <array>

// ============================================================================

class AcidbrotApp : public GLFWApp
{
public:

    /// Public constructor
    AcidbrotApp ();
    /// Destructor

protected:

    /// The initialize method
    int initialize ();
    /// The loop method
    int loop (double dt) override;

    // ..........................................

    /// Main window
    GLFWwindow* m_Window = nullptr;

    /// Fonts
    GL::Map<GL::Font>           m_Fonts;
    /// OpenGL shaders
    GL::Map<GL::ShaderProgram>  m_Shaders;
    /// OpenGL textures
    GL::Map<GL::Texture>        m_Textures;
    /// OpenGL framebuffers
    GL::Map<GL::Framebuffer>    m_Framebuffers;

    // Viewport
    struct {

        // Position and velocity X(re), Y(im), Z(zoom), C(color), Re, Im
        std::array<double, 6>   position;
        std::array<double, 6>   velocity;
    } m_Viewport;

};

#endif // ACIDBROT_APP_HH
