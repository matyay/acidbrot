#include "font.hh"
#include "utils.hh"

#include <utils/stringf.hh>

#include <cstdarg>
#include <stdexcept>

namespace GL {

// ============================================================================

/// The FreeType library context
FT_Library Font::freeType = nullptr;

// ============================================================================

Font::Font (const std::string a_Name, size_t a_Height) :
    m_Height(a_Height)
{
    int res;

    logger->info("Loading font '{}'...", a_Name);

    // Initialzie FreeType if not already initialized
    if (freeType == nullptr) {
        res = FT_Init_FreeType(&freeType);
        if (res) {
            throw std::runtime_error(
                stringf("Error initializing FreeType (%d)", res)
            );
        }
    }

    // Load font face
    FT_Face face;
    res = FT_New_Face(freeType, a_Name.c_str(), 0, &face);
    if (res) {
        throw std::runtime_error(
            stringf("Error loading font '%s' (code %d)", a_Name.c_str(), res)
        );
    }

    // Set face parameters
    FT_Set_Pixel_Sizes(face, 0, m_Height);
    
    // Generate glyphs
    GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));

    for (size_t i=32; i<128; ++i) {

        Glyph glyph;
        
        // Load glyph
        res = FT_Load_Char(face, i, FT_LOAD_RENDER);
        if (res) {
            logger->error("FT_Load_Char() failed! ({})", res);
            continue;
        }

        FT_GlyphSlot g = face->glyph;
        
        // Upload it to OpenGL        
        GL_CHECK(glGenTextures(1, &glyph.texture));
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, glyph.texture));

        GL_CHECK(glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            g->bitmap.width,
            g->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            g->bitmap.buffer
        ));
    
        // Set texture parameters    
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));
        GL_CHECK(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
        
        // Set glyph size information
        glyph.size[0] = g->bitmap.width;
        glyph.size[1] = g->bitmap.rows;
        glyph.ofs[0]  = g->bitmap_left;
        glyph.ofs[1]  = g->bitmap_top;
        glyph.advance = g->advance.x;
        
        // Store the glyph        
        m_Glyphs.insert(std::pair<char, Glyph>(i, glyph));
    }
    
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
    
    // Free font face
    FT_Done_Face(face);   
}

Font::~Font () {

    // Free OpenGL textures
    for (auto it=m_Glyphs.begin(); it!=m_Glyphs.end(); ++it) {
        glDeleteTextures(1, &(it->second.texture));
    }
}

// ============================================================================

void Font::drawText (float x, float y, const std::string a_Format, ...) {
    
    const size_t maxCount = 1024;

    va_list args;
    std::unique_ptr<char[]> buffer(new char[maxCount]);
    
    // Format string
    va_start(args, a_Format);
    vsnprintf(buffer.get(), maxCount, a_Format.c_str(), args);
    va_end(args);
    
    // Draw the text
    _drawText(x, y, buffer.get());
}

void Font::_drawText(float x, float y, const char* a_String) {

    const float x0    = x;
    const float scale = 1.0f;

    // Setup OpenGL state
    GL_CHECK(glEnableVertexAttribArray(0));
    GL_CHECK(glActiveTexture(GL_TEXTURE0));    

    // Render glyphs
    for (char const *p = a_String; *p; ++p) {
        char c = *p;
    
        // Got a carriage return
        if (c == '\r') {
            x = x0;
            continue;
        }
        
        // Got a newline
        if (c == '\n') {
            x  = x0;
            y -= ((float)m_Height * 1.1f) * scale;
            continue;
        }       

        // Got an unknown glyph, replace it with space
        if (!m_Glyphs.count(c)) {
            c = ' ';
        }

        // Get the glyph
        Glyph& glyph = m_Glyphs[c];

        // Compute glyph coordinates
        GLfloat xpos = x + glyph.ofs[0] * scale;
        GLfloat ypos = y - (glyph.size[1] - glyph.ofs[1]) * scale;

        GLfloat w = glyph.size[0] * scale;
        GLfloat h = glyph.size[1] * scale;

        // Advance
        x += (glyph.advance >> 6) * scale;

        // Update VBO for each character
        GLfloat vertices[6][4] = {
            { xpos,     ypos + h,   0.0, 0.0 },            
            { xpos,     ypos,       0.0, 1.0 },
            { xpos + w, ypos,       1.0, 1.0 },

            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos + w, ypos,       1.0, 1.0 },
            { xpos + w, ypos + h,   1.0, 0.0 }           
        };
                
        GL_CHECK(glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, vertices));
            
        // Bind glyph texture
        GL_CHECK(glBindTexture(GL_TEXTURE_2D, glyph.texture));
        
        // Render
        GL_CHECK(glDrawArrays(GL_TRIANGLES, 0, 6));
    }
    
    // Cleanup
    GL_CHECK(glDisableVertexAttribArray(0));
    GL_CHECK(glBindTexture(GL_TEXTURE_2D, 0));
}

// ============================================================================

GenericFontShader::GenericFontShader () : ShaderProgram() {

    static const std::string vertexShaderCode = 
        "#version 100\n"
        "\n"
        "attribute vec4 coord;\n"
        "uniform vec4 viewport;\n"
        "varying vec2 texcoord;\n"
        "\n"
        "void main(void) {\n"
        "  gl_Position = vec4(\n"
        "    2.0 * coord.x / viewport.z - 1.0,\n"
        "    2.0 * coord.y / viewport.w - 1.0,\n"        
        "    0, 1);\n"
        "  texcoord = coord.zw;\n"
        "}\n";

    static const std::string fragmentShaderCode = 
        "#version 100\n"
        "precision mediump float;\n"
        "\n"
        "varying vec2 texcoord;\n"
        "uniform sampler2D tex;\n"
        "uniform vec4 color;\n"
        "\n"
        "void main(void) {\n"
        "  gl_FragColor = vec4(1, 1, 1, texture2D(tex, texcoord).r) * color;\n"
        "}\n";

    // Compile shaders
    Shader vertexShader  ("genericFontVsh", vertexShaderCode,   GL_VERTEX_SHADER);
    Shader fragmentShader("genericFontFsh", fragmentShaderCode, GL_FRAGMENT_SHADER);

    // Set name
    m_Name = "genericFontProg";

    // Link program
    link(vertexShader.get(), fragmentShader.get());
}

// ============================================================================
}; // GL

