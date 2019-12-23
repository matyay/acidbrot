#include "glfw_wrapper.hh"

#include <spdlog/sinks/stdout_color_sinks.h>

#include <GLFW/glfw3.h>

// ============================================================================

std::shared_ptr<GLFWWrapper> GLFWWrapper::getInstance () {
    static std::shared_ptr<GLFWWrapper> wrapper;

    // Initialize
    if (!wrapper) {        
        wrapper.reset(new GLFWWrapper());
    }

    // Return
    return wrapper;
}

// ============================================================================

GLFWWrapper::GLFWWrapper () {

    // Initialize the logger
    m_Logger = spdlog::stderr_color_mt("glfw");

    // Set error callback
    glfwSetErrorCallback(GLFWWrapper::errorCallback);

    // Initialize GLFW, raise an exception if that fails.
    SPDLOG_LOGGER_DEBUG(m_Logger, "Initializing GLFW");
    if(!glfwInit()) {
        throw std::runtime_error("GLFW initialization failed!");
    }

    // Log version string
    m_Logger->info("Version: {}", glfwGetVersionString());
}

GLFWWrapper::~GLFWWrapper () {

    // Terminate GLFW
    SPDLOG_LOGGER_DEBUG(m_Logger, "Terminating GLFW");
    glfwTerminate();
}

void GLFWWrapper::errorCallback (int a_Error, const char* a_Description) {
    auto logger = spdlog::get("glfw");
    logger->error("Error ({}): '{}'", a_Error, a_Description);
}

