#include "texture.hh"
#include "utils.hh"

#include <utils/stringf.hh>

#include <stb/stb_image.h>

#include <memory>
#include <stdexcept>

namespace GL {

// ============================================================================

Texture::Texture (GLint a_Filter, GLint a_Border) {
    
    // Create the OpenGL object
    create();

    // Set filtering    
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_Texture));
    GL_CHECK(setupFiltering(GL_TEXTURE_2D, a_Filter, a_Border));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
}

Texture::Texture (size_t a_Width, size_t a_Height, GLenum a_Format, GLint a_Filter, GLint a_Border) :
    m_Width  (a_Width),
    m_Height (a_Height),
    m_Format (a_Format)
{
    // Create the OpenGL object
    create();

    // Allocate
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_Texture));
    GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, a_Format, a_Width, a_Height, 0, a_Format, GL_UNSIGNED_BYTE, nullptr));

    // Set filtering
    setupFiltering(GL_TEXTURE_2D, a_Filter, a_Border);
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
}

Texture::Texture(const std::string a_FileName, GLint a_Filter, GLint a_Border) {

    logger->info("Loading texture from '{}'...", a_FileName.c_str());

    // Create the OpenGL object
    create();
    
    // Load the texture from file
    int dx, dy, channels;
    
    stbi_uc* image = stbi_load(a_FileName.c_str(), &dx, &dy, &channels, 4);
    if (image == nullptr) {
        throw std::runtime_error(
            stringf("stbi_load() failed!", a_FileName.c_str())
        );
    }
    
    logger->debug("{}x{} c={}", dx, dy, channels);

    // Setup info
    m_Width  = dx;
    m_Height = dy;
    m_Format = GL_RGBA;
        
    // Upload to OpenGL
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_Texture));
    
    GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
    GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dx, dy, 0, GL_RGBA, GL_UNSIGNED_BYTE, image));
    
    // Set filtering
    setupFiltering(GL_TEXTURE_2D, a_Filter, a_Border);
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
    
    // Free image data
    stbi_image_free(image);
}

Texture::~Texture() {

    // If the texture handle is valid then free it
    if (m_Texture) {
        glDeleteTextures(1, &m_Texture);
    }
}

// ============================================================================

void Texture::create () {

    // Create the texture
    glGenTextures(1, &m_Texture);

    // Raise exception on error
    if (!m_Texture) {
        throw std::runtime_error(
            stringf("glGenTextures() failed! (%s)", getErrorString(glGetError()).c_str())
        );
    }
}

void Texture::clear () {
    size_t size  = m_Width * m_Height * 4; // FIXME: May not work with GL_RGB format
    auto   zeros = std::unique_ptr<uint8_t>(new uint8_t[size]);
    
    memset(zeros.get(), 0, size);
    GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, m_Format, m_Width, m_Height, 0, m_Format, GL_UNSIGNED_BYTE, zeros.get()));
}

void Texture::setupFiltering (GLenum a_Target, GLint a_Filter, GLint a_Border) {

    // Setup default filtering
    GL_CHECK(glTexParameteri(a_Target, GL_TEXTURE_MIN_FILTER, a_Filter));
    GL_CHECK(glTexParameteri(a_Target, GL_TEXTURE_MAG_FILTER, a_Filter));    

    // Setup default clamping
    GL_CHECK(glTexParameteri(a_Target, GL_TEXTURE_WRAP_S, a_Border));
    GL_CHECK(glTexParameteri(a_Target, GL_TEXTURE_WRAP_T, a_Border));
}

// ============================================================================

GLuint Texture::get() const {
    return m_Texture;
}

size_t Texture::getWidth() const {
    return m_Width;
}

size_t Texture::getHeight() const {
    return m_Height;
}

GLenum Texture::getFormat() const {
    return m_Format;
}

GLenum Texture::getTarget() const {
    return GL_TEXTURE_2D;
}

// ============================================================================

}; // GL


