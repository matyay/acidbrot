#include "gl.hh"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <sstream>

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

    // Get list of extensions
    const std::string extensionsStr(extensions);
    std::stringstream ss(extensionsStr);

    // Find the interesting one
    std::string word;
    while (ss >> word) {
        if (word == a_Name) {
            return true;
        }
    }

    // Not found
    return false;
}

}; // GL

