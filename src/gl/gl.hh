#ifndef GL_HH
#define GL_HH

#include <GL/glew.h>

#include <spdlog/spdlog.h>
#include <memory>
#include <map>

namespace GL {

    // ============================================================================

/// GL logger
extern std::shared_ptr<spdlog::logger> logger;

/// A generic map
template <typename T>
using Map = std::map<std::string, std::unique_ptr<T>>;

// ============================================================================

/// Returns true when the given extension is available
bool isExtensionAvailable (const std::string& a_Name);

// ============================================================================

}; // GL
#endif // GL_HH
