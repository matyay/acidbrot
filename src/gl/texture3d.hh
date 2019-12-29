#ifndef GL_TEXTURE3D_HH
#define GL_TEXTURE3D_HH

#include "gl.hh"

#include <string>

#include "texture.hh"

namespace GL {

// ============================================================================

class Texture3d : public Texture
{
public:

    /// Creates an empty texture
    Texture3d ();

    /// Creates an empty texture with given resolution and format
    Texture3d (size_t a_Width, size_t a_Height, size_t a_Depth, GLenum a_Format);

    /// Creates a texture from file
    Texture3d (const std::string a_FileName);
     
    /// Returns depth
    size_t getDepth () const;

    /// Clears the texture
    void clear () {}; // TODO:

protected:

    /// Depth (Z)
    size_t  m_Depth  = 0;
};

// ============================================================================

}; // GL
#endif // GL_TEXTURE_HH


