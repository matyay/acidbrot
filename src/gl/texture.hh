#ifndef GL_TEXTURE_HH
#define GL_TEXTURE_HH

#include "gl.hh"

#include <string>

namespace GL {

// ============================================================================

class Texture
{
public:

    /// Creates an empty texture
    Texture   ();
    /// Creates an empty texture with given resolution and format
    Texture   (size_t a_Width, size_t a_Height, GLenum a_Format);
    /// Creates a texture from file
    Texture   (const std::string a_FileName);
    
    /// Destructor
    virtual ~Texture  ();

    /// Retreives the texture handle
    GLuint get () const;
    
    /// Returns width (X)
    size_t getWidth  () const;
    /// Returns width (Y)
    size_t getHeight () const;
    /// Returns color format
    GLenum getFormat () const;

    /// Retrieves desired texture target
    GLenum getTarget () const;

    /// Binds the texture
    virtual void bind ();
    /// Unbinds the texture
    virtual void unbind ();

    /// Clears the texture
    virtual void clear ();

protected:

    /// Texture handle
    GLuint  m_Texture = GL_INVALID_VALUE;
   
    /// Bound flag
    bool    m_IsBound = false;

    /// Width (X)
    size_t  m_Width  = 0;
    /// Height (Y)
    size_t  m_Height = 0;
    /// Format
    GLenum  m_Format = 0;

    /// Bind target
    GLenum  m_BindTarget = 0;
    /// Bint target retrieve
    GLenum  m_BindRetrieve = 0;

    // ................................
    
    /// Create the OpenGL object. Raises an exception on failure
    virtual void create ();
};

// ============================================================================

}; // GL
#endif // GL_TEXTURE_HH


