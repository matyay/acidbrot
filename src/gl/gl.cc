#include "gl.hh"

#include <spdlog/sinks/stdout_color_sinks.h>

namespace GL {

// ============================================================================

/// GL logger
std::shared_ptr<spdlog::logger> logger = spdlog::stderr_color_mt("gl");

// ============================================================================

}; // GL

