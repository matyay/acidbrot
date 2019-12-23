#ifndef GL_FRAMEBUFFER_HH
#define GL_FRAMEBUFFER_HH

#include "gl.hh"

#include <vector>
#include <memory>

#include <cstdint>

namespace GL {

// ============================================================================

class Framebuffer
{
public:

    /// Creates the framebuffer
    Framebuffer (size_t a_Width, size_t a_Height, GLenum a_Format, size_t a_Count=1, bool a_WithDepth=true);

    /// Destructor
    virtual ~Framebuffer ();

    /// Returns the FBO
    GLuint  get         ();
    /// Returns a given texture
    GLuint  getTexture  (size_t a_Index=0);
    /// Returns framebuffer width
    size_t  getWidth    () const;
    /// Returns framebuffer height
    size_t  getHeight   () const;
    /// Returns texture format
    GLenum  getFormat   () const;
   
    /// Enables the framebuffer as the render target
    bool    enable      ();
    /// Disables the framebuffer as the render target
    bool    disable     ();
    
    /// Retrieves pixel data. The framebuffer must be active
    std::unique_ptr<uint8_t> readPixels (size_t a_Index = 0);
    
protected:

    /// The framebuffer object
    GLuint  m_Framebuffer = 0;
    /// The buffer for depth data
    GLuint  m_Depthbuffer = 0;

    /// Textures for RGB data
    std::vector<GLuint> m_Textures;

    /// Resolution
    size_t  m_Width  = 0;
    size_t  m_Height = 0;
    
    /// Format
    GLenum  m_Format = GL_RGBA;

    /// Is active
    bool    m_IsActive = false;

    /// Saved viewport and frambeuffer
    struct Context {
        GLuint  framebuffer;
        GLfloat viewport[4];
    } m_SavedContext;
    
    // ................................
};

// ============================================================================

}; // GL
#endif // GL_FRAMEBUFFER_HH

