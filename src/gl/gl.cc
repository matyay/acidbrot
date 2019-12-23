#include "gl.hh"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace GL {

// ============================================================================

/// GL logger
std::shared_ptr<spdlog::logger> logger = spdlog::stderr_color_mt("gl");

// ============================================================================

bool isExtensionAvailable (const std::string& a_Name) {

    // Get OpenGL extension string.
    const char* extensions = (const char*)glGetString(GL_EXTENSIONS);
    if (extensions == nullptr) {
        return false;
    }

    // Check for the one
    return (std::string(extensions).find(a_Name) != std::string::npos);
}

}; // GL

