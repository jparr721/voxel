#include "chunk.h"
#include "../paths.h"
#include <fstream>
#include <spdlog/spdlog.h>
#include <utility>

namespace vx::gfx {
    Chunk::Chunk(const ivec3 &chunkSize, const vec3 &chunkTranslation, std::string moduleName, std::string identifier)
        : shaderModule(std::move(moduleName)), identifier(std::move(identifier)) {
        spdlog::debug("Loading chunk id: {} module: {}", identifier, moduleName);
        // Origin point is always 0 0 0, so we draw from there
        int ii = 0;
        for (int xx = 0; xx < chunkSize.x; ++xx) {
            for (int yy = 0; yy < chunkSize.y; ++yy) {
                for (int zz = 0; zz < chunkSize.z; ++zz) {
                    // TODO - Compute block direction
                    BlockDir::BlockDirIndices baseIndices = BlockDir::kDebug;
                    // TODO - Add custom color

                    // Increment indices to avoid overlapping faces
                    for (auto &index : baseIndices) {
                        index += kCubeVertices.size() * ii;
                        indices.push_back(index);
                    }

                    const u32 color = makeColorFromBlockType(BlockType::kDebug);
                    const vec3 startingPosition = vec3(xx, yy, zz);
                    for (const auto &vertex : makeOffsetCubeVertices(startingPosition)) {
                        geometry.emplace_back(vertex, color);
                    }
                    ++ii;
                }
            }
        }

        // Translate the chunk
        for (auto &[pos, _] : geometry) { pos += chunkTranslation; }
        setBounds();
        spdlog::debug("Chunk loaded successfully");
    }

    void Chunk::write(bool isFixture) noexcept {
        const fs::path filepath = isFixture ? paths::kFixturesPath / fs::path(identifier + ".chunk")
                                            : paths::kAssetsPath / fs::path(identifier + ".chunk");
        std::ofstream chunkfile(filepath, std::ios::out | std::ios::in);
    }

    void Chunk::setBounds() {
        for (const auto &vertex : geometry) {
            const auto x = vertex.position[0];
            const auto y = vertex.position[1];
            const auto z = vertex.position[2];

            minX = std::min(minX, x);
            maxX = std::max(maxX, x);

            minY = std::min(minY, y);
            maxY = std::max(maxY, y);

            minZ = std::min(minZ, z);
            maxZ = std::max(maxZ, z);
        }
    }

    void translateChunk(const vec3 &amount, Chunk &chunk) {
        for (auto &[pos, _] : chunk.geometry) { pos += amount; }
    }
}// namespace vx::gfx