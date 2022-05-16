#pragma once

#include "../gfx/chunk.h"
#include "../gfx/chunk_storage.h"
#include <filesystem>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace fs = std::filesystem;

namespace vx::level_editor {
    class Project {
    public:
        std::string name = "Level";

        Project(const Project &cs) = delete;
        auto operator=(const Project &cs) -> Project & = delete;

        static auto getInstance() -> Project *;

        // TODO(@jparr721) Delete this
        void render();
        void destroy();
        void addChunk(const gfx::Chunk &chunk);
        void setChunk(const gfx::Chunk &chunk);
        void deleteChunk(const gfx::Chunk &chunk);

        // ====== Getters
        //
        auto storage() -> std::unique_ptr<gfx::ChunkStorage> & { return chunkStorage_; }
        auto getChunks() const -> const std::vector<gfx::Chunk> &;
        auto getChunkByIdentifier(const std::string &identifier) const -> std::optional<const gfx::Chunk>;

        // Project path management
        auto projectFilePath() -> fs::path;
        auto projectFolderPath() -> fs::path;
        auto fixtureFolderPath() -> fs::path;
        auto gameObjectFolderPath() -> fs::path;

    private:
        static Project *project_;
        Project();

        // Files for this project
        std::vector<gfx::Chunk> fixtureChunks_;
        std::vector<gfx::Chunk> gameObjectChunks_;

        // Unique ref to chunk storage
        std::unique_ptr<gfx::ChunkStorage> chunkStorage_;

        /**
         * Write to the project configuration.
         */
        void write();
    };
}// namespace vx::level_editor
