#include "utils.hh"

#include <string>
#include <sstream>
#include <vector>
#include <stdexcept>

namespace GL {

// ============================================================================

std::string getErrorString(const GLenum err) {

    switch (err)
    {
    // opengl 2 errors (8)
    case GL_NO_ERROR:
        return "GL_NO_ERROR";

    case GL_INVALID_ENUM:
        return "GL_INVALID_ENUM";

    case GL_INVALID_VALUE:
        return "GL_INVALID_VALUE";

    case GL_INVALID_OPERATION:
        return "GL_INVALID_OPERATION";

#ifdef GL_STACK_OVERFLOW
    case GL_STACK_OVERFLOW:
        return "GL_STACK_OVERFLOW";
#endif

#ifdef GL_STACK_UNDERFLOW
    case GL_STACK_UNDERFLOW:
        return "GL_STACK_UNDERFLOW";
#endif

    case GL_OUT_OF_MEMORY:
        return "GL_OUT_OF_MEMORY";

#ifdef GL_TABLE_TOO_LARGE
    case GL_TABLE_TOO_LARGE:
        return "GL_TABLE_TOO_LARGE";
#endif

    // opengl 3 errors (1)
    case GL_INVALID_FRAMEBUFFER_OPERATION:
        return "GL_INVALID_FRAMEBUFFER_OPERATION";

    // glCheckFramebufferStatus error codes
    case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:
        return "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";

#ifdef GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS
    case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS:
        return "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS";
#endif

    case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:
        return "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";

    case GL_FRAMEBUFFER_UNSUPPORTED:
        return "GL_FRAMEBUFFER_UNSUPPORTED";

    // gles 2, 3 and gl 4 error are handled by the switch above
    default:
        return std::string("Unknown error !");
    }
}

void checkPreCall (const char* a_Call) {

    // Check if there is an unhandled error queued
    GLenum res = glGetError();

    // Display a warning
    if (res != GL_NO_ERROR) {
        std::string message;
        message += std::string("Unhandled error ");
        message += getErrorString(res);
        message += std::string(" caught before ");
        message += std::string(a_Call);

        logger->warn(message);
    }
}

void checkPostCall (const char* a_Call) {

    // Get the error
    GLenum res = glGetError();

    // Throw an exception on failure
    if (res != GL_NO_ERROR) {
        std::string message;
        message += std::string(a_Call);
        message += std::string(" failed with ");
        message += getErrorString(res);

        throw std::runtime_error(message);
    }
}

// ============================================================================

void dumpRendererInfo () {

    const std::vector<std::pair<std::string, GLenum>> items = {
        {"GL_VENDOR", GL_VENDOR},
        {"GL_RENDERER", GL_RENDERER},
        {"GL_VERSION", GL_VERSION},
        {"GL_SHADING_LANGUAGE_VERSION", GL_SHADING_LANGUAGE_VERSION}
    };

    // Get version information
    for (auto item : items) {
        const char* str = (const char*)glGetString(item.second);
        GLenum err = glGetError();

        if (str && err == GL_NO_ERROR) {
            logger->info("{:<35}: {}", item.first, str);
        } else {
            logger->info("{:<35}: N/A ({})", item.first, getErrorString(err));
        }
    }

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

    // Dump extensions as separate log lines.
    logger->info("{:<35}:", "GL_EXTENSIONS");
    
    std::stringstream ss(extensions);
    std::string extension;
    while (std::getline(ss, extension, ' ')) {
        logger->info("  {}", extension);
    }
}

void dumpRendererCaps () {

    const std::vector<std::pair<std::string, GLenum>> items = {
        {"GL_MAX_TEXTURE_SIZE", GL_MAX_TEXTURE_SIZE},
        {"GL_MAX_TEXTURE_IMAGE_UNITS", GL_MAX_TEXTURE_IMAGE_UNITS},
        {"GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS", GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS},
        {"GL_MAX_FRAGMENT_UNIFORM_VECTORS", GL_MAX_FRAGMENT_UNIFORM_VECTORS},
        {"GL_MAX_VARYING_VECTORS", GL_MAX_VARYING_VECTORS},
        {"GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS", GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS},
        {"GL_MAX_VERTEX_UNIFORM_VECTORS", GL_MAX_VERTEX_UNIFORM_VECTORS},
        {"GL_MAX_VERTEX_ATTRIBS", GL_MAX_VERTEX_ATTRIBS}
    };

    for (auto item : items) {
        GLint  value;

        glGetIntegerv(item.second, &value);
        GLenum err = glGetError();

        if (err == GL_NO_ERROR) {
            logger->info("{:<35}: {}", item.first, value);
        } else {
            logger->info("{:<35}: N/A ({})", item.first, getErrorString(err));
        }
    }
}

// ============================================================================

}; // GL

