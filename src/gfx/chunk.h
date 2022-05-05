#pragma once

#include "bgfx.h"
#include "block.h"
#include "primitive.h"
#include <string>
#include <utility>
#include <vector>

namespace vx::gfx {
    struct Chunk {
        std::string identifier;

        float minX;
        float maxX;
        float minY;
        float maxY;
        float minZ;
        float maxZ;

        std::vector<Block> blocks;
        std::vector<u16> indices;
        std::vector<VertexColorHex> geometry;

        explicit Chunk(const ivec3 &chunkSize, std::string identifier = "Chunk");
    };

    void translateChunk(const vec3 &amount, Chunk &chunk);
}// namespace vx::gfx