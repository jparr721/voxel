#include <cstddef>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <spdlog/spdlog.h>

#include "src/geometry.h"
#include "src/math.h"
#include "src/resources.h"
#include "src/trigonometry.h"
#include "src/window.h"

static const vec2 SIZE = vec2(1280, 720);

int main(int argc, char *argv[]) {
#ifdef NDEBUG
    spdlog::info("Logging level is at info");
    spdlog::set_level(spdlog::level::info);
#else
    spdlog::info("Logging level is at debug");
    spdlog::set_level(spdlog::level::debug);
#endif
    vx::launchWindow(SIZE, "Voxel");
}
