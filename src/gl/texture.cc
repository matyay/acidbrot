#include "texture.hh"
#include "utils.hh"

#include <utils/stringf.hh>

#include <stb/stb_image.h>

#include <memory>
#include <stdexcept>

namespace GL {

// ============================================================================

Texture::Texture () {
    create();

    m_BindTarget   = GL_TEXTURE_2D;
    m_BindRetrieve = GL_TEXTURE_BINDING_2D;
}

Texture::Texture (size_t a_Width, size_t a_Height, GLenum a_Format) :
    m_Width  (a_Width),
    m_Height (a_Height),
    m_Format (a_Format)
{
    // Create the OpenGL object
    create();

    m_BindTarget   = GL_TEXTURE_2D;
    m_BindRetrieve = GL_TEXTURE_BINDING_2D;

    // Allocate
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_Texture));
    GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, a_Format, a_Width, a_Height, 0,
                          a_Format, GL_UNSIGNED_BYTE, nullptr));

    // Setup default filtering
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));    

    // Setup default clamping
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

    GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
}

Texture::Texture (const std::string a_FileName) {

    logger->info("Loading texture from '{}'...", a_FileName.c_str());

    // Create the OpenGL object
    create();

    m_BindTarget   = GL_TEXTURE_2D;
    m_BindRetrieve = GL_TEXTURE_BINDING_2D;

    // Load the texture from file
    int dx, dy, channels;
    
    stbi_uc* image = stbi_load(a_FileName.c_str(), &dx, &dy, &channels, 4);
    if (image == nullptr) {
        throw std::runtime_error(
            stringf("stbi_load() failed!", a_FileName.c_str())
        );
    }

    // Setup info
    m_Width  = dx;
    m_Height = dy;
    m_Format = GL_RGBA;
        
    // Upload to OpenGL
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_Texture));
    
    GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
    GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, dx, dy, 0,
                          GL_RGBA, GL_UNSIGNED_BYTE, image));
    
    // Setup default filtering
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));    

    // Setup default clamping
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));

    GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
    
    // Free image data
    stbi_image_free(image);
}

Texture::~Texture () {

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
    GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, m_Format, m_Width, m_Height, 0,
                          m_Format, GL_UNSIGNED_BYTE, zeros.get()));
}

// ============================================================================

GLuint Texture::get () const {
    return m_Texture;
}

size_t Texture::getWidth () const {
    return m_Width;
}

size_t Texture::getHeight () const {
    return m_Height;
}

GLenum Texture::getFormat () const {
    return m_Format;
}

GLenum Texture::getTarget () const {
    return m_BindTarget;
}

// ============================================================================

void Texture::bind () {

    // Already bound
    if (m_IsBound) {
        throw std::runtime_error("Texture already bound!");
    }

    // Bind it
    GL_CHECK(glBindTexture(m_BindTarget, m_Texture));
    m_IsBound = true;
}

void Texture::unbind () {

    // Not bound
    if (m_IsBound) {
        throw std::runtime_error("Texture not bound!");
    }

    // Bound but elsewhere
    GLint currBinding = 0;
    GL_CHECK(glGetIntegerv(m_BindTarget, &currBinding));
    if (currBinding != m_Texture) {
        throw std::runtime_error("Texture bound but elsewhere!");
    }

    // Unbind
    GL_CHECK(glBindTexture(m_BindTarget, m_Texture));
    m_IsBound = false;
}

// ============================================================================

}; // GL


