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

    // Create and fill window context
    WindowContext context;
    context.window      = a_Window;
    context.sizeChanged = false;

    glfwGetWindowPos (a_Window, &context.position[0], &context.position[1]);
    glfwGetWindowSize(a_Window, &context.size[0],     &context.size[1]);

    glfwGetFramebufferSize(a_Window, &context.fbSize[0], &context.fbSize[1]);

    // Set user pointer
    glfwSetWindowUserPointer(a_Window, (void*)this);

    // Add to the list
    m_Windows[a_Window] = context;

    m_Logger->info("window[{}]: pos=({}, {}), size=({}, {}), fbSize=({}, {})",
        (void*)a_Window,
        context.position[0], context.position[1],
        context.size[0],     context.size[1],
        context.fbSize[0],   context.fbSize[1]);
}

bool GLFWApp::allWindowsClosed () {

    // Check if any of the windows is still open
    for (auto& pair : m_Windows) {
        if (!glfwWindowShouldClose(pair.first)) {
            return false;
        }
    }

    return true;
}

bool GLFWApp::isFullscreen (GLFWwindow* a_Window) {
    return (glfwGetWindowMonitor(a_Window) != nullptr);
}

void GLFWApp::setFullscreen (GLFWwindow* a_Window, bool a_Fullscreen) {

    // Already set
    if (isFullscreen(a_Window) == a_Fullscreen) {
        return;
    }

    // Get context
    auto& context = m_Windows.at(a_Window);

    // Set to fullscreen
    if (a_Fullscreen) {

        // Save position and size
        glfwGetWindowPos (a_Window, &context.position[0], &context.position[1]);
        glfwGetWindowSize(a_Window, &context.size[0],     &context.size[1]);
       
        // Get resolution of monitor
        GLFWmonitor*       monitor = getBestMonitor(a_Window);
        const GLFWvidmode* mode    = glfwGetVideoMode(monitor);

        // Switch to full screen
        glfwSetWindowMonitor(a_Window, monitor, 0, 0, mode->width, mode->height, 0);

        m_Logger->info("window[{}]: fullscreen, monitor='{}', size=({}, {})",
                (void*)a_Window, glfwGetMonitorName(monitor),
                mode->width, mode->height);
    }
    // Switch to windowed
    else {

        glfwSetWindowMonitor(a_Window, nullptr,
            context.position[0], context.position[1],
            context.size[0],     context.size[1],
            0);

        glfwSetWindowSize(a_Window, context.size[0], context.size[1]);

        m_Logger->info("window[{}]: windowed, pos=({}, {}), size=({}, {})",
            (void*)a_Window,
            context.position[0], context.position[1],
            context.size[0],     context.size[1]
            );
    }

    context.sizeChanged = true;
}

bool GLFWApp::sizeChanged (GLFWwindow* a_Window) {
    auto& context = m_Windows.at(a_Window);

    bool res = context.sizeChanged;
    context.sizeChanged = false;

    return res;
}

// ============================================================================

GLFWmonitor* GLFWApp::getBestMonitor (GLFWwindow* a_Window) {

    auto MIN = [](int a, int b) {
        return (a > b) ? b : a;
    };
    auto MAX = [](int a, int b) {
        return (a > b) ? a : b;
    };

    int monitorCount = 0;
    GLFWmonitor** monitors    = glfwGetMonitors(&monitorCount);
    GLFWmonitor*  bestMonitor = glfwGetPrimaryMonitor();

    if (monitors == nullptr || monitorCount <= 0) {
        return bestMonitor;
    }

    int windowX, windowY, windowWidth, windowHeight;
    glfwGetWindowSize(a_Window, &windowWidth, &windowHeight);
    glfwGetWindowPos(a_Window, &windowX, &windowY);

    int bestArea = 0;

    for (int i = 0; i < monitorCount; ++i)
    {
        GLFWmonitor *monitor = monitors[i];

        int monitorX, monitorY;
        glfwGetMonitorPos(monitor, &monitorX, &monitorY);

        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        if (mode == nullptr) {
            continue;
        }

        int areaMinX = MAX(windowX, monitorX);
        int areaMinY = MAX(windowY, monitorY);

        int areaMaxX = MIN(windowX + windowWidth, monitorX + mode->width);
        int areaMaxY = MIN(windowY + windowHeight, monitorY + mode->height);

        int area = (areaMaxX - areaMinX) * (areaMaxY - areaMinY);

        if (area > bestArea) {
            bestArea    = area;
            bestMonitor = monitor;
        }
    }

    return bestMonitor;
}

void GLFWApp::center (GLFWwindow* a_Window, GLFWmonitor* a_Monitor) {

    // Get the monitor mode
    const GLFWvidmode* mode = glfwGetVideoMode(a_Monitor);
    if (mode == nullptr) {
        return;
    }

    // Get monitor size
    int monitorX, monitorY;
    glfwGetMonitorPos(a_Monitor, &monitorX, &monitorY);

    // Get window size
    int windowWidth, windowHeight;
    glfwGetWindowSize(a_Window, &windowWidth, &windowHeight);

    // Center it
    glfwSetWindowPos(a_Window,
                     monitorX + (mode->width  - windowWidth ) / 2,
                     monitorY + (mode->height - windowHeight) / 2);
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
            exitCode = (res < 0) ? res : 0;
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
