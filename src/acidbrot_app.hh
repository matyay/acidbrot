#ifndef ACIDBROT_APP_HH
#define ACIDBROT_APP_HH

#include <gl/gl.hh>
#include <gl/font.hh>
#include <gl/shader.hh>
#include <gl/texture.hh>
#include <gl/texture3d.hh>
#include <gl/framebuffer.hh>
#include <gl/primitives.hh>

#include "glfw_app.hh"
#include "filter_mask.hh"
#include "video_encoder.hh"

#include <vector>
#include <array>
#include <iostream>
#include <fstream>

// ============================================================================

class AcidbrotApp : public GLFWApp
{
public:

    /// Public constructor
    AcidbrotApp ();

protected:

    /// Video resolutions
    const std::vector<std::array<size_t, 2>> Resolutions = {
        {640,   480},
        {800,   600},
        {1280,  720},
        {1366,  768},
        {1920, 1080}
    };

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

    /// Updates timers
    void updateTimers (double dt);

    /// Sets shader uniforms
    void setUniforms ();

    /// Saves a screenshot
    void takeScreenshot ();

    /// Starts video recording
    void startRecording ();
    /// Stops video recording
    void stopRecording ();
    /// Records a video frame
    void recordFrame ();

    /// Updates the scene
    int updateScene (double dt);
    /// Renders the scene
    int renderScene ();

    // ..........................................

    /// Main window
    GLFWwindow* m_Window = nullptr;

    /// Screen quad
    std::unique_ptr<GL::ScreenQuad>  m_ScreenQuad;

    /// Fonts
    GL::Map<GL::Font>           m_Fonts;
    /// OpenGL shaders
    GL::Map<GL::ShaderProgram>  m_Shaders;
    /// OpenGL textures
    GL::Map<GL::Texture>        m_Textures;
    /// OpenGL 3D textures
    GL::Map<GL::Texture3d>      m_Textures3d;
    /// OpenGL framebuffers
    GL::Map<GL::Framebuffer>    m_Framebuffers;

    /// FIR filter masks
    GL::Map<FilterMask>         m_Masks;

    /// Screenshot flag
    bool m_DoScreenshot = false;
    /// Have fp64 shader extension
    bool m_HaveFp64 = false;
    /// VSync enabled
    bool m_EnableVSync = true;

    /// Fix the framerate
    bool  m_FixedFrameRate  = false;
    /// Fixed frame rate
    float m_TargetFrameRate = 30.0f;
    /// Frame timer
    float m_FrameTime = 0.0f;

    /// Viewport data
    union Viewport {

        struct {
            std::array<double, 2>   position;
            double                  rotation;
            double                  zoom;
            double                  color;
            std::array<double, 2>   julia;
        };

        double param[7];
    };

    /// Fractal type
    enum class Fractal {
        Mandelbrot,
        Julia
    };

    /// Parameter
    struct Parameter {
        std::string name;
        bool        forShader;
        float       value;

        float       min, max;
        float       speed;

        Parameter (const std::string& _name, bool _forShader, float _value,
                   float _min, float _max, float _speed=1.0) :
            name      (_name),
            forShader (_forShader),
            value     (_value),
            min       (_min),
            max       (_max),
            speed     (_speed)
        {};
    };

    /// Viewport
    struct {
        Viewport position;
        Viewport velocity;
    } m_Viewport;

    /// Fractal type
    Fractal m_Fractal = Fractal::Mandelbrot;

    /// Parameters
    std::map<std::string, Parameter> m_Parameters;
    /// Current parameter
    std::map<std::string, Parameter>::iterator m_CurrParam;

    /// Timers
    std::map<std::string, double> m_Timers;

    /// Video recording
    struct {

        /// Running flag
        bool running = false;

        /// The encoder
        std::unique_ptr<VideoEncoder> encoder;
        /// Output file
        std::ofstream file;

    } m_VideoRec;

    // ..........................................

    /// Static Keyboard callback
    static void _keyCallback (GLFWwindow* a_Window,
                              int a_Key, 
                              int a_Scancode, 
                              int a_Action, 
                              int a_Mods);
};

#endif // ACIDBROT_APP_HH
