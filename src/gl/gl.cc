#include "gl.hh"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <sstream>

namespace GL {

// ============================================================================

/// GL logger
std::shared_ptr<spdlog::logger> logger = spdlog::stderr_color_mt("gl");

// ============================================================================

bool isExtensionAvailable (const std::string& a_Name) {

    // Get the extension string
    std::string extensions;

    // Try the "old" method
    const char* str = (const char*)glGetString(GL_EXTENSIONS);
    GLenum err = glGetError();

    // Success
    if (err == GL_NO_ERROR && str != nullptr) {
        extensions = std::string(str);
    }

    // Failure, use the "new" method
    else {
        GLint numExtensions = 0;
        glGetIntegerv(GL_NUM_EXTENSIONS, &numExtensions);
    
        for (GLint i=0; i<numExtensions; ++i) {
            const char* ext = (const char*)glGetStringi(GL_EXTENSIONS, i); 
            extensions += std::string(ext);
            extensions += " ";
        }
    }

    // Get list of extensions
    std::stringstream ss(extensions);

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

