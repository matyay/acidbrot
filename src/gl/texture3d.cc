#include "texture3d.hh"
#include "utils.hh"

#include <utils/stringf.hh>

#include <vector>
#include <fstream>
#include <stdexcept>

namespace GL {

// ============================================================================

Texture3d::Texture3d () : Texture() {

    m_BindTarget   = GL_TEXTURE_3D;
    m_BindRetrieve = GL_TEXTURE_BINDING_3D;
}

Texture3d::Texture3d (const std::string a_FileName) {

    logger->info("Loading 3D texture from '{}'...", a_FileName.c_str());

    m_BindTarget   = GL_TEXTURE_2D;
    m_BindRetrieve = GL_TEXTURE_BINDING_2D;

    // Load data (FIXME: TODO:)
    m_Width  = 64;
    m_Height = 64;
    m_Depth  = 32;
    m_Format = GL_RED;

    std::fstream fs(a_FileName, std::ios_base::in | std::ios_base::binary);

    size_t size = m_Width * m_Height * m_Depth;    
    std::vector<uint8_t> data(size);
    fs.read((char*)data.data(), size);

    // Upload to OpenGL
    GL_CHECK(glBindTexture(GL_TEXTURE_3D, m_Texture));
    
    GL_CHECK(glPixelStorei(GL_UNPACK_ALIGNMENT, 1));
    GL_CHECK(glTexImage3D(GL_TEXTURE_3D, 0, 
                          GL_RED, m_Width, m_Height, m_Depth,
                          0,
                          GL_RED, GL_UNSIGNED_BYTE, (const void*)data.data()));

    // Setup default filtering
    GL_CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
    GL_CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));    

    // Setup default clamping
    GL_CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT));
    GL_CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT));
    GL_CHECK(glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT));

    GL_CHECK(glBindTexture(GL_TEXTURE_3D, 0));
}

// ============================================================================

size_t Texture3d::getDepth () const {
    return m_Depth;
}

// ============================================================================

}; // GL


