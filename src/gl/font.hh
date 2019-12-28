#ifndef GL_FONT_HH
#define GL_FONT_HH

#include "shader.hh"
#include "gl.hh"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <string>
#include <map>

namespace GL {

// ============================================================================

class Font
{
public:

    /// Constructs a font from a TTF file
	 Font (const std::string a_FileName, size_t a_Height = 16);
    /// Destructor
    ~Font ();

    /// Draws formatted text at given coordinates
    void drawText (float x, float y, const std::string a_Format, ...);

    /// freetype library context
    static FT_Library freeType;

protected:

    /// Glyph data
    struct Glyph {
        GLuint  texture;    /// GL texture
        int     size[2];    /// Size
        int     ofs [2];    /// Offset
        int     advance;    /// X advance
    };

    /// Glyph height
    size_t m_Height;
    /// Glyphs
    std::map<char, Glyph> m_Glyphs;

    /// VAO & VBO
    GLuint m_Vao = 0;
    GLuint m_Vbo = 0;

    /// Draws text at given coordinates
    void  _drawText (float x, float y, const char* a_String);
};

// ============================================================================

class GenericFontShader : public ShaderProgram
{
public:

    /// Constructs a generic font shader
    GenericFontShader ();
};

// ============================================================================

}; // GL
#endif // GL_FONT_HH

