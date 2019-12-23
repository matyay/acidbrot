#include "utils.hh"

#include <map>
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

void check (const char* a_Call) {

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

    const std::map<std::string, GLenum> items = {
        {"GL_VENDOR", GL_VENDOR},
        {"GL_RENDERER", GL_RENDERER},
        {"GL_VERSION", GL_VERSION},
        {"GL_SHADING_LANGUAGE_VERSION", GL_SHADING_LANGUAGE_VERSION},
        {"GL_EXTENSIONS", GL_EXTENSIONS}
    };

    for (auto item : items) {
        const char* str = (const char*)glGetString(item.second);
        GLenum err = glGetError();

        if (str && err == GL_NO_ERROR) {
            logger->info("{:<35}: {}", item.first, str);
        } else {
            logger->info("{:<35}: N/A ({})", item.first, getErrorString(err));
        }
    }
}

void dumpRendererCaps () {

    const std::map<std::string, GLenum> items = {
        {"GL_MAX_TEXTURE_SIZE", GL_MAX_TEXTURE_SIZE},
        {"GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS", GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS},
        {"GL_MAX_TEXTURE_IMAGE_UNITS", GL_MAX_TEXTURE_IMAGE_UNITS},
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

