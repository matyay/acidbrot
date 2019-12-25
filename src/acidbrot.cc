#include "glfw_wrapper.hh"
#include "acidbrot_app.hh"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

// ============================================================================

int main (int argc, char* argv[]) {

    (void)argc;
    (void)argv;
    
    spdlog::set_pattern("%n: %^%v%$");
    spdlog::set_level(spdlog::level::debug);

    auto logger = spdlog::stderr_color_mt("master");
    logger->info("spdlog v{}.{}.{}", SPDLOG_VER_MAJOR,
                                     SPDLOG_VER_MINOR,
                                     SPDLOG_VER_PATCH);

    int  exitCode = 0;

    try {
        // Initialize GLFW
        auto glfw = GLFWWrapper::getInstance();

        // Initialize the app
        AcidbrotApp app;

        // Run the app
        exitCode = app.run();
    }

    catch (const std::runtime_error& ex) {
        logger->critical("std::runtime_error: '{}'", ex.what());
        return -1;
    }
    
    catch (const std::exception& ex) {
        logger->critical("std::exception: '{}'", ex.what());
        return -1;
    }

    SPDLOG_LOGGER_DEBUG(logger, "Exitting with {}", exitCode);
    return exitCode;
}
