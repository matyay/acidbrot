#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

// ============================================================================

int main (int argc, char* argv[]) {

    spdlog::set_pattern("%n: %^%v%$");
    spdlog::set_level(spdlog::level::debug);

    auto logger = spdlog::stderr_color_mt("master");

    logger->info("AcidBrot");
    return 0;
}
