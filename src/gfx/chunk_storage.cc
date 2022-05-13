#include "chunk_storage.h"
#include "../paths.h"
#include "../resources.h"

namespace vx::gfx {
    void ChunkStorage::render() {
        for (const auto &chunk : chunks_) {
            const auto moduleName = chunk.shaderModule;
            renderers_.at(moduleName)->render(shaderPrograms_.at(moduleName));
        }
    }

    void ChunkStorage::destroy() {
        spdlog::info("Destroying Chunk Storage");
        for (const auto &[_, renderer] : renderers_) { renderer->destroy(); }
        for (const auto &[_, program] : shaderPrograms_) { bgfx::destroy(program); }
    }

    void ChunkStorage::addChunk(const Chunk &chunk) {
        chunks_.push_back(chunk);
        loadChunks();
    }

    void ChunkStorage::deleteChunk(const Chunk &chunk) {
        for (int ii = 0; ii < chunks_.size(); ++ii) {
            const auto &ck = chunks_.at(ii);
            if (ck.identifier == chunk.identifier) {
                chunks_.erase(chunks_.begin() + ii);
                break;
            }
        }

        for (auto &[moduleName, renderer] : renderers_) {
            renderer->removeChunk(chunk);
            break;
        }
    }

    void ChunkStorage::loadChunks() {
        // TODO(@jparr721) - We need to nuke the module if there are none left in this module
        for (const auto &chunk : chunks_) {
            const auto moduleName = chunk.shaderModule;
            // Load the shader program
            if (shaderPrograms_.find(moduleName) == shaderPrograms_.end()) {
                const auto shaderProgram = vx::loadShaderProgram(paths::kShadersPath, moduleName);
                shaderPrograms_.insert({moduleName, shaderProgram});
            }

            // Load the chunk renderer
            if (renderers_.find(moduleName) == renderers_.end()) {
                renderers_.insert({moduleName, std::make_unique<ChunkRenderer>()});
            }
        }

        for (const auto &chunk : chunks_) {
            const auto moduleName = chunk.shaderModule;
            renderers_.at(moduleName)->addChunk(chunk);
        }
    }

    void ChunkStorage::setChunk(const Chunk &newChunk) {
        for (auto &chunk : chunks_) {
            if (chunk.identifier == newChunk.identifier) {
#ifndef NDEBUG
                const auto chunksSize = chunks_.size();
#endif
                chunk = newChunk;
#ifndef NDEBUG
                assert(chunksSize == chunks_.size());
#endif
                break;
            }
        }
    }
}// namespace vx::gfx
