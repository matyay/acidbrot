#ifndef GL_SHADER_HH
#define GL_SHADER_HH

#include "gl.hh"

#include <string>
#include <map>

namespace GL {

// ============================================================================

class Shader
{
public:

    /// A type of #define values map for injection to shader code
    typedef std::map<std::string, std::string> Defines;

    /// Construct a shader from data in memory
    Shader (const std::string a_Name, const std::string& a_Code, GLenum a_Type, const Defines& a_Defines = Defines());
    /// Construct a shader from a file
    Shader (const std::string a_FileName, GLenum a_Type, const Defines& a_Defines = Defines());

    /// Destructor
    virtual ~Shader ();

    // ................................

    /// Returns shader name
    const std::string getName() const;
    /// Returns shader type
    GLenum getType () const;
    /// Returns shader handle
    GLuint get () const;

protected:

    /// Shader name
    std::string m_Name = "";
    /// Shader type. GL_VERTEX_SHADER or GL_FRAGMENT_SHADER
    GLenum      m_Type = 0;
    /// Shader handle
    GLuint      m_Shader = GL_INVALID_VALUE;

    // ................................

    /// Load shader code from a file
    static const std::string load (const std::string a_FileName);
    /// Inject #define statements to the code
    static const std::string injectDefines (const std::string& a_Code, const Defines& a_Defines);
    /// Processes #include statements
    static const std::string processIncludes (const std::string& a_Code, const std::string& a_FileName = "");

    /// Dumps code to the logger
    static void dumpCode (const std::string& a_Code, spdlog::level::level_enum a_Level);

    /// Compile the shader
    virtual void compile (const std::string& a_Code);
};

// ============================================================================

class ShaderProgram
{
public:

    /// Constructor
    ShaderProgram (const Shader& a_VertexShader,
                   const Shader& a_FragmentShader,
                   const std::string a_Name = std::string());
    /// Destructor
    virtual ~ShaderProgram    ();

    // ................................

    /// Returns shader program name
    const std::string getName() const;    
    /// Returns shader program handle
    GLuint get () const;
    /// Queries for attribute location
    GLint getAttribLocation  (const char* a_Name);
    /// Queries for uniform location
    GLint getUniformLocation (const char* a_Name);

protected:

    /// A default constructor
    ShaderProgram () = default;

    /// Shader program name
    std::string m_Name = "";
    /// Shader program handle
    GLuint      m_Program = GL_INVALID_VALUE;
    
    // ................................

    /// Link the shader program
    virtual void link (GLuint a_VertexShader, GLuint a_FragmentShader);    
};

// ============================================================================
}; // GL
#endif // GL_SHADER_HH

