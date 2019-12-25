#ifndef GLFW_APP_HH
#define GLFW_APP_HH

#include <spdlog/spdlog.h>

#include <GLFW/glfw3.h>

#include <map>
#include <memory>

// ============================================================================

class GLFWApp
{
public:

    /// Virtual destructor
    virtual ~GLFWApp () {};

    /// The run method
    int run ();

protected:

    /// A window context
    struct WindowContext {
        GLFWwindow*        window;
        std::array<int, 2> position;
        std::array<int, 2> size;
        std::array<int, 2> fbSize;
        bool               sizeChanged;
    };

    /// Private constructor
    GLFWApp ();

    /// Adds a new window
    void addWindow (GLFWwindow* a_Window);
    /// Returns true when all windows are closed
    bool allWindowsClosed ();

    /// Returns true if a given window is fullscreen
    bool isFullscreen  (GLFWwindow* a_Window);
    /// Switches window between fullscreen and normal mode
    void setFullscreen (GLFWwindow* a_Window, bool a_Fullscreen);
    /// Returns true if a window size has changed
    bool sizeChanged   (GLFWwindow* a_Window);

    /// Main loop method
    virtual int loop (double dt) = 0;

    /// Should be called after a frame is done
    void  frameDone ();
    /// Returns the current frame rate
    float getFrameRate () const;

    // ..........................................

    /// Logger
    std::shared_ptr<spdlog::logger> m_Logger;

private:

    /// GLFW Windows
    std::map<GLFWwindow*, WindowContext> m_Windows;

    /// Frame rate counting
    struct {
        double  time;
        size_t  count;
        float   rate;
    } m_FrameRate;
};

#endif // GLFW_APP_HH
