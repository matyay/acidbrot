# =============================================================================

# GIT submodule initialization & update target
add_custom_target(
    submodules
    COMMAND git submodule update --init --recursive
    VERBATIM
)

# =============================================================================

# spdlog
file(GLOB_RECURSE SPDLOG_SRCS
    spdlog/src/*.cpp
)

add_library(spdlog STATIC ${SPDLOG_SRCS})
add_dependencies(spdlog submodules)
target_compile_definitions(spdlog PRIVATE SPDLOG_COMPILED_LIB)

# STB image
add_library(stb STATIC stb.c)
add_dependencies(stb submodules)

