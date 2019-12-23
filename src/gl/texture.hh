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
    Texture   (GLint a_Filter=GL_LINEAR,
               GLint a_Border=GL_REPEAT);

    /// Creates an empty texture with given resolution and format
    Texture   (size_t a_Width, size_t a_Height, GLenum a_Format,
               GLint a_Filter=GL_LINEAR,
               GLint a_Border=GL_REPEAT);

    /// Creates a texture from file
    Texture   (const std::string a_FileName,
               GLint a_Filter=GL_LINEAR,
               GLint a_Border=GL_REPEAT);
    
    /// Destructor
    virtual ~Texture  ();

    /// Retreives texture handle
    GLuint get () const;
    
    /// Retireves texture parameters
    size_t getWidth  () const;
    size_t getHeight () const;
    GLenum getFormat () const;

    /// Retrieves desired texture target
    virtual GLenum getTarget () const;

    /// Clears the texture
    virtual void clear ();

protected:

    /// Resolution
    size_t  m_Width  = 0;
    size_t  m_Height = 0;
    GLenum  m_Format = 0;

    /// Texture handle
    GLuint  m_Texture = GL_INVALID_VALUE;
    
    // ................................
    
    /// Create the OpenGL object. Raises an exception on failure
    void create ();
    /// Setup texture filtering
    void setupFiltering (GLenum a_Target, GLint a_Filter, GLint a_Border);
};

// ============================================================================

}; // GL
#endif // GL_TEXTURE_HH


