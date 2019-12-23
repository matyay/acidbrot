#ifndef GLFW_WRAPPER_HH
#define GLFW_WRAPPER_HH

#include <spdlog/spdlog.h>

#include <memory>

// ============================================================================

class GLFWWrapper
{
public:

    /// Destructor
    ~GLFWWrapper ();

    /// Returns an instance of the GLFW wrapper
    static std::shared_ptr<GLFWWrapper> getInstance ();

protected:

    /// Constructor
     GLFWWrapper ();

    /// Logger
    std::shared_ptr<spdlog::logger> m_Logger;

    /// GLFW error callback
    static void errorCallback (int a_Error, const char* a_Description);
};

#endif // GLFW_WRAPPER_HH
