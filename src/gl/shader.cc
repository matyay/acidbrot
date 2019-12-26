#include "shader.hh"
#include "utils.hh"

#include <utils/stringf.hh>

#include <fstream>
#include <sstream>
#include <vector>
#include <stdexcept>

#include <cstddef>

namespace GL {

// ============================================================================

Shader::Shader (const std::string a_Name, const std::string& a_Code, GLenum a_Type, const Defines& a_Defines) :
    m_Name(a_Name),
    m_Type(a_Type)
{
    if (m_Type != GL_VERTEX_SHADER &&
        m_Type != GL_GEOMETRY_SHADER &&
        m_Type != GL_FRAGMENT_SHADER)
    {
        throw std::runtime_error("Invalid shader type specified");
    }

    // Inject defines to the code
    std::string code = injectDefines(a_Code, a_Defines);

    // Process includes
    code = processIncludes(code);

    // Compile the shader
    compile(code);
}

Shader::Shader (const std::string a_FileName, GLenum a_Type, const Defines& a_Defines) :
    m_Name(a_FileName),
    m_Type(a_Type)
{
    // Load code
    std::string code = load(a_FileName);

    // Inject defines to the code
    code = injectDefines(code, a_Defines);

    // Process includes
    code = processIncludes(code, a_FileName);

    // Compile the shader
    compile(code);
}

Shader::~Shader () {

    // Delete the shader
    if (m_Shader != GL_INVALID_VALUE && glIsShader(m_Shader)) {
        glDeleteShader(m_Shader);
    }
}

// ============================================================================

const std::string Shader::getName () const {
    return m_Name;
}

GLenum Shader::getType () const {
    return m_Type;
}

GLuint Shader::get () const {
    return m_Shader;
}

// ============================================================================

const std::string Shader::load (const std::string a_FileName) {

    logger->info("Loading shader code from '{}'...", a_FileName.c_str());

    // Open the file
    std::ifstream file(a_FileName);

    // Error
    if (!file.is_open()) {
        throw std::runtime_error(
            stringf("Error opening '%s'", a_FileName.c_str()));
    }

    // Get its size
    file.seekg(0, std::ios::end);
    size_t size = file.tellg();
    
    // Allocate buffer & read
    std::vector<char> data(size);

    file.seekg(0, std::ios::beg);
    file.read(data.data(), size);

    return std::string(data.begin(), data.end());
}

const std::string Shader::injectDefines (const std::string& a_Code, const Defines& a_Defines) {

    // Nothing to injest
    if (a_Defines.empty()) {
        return a_Code;
    }

    // Generate the #define statements
    std::string defineStatements = "";

    for (auto const& define : a_Defines) {
        const std::string str = std::string("#define ") + define.first + std::string(" ") + define.second;
        logger->debug("Injecting '{}'", str);
        defineStatements += str + std::string("\n");
    }

    // If the code does not contain #version directive then inject defines
    // at the beginning.
    size_t pos = a_Code.find("#version");
    if (pos == std::string::npos) {
        return defineStatements + a_Code;
    }

    // Find next "\n" after #version
    pos = a_Code.find("\n", pos);
    if (pos == std::string::npos) {
        logger->error("Unable to find code after '#version' directive. Aborting injection.");
        return a_Code;
    }

    // Assemble the code. Insert #define statements after #version
    std::string code;
    code += a_Code.substr(0, pos);
    code += std::string("\n");
    code += defineStatements;
    code += a_Code.substr(pos, std::string::npos);

    return code;
}

const std::string Shader::processIncludes (const std::string& a_Code, const std::string& a_FileName) {
    std::stringstream codeStream(a_Code);

    std::string code;
    std::string origLine;
    std::string procLine;

    size_t lineIdx = 1;

    // Error handler
    auto error = [](size_t a_Line, const std::string& a_LineStr, const std::string& a_Error = "") {
        logger->error("Malformed \"#include\" directive: {}", a_Error); 
        logger->error(" {:3d}: '{}'", a_Line, a_LineStr);

        throw std::runtime_error("Shader compilation failed!");
    };

    // Left strip
    auto lstrip = [](const std::string& a_String) {
        size_t pos = a_String.find_first_not_of("\n\r\t\f\v ");
        return (pos == std::string::npos) ? "" : a_String.substr(pos);
    };

    // Right strip
    auto rstrip = [](const std::string& a_String) {
        size_t pos = a_String.find_last_not_of("\n\r\t\f\v ");
        return (pos == std::string::npos) ? "" : a_String.substr(0, pos + 1);
    };

    // Count chars
    auto count = [](const std::string& a_String, char a_Char) {
        size_t n = 0;

        for (size_t i=0; i<a_String.length(); ++i) {
            if (a_String[i] == a_Char) n++;
        }

        return n;
    };

    // Returns base directory for a full file name path
    auto basedir = [](const std::string& a_String) {
        size_t pos = a_String.rfind("/");
        return (pos == std::string::npos) ? "" : a_String.substr(0, pos);
    };

    // Process lines. Look for #include directives and replace them with
    // included code.
    while (std::getline(codeStream, origLine, '\n')) {

        // Left strip
        procLine = lstrip(origLine);

        // Got #include directive
        if (procLine.find("#include") == 0) {

            // Strip keyword
            procLine = procLine.substr(8);

            // There has to be at least one whitespace
            if (!std::isspace(procLine[0])) {
                error(lineIdx, origLine);
            }

            // Strip everything
            procLine = lstrip(rstrip(procLine));

            // Either two quotes or one '<' and one '>'
            size_t numQuotes = count(procLine, '\"');
            size_t numLB = count(procLine, '<');
            size_t numRB = count(procLine, '>');

            if (numQuotes != 0 && numQuotes != 2) {
                error(lineIdx, origLine);
            }
            if (numQuotes == 0 && (numLB != 1 && numRB != 1)) {
                error(lineIdx, origLine);
            }
            if (numQuotes == 2 && (numLB != 0 || numRB != 0)) {
                error(lineIdx, origLine);
            }

            // Get quotes and check them
            char quoteL = procLine[0];
            char quoteR = procLine[procLine.length()-1];

            if (quoteL != '\"' || quoteR != '\"') {
                if (quoteL != '<' || quoteR != '>') {
                    error(lineIdx, origLine);
                }
            }

            // Unquote
            std::string fileName = procLine.substr(1, procLine.length()-2);

            // Prepend include path
            std::string includePath = basedir(a_FileName);
            if (includePath.length()) {
                fileName = includePath + "/" + fileName;
            }

            // Load the included file
            std::string includedCode = load(fileName);

            // Recurse
            includedCode = processIncludes(includedCode, fileName);

            // Insert
            code.append(includedCode);
        }

        // A regular line
        else {
            code.append(origLine + "\n");
        }

        lineIdx++;
    }

    return code;
}

void Shader::dumpCode (const std::string& a_Code, spdlog::level::level_enum a_Level) {
    std::stringstream   codeStream(a_Code);
    std::string         line;

    logger->log(a_Level, "Shader code:");

    for (size_t i=1; std::getline(codeStream, line, '\n'); ++i) {
        logger->log(a_Level, " {:3d}: {}", i, line.c_str());
    }
}

void Shader::compile (const std::string& a_Code) {

    const std::string typeName = (m_Type == GL_VERTEX_SHADER)   ? "vertex"   :
                                 (m_Type == GL_GEOMETRY_SHADER) ? "geometry" :
                                 (m_Type == GL_FRAGMENT_SHADER) ? "fragment" :
                                                                  "unknown";

    logger->info("Compiling {} shader '{}'...",
        typeName.c_str(),
        m_Name.c_str()
    );

    // Create shader object
    m_Shader = glCreateShader(m_Type);
    if (!m_Shader) {
        throw std::runtime_error(
            stringf("glCreateShader() failed! (%s)", getErrorString(glGetError()).c_str())
        );
    }

    // Compile the code
    GLint         length = (GLint)a_Code.length();
    const GLchar* code   = (GLchar*)a_Code.c_str();
    GL_CHECK(glShaderSource (m_Shader, 1, &code, &length));
    GL_CHECK(glCompileShader(m_Shader));

    // Check status
    GLint isCompiled = GL_FALSE;
    GL_CHECK(glGetShaderiv(m_Shader, GL_COMPILE_STATUS, &isCompiled));

    if (!isCompiled) {
        logger->error("Shader compilation failed!");
    }

    // Dump code and compilation log to stderr
    GLint logLength = 0;
    GL_CHECK(glGetShaderiv(m_Shader, GL_INFO_LOG_LENGTH, &logLength));

    if (!isCompiled && logLength > 0) {
        dumpCode(a_Code, spdlog::level::err);
    }
    else {
        dumpCode(a_Code, spdlog::level::trace);
    }

    if (logLength > 0)
    {
        GLint   dummy = 0;
        GLchar* log   = new GLchar[logLength+1];
        GL_CHECK(glGetShaderInfoLog(m_Shader, logLength+1, &dummy, log));

        std::string         logString(log);
        std::stringstream   logStream(logString);
        std::string         line;

        spdlog::level::level_enum level = (isCompiled) ? spdlog::level::info : spdlog::level::err;
        logger->log(level, "Shader compilation log:");
        
        while (std::getline(logStream, line, '\n')) {
            if (line.length()) {
                logger->log(level, "'{}'", line.c_str());
            }
        }

        delete[] log;
    }

    // Raise an exception on failure
    if (!isCompiled) {
        throw std::runtime_error(
            "Shader compilation failed!"
        );
    }
}

// ============================================================================

ShaderProgram::ShaderProgram (const Shader& a_VertexShader,
                                  const Shader& a_FragmentShader,
                                  const std::string a_Name)
{
    // Determine shader program name if not given
    if (a_Name.length() == 0) {
        m_Name = a_VertexShader.getName() + std::string("+") + a_FragmentShader.getName();
    } else {
        m_Name = a_Name;
    }

    // Link it
    link(a_VertexShader.get(), a_FragmentShader.get());
}

ShaderProgram::~ShaderProgram () {
    
    // Delete the shader program
    if (m_Program != GL_INVALID_VALUE && glIsProgram(m_Program)) {
        glDeleteProgram(m_Program);
    }
}

// ============================================================================

void ShaderProgram::link (GLuint a_VertexShader, GLuint a_FragmentShader) {

    logger->info("Linking shader '{}'...",
        m_Name.c_str()
    );

    // Create shader program object
    m_Program = glCreateProgram();
    if (!m_Program) {
        throw std::runtime_error(
            stringf("glCreateProgram() failed! (%s)", getErrorString(glGetError()).c_str())
        );
    }
    
    // Link program
    GL_CHECK(glAttachShader(m_Program, a_VertexShader));
    GL_CHECK(glAttachShader(m_Program, a_FragmentShader));

    GL_CHECK(glLinkProgram(m_Program));
    
    // Check status
    GLint isLinked = GL_FALSE;
    GL_CHECK(glGetProgramiv(m_Program, GL_LINK_STATUS, &isLinked));
    
    // Dump link log to stderr
    GLint logLength = 0;
    GL_CHECK(glGetProgramiv(m_Program, GL_INFO_LOG_LENGTH, &logLength));

    if(logLength > 0)
    {
        GLint   dummy = 0;
        GLchar* log   = new GLchar[logLength+1];
        GL_CHECK(glGetProgramInfoLog(m_Program, logLength+1, &dummy, log));

        std::string         logString(log);
        std::stringstream   logStream(logString);
        std::string         line;

        spdlog::level::level_enum level = (isLinked) ? spdlog::level::info : spdlog::level::err;
        logger->log(level, "Shader linking log:");
        
        while (std::getline(logStream, line, '\n')) {
            logger->log(level, "'{}'", line.c_str());
        }
       
        delete[] log;
    }

    // Raise an exception on failure
    if (!isLinked) {
        throw std::runtime_error(
            "Shader linking failed!"
        );
    }
}

// ============================================================================

const std::string ShaderProgram::getName () const {
    return m_Name;
}

GLuint ShaderProgram::get () const {
    return m_Program;
}

GLint ShaderProgram::getAttribLocation(const char* a_Name) {
    return glGetAttribLocation(m_Program, (const GLchar*)a_Name);
}

GLint ShaderProgram::getUniformLocation(const char* a_Name) {
    return glGetUniformLocation(m_Program, (const GLchar*)a_Name);
}

// ============================================================================

}; // GL

