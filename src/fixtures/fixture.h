#pragma once

#include "../gfx/chunk_renderer.h"
#include <vector>

namespace vx::fixtures {
    struct Fixture {
        std::vector<gfx::Chunk> chunks;
        std::unique_ptr<gfx::ChunkRenderer> renderer;
    };

    auto initializeFixture(std::vector<gfx::Chunk> &chunks) -> Fixture;
    auto getBaseLayerFixture() -> Fixture &;
}// namespace vx::fixtures