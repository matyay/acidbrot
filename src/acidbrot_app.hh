#ifndef ACIDBROT_APP_HH
#define ACIDBROT_APP_HH

#include <gl/gl.hh>
#include <gl/font.hh>
#include <gl/shader.hh>
#include <gl/texture.hh>
#include <gl/framebuffer.hh>

#include "glfw_app.hh"
#include "filter_mask.hh"

#include <array>

// ============================================================================

class AcidbrotApp : public GLFWApp
{
public:

    /// Public constructor
    AcidbrotApp ();

protected:

    /// The initialize method
    int initialize ();
    /// The loop method
    int loop (double dt) override;

    // ..........................................

    /// Initializes / Reinitializes framebuffers
    int initializeFramebuffers ();

    /// Keyboard callback
    void keyCallback (GLFWwindow* a_Window,
                      int a_Key, 
                      int a_Scancode, 
                      int a_Action, 
                      int a_Mods);

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

    GL::Map<FilterMask>         m_Masks;

    /// Have fp64 shader extension
    bool m_HaveFp64 = false;

    /// Viewport data
    union Viewport {

        struct {
            std::array<double, 2>   position;
            double                  rotation;
            double                  zoom;
            double                  color;
            std::array<double, 2>   julia;
        };

        double param[];
    };

    /// Fractal type
    enum class Fractal {
        Mandelbrot,
        Julia
    };

    /// Viewport
    struct {
        Viewport position;
        Viewport velocity;
    } m_Viewport;

    /// Fractal type
    Fractal m_Fractal = Fractal::Mandelbrot;

    // ..........................................

    /// Static Keyboard callback
    static void _keyCallback (GLFWwindow* a_Window,
                              int a_Key, 
                              int a_Scancode, 
                              int a_Action, 
                              int a_Mods);
};

#endif // ACIDBROT_APP_HH
