#include "framebuffer.hh"
#include "utils.hh"

#include <utils/stringf.hh>

#include <stdexcept>

namespace GL {

// ============================================================================

Framebuffer::Framebuffer (size_t a_Width, size_t a_Height, GLenum a_Format, size_t a_Count, bool a_WithDepth) :
    m_Width  (a_Width),
    m_Height (a_Height),
    m_Format (a_Format)
{

    // Check limit of maximum color attachments
#ifdef GL_MAX_COLOR_ATTACHMENTS_NV
    if (a_Count > GL_MAX_COLOR_ATTACHMENTS_NV) {
        throw std::runtime_error(
            stringf("Too many color attachments requested (%d, max is %d)", a_Count, GL_MAX_COLOR_ATTACHMENTS_NV)
        );
    }
#else
    if (a_Count > 1) {
        throw std::runtime_error(
            stringf("Too many color attachments requested (%d, max is 1)", a_Count)
        );
    }
#endif    

    // Create the frame buffer object
    GL_CHECK(glGenFramebuffers(1, &m_Framebuffer));
    
    // Bind it
    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer));

    // Create textures
    m_Textures.resize(a_Count);
    GL_CHECK(glGenTextures(a_Count, (GLuint*)m_Textures.data()));

    for (size_t i=0; i<m_Textures.size(); ++i) {
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, m_Textures[i]));        
        GL_CHECK(glTexImage2D(GL_TEXTURE_2D, 0, m_Format, a_Width, a_Height, 0, m_Format, GL_UNSIGNED_BYTE, 0));
        
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
        
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
    }

    // Create depth buffer
    if (a_WithDepth) {
        GL_CHECK(glGenRenderbuffers(1, &m_Depthbuffer));
        GL_CHECK(glBindRenderbuffer(GL_RENDERBUFFER, m_Depthbuffer));
        GL_CHECK(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, a_Width, a_Height)); // FIXME: Can do better than GL_DEPTH_COMPONENT16 for non-ES OpenGL
        GL_CHECK(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_Depthbuffer));
    }
    
    // Setup color attachments
    for (size_t i=0; i<m_Textures.size(); ++i) {
#ifdef GL_MAX_COLOR_ATTACHMENTS_NV
        GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0_NV + i, GL_TEXTURE_2D, m_Textures[i], 0));
#else
        GL_CHECK(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_Textures[i], 0)); // There will always be 1 texture.
#endif        
    }
    
    // Check
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE");
    }
    
    // Unbind
    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, 0));
    GL_CHECK(glBindRenderbuffer(GL_RENDERBUFFER, 0));
}

Framebuffer::~Framebuffer () {

    // Delete depth buffer
    if (m_Depthbuffer) {
        glDeleteRenderbuffers(1, &m_Depthbuffer);
    }
    
    // Delete textures
    glDeleteTextures(m_Textures.size(), (GLuint*)m_Textures.data());
    
    // Delete frame buffer object
    if (m_Framebuffer) {
        glDeleteFramebuffers(1, &m_Framebuffer);
    }
}

// ============================================================================

GLuint Framebuffer::get () {
    return m_Framebuffer;
}

GLuint Framebuffer::getTexture (size_t a_Index) {
    return m_Textures[a_Index];
}

size_t Framebuffer::getWidth() const {
    return m_Width;
}

size_t Framebuffer::getHeight() const {
    return m_Height;
}

GLenum Framebuffer::getFormat() const {
    return m_Format;
}

// ============================================================================

void Framebuffer::enable () {

    // Already active
    if (m_IsActive) {
        throw std::runtime_error("Framebuffer already in use!");
    }

    // Store the context
    GL_CHECK(glGetFloatv(GL_VIEWPORT, m_SavedContext.viewport));
    GL_CHECK(glGetIntegerv(GL_FRAMEBUFFER_BINDING, (GLint*)&m_SavedContext.framebuffer));

    // Enable the framebuffer
    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, m_Framebuffer));
    GL_CHECK(glViewport(0, 0, m_Width, m_Height));

    m_IsActive = true;
}

void Framebuffer::disable () {

    // Not active
    if (!m_IsActive) {
        throw std::runtime_error("Framebuffer not in use!");
    }

    // An other framebuffer is active ?
    GLint currBinding = 0;
    GL_CHECK(glGetIntegerv(GL_FRAMEBUFFER_BINDING, &currBinding));
    if (currBinding != m_Framebuffer) {
        throw std::runtime_error("Framebuffer bound but elsewhere!");
    }

    // Restore bindings
    GL_CHECK(glBindFramebuffer(GL_FRAMEBUFFER, m_SavedContext.framebuffer));

    GL_CHECK(glViewport(
        m_SavedContext.viewport[0],
        m_SavedContext.viewport[1],
        m_SavedContext.viewport[2],
        m_SavedContext.viewport[3]
    ));

    m_IsActive = false;
}

// ============================================================================

std::unique_ptr<uint8_t> Framebuffer::readPixels (size_t a_Index) {

    // TODO: Index
    (void)a_Index;

    // Sample size
    size_t sampleSize;

    switch (m_Format)
    {
    case GL_RED:    sampleSize = 1; break;
    case GL_RG:     sampleSize = 2; break;
    case GL_RGB:    sampleSize = 3; break;
    case GL_RGBA:   sampleSize = 4; break;

    default:
        throw std::runtime_error("Invalid framebuffer pixel format");
    }

    // Allocate
    size_t size = m_Width * m_Height * sampleSize;
    auto   data = std::unique_ptr<uint8_t>(new uint8_t[size]);

    // Read pixels
    GL_CHECK(glPixelStorei(GL_PACK_ALIGNMENT, 1));
    GL_CHECK(glReadPixels(0, 0, m_Width, m_Height, m_Format,
                          GL_UNSIGNED_BYTE, data.get()));
    
    return data;
}


// ============================================================================

}; // GL

