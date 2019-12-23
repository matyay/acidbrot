#ifndef GLFW_APP_HH
#define GLFW_APP_HH

#include <spdlog/spdlog.h>

#include <GLFW/glfw3.h>

#include <vector>
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

    /// Private constructor
    GLFWApp ();

    /// Adds a new window
    void addWindow (GLFWwindow* a_Window);
    /// Returns true when all windows are closed
    bool allWindowsClosed ();

    /// Main loop method
    virtual int loop (double dt) = 0;

    /// Should be called after a frame is done
    void frameDone ();
    /// Returns the current frame rate
    float getFrameRate () const;

    // ..........................................

    /// Logger
    std::shared_ptr<spdlog::logger> m_Logger;

private:

    /// GLFW Windows
    std::vector<GLFWwindow*> m_Windows;

    /// Frame rate counting
    struct {
        double  time;
        size_t  count;
        float   rate;
    } m_FrameRate;
};

#endif // GLFW_APP_HH
