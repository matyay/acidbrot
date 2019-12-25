#include "glfw_app.hh"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

// ============================================================================

GLFWApp::GLFWApp ()
{
    // Create the logger
    m_Logger = spdlog::stderr_color_mt("app");
}

// ============================================================================

void GLFWApp::addWindow (GLFWwindow* a_Window) {
    glfwSetWindowUserPointer(a_Window, (void*)this);
    m_Windows.push_back(a_Window);
}

bool GLFWApp::allWindowsClosed () {

    // Check if any of the windows is still open
    for (auto& window : m_Windows) {
        if (!glfwWindowShouldClose(window)) {
            return false;
        }
    }

    return true;
}

// ============================================================================

int GLFWApp::run () {
    int exitCode = 0;

    double time = glfwGetTime();

    m_FrameRate.time  = 0.0;
    m_FrameRate.count = 0;
    m_FrameRate.rate  = 0.0f;

    // Loop until all windows are closed
    while (!allWindowsClosed()) {

        // Poll events
        glfwPollEvents();

        // Compute time step
        double now = glfwGetTime();
        double dt  = now - time;
        time = now;

        // Compute frame rate
        m_FrameRate.time += dt;
        if (m_FrameRate.time >= 1.0) {
            m_FrameRate.rate   = (float)m_FrameRate.count / m_FrameRate.time;
            m_FrameRate.count  = 0;
            m_FrameRate.time  -= 1.0;

            //SPDLOG_LOGGER_DEBUG(m_Logger, "FPS {:.3f}", m_FrameRate.rate);
        }

        // Invoke the loop method
        int res = loop(dt);
        if (res) {
            exitCode = res;
            break;
        }
    }

    return exitCode;
}

void GLFWApp::frameDone () {
    m_FrameRate.count++;
}

float GLFWApp::getFrameRate () const {
    return m_FrameRate.rate;
}
