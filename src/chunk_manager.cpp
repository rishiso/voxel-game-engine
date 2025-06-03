#include "chunk_manager.hpp"

std::vector<std::shared_ptr<Chunk>> ChunkManager::getChunks() const {
    std::vector<std::shared_ptr<Chunk>> chunkList;
    for (const auto& chunk : chunks) {
        chunkList.push_back(chunk.second);
    }
    return chunkList;
}

bool ChunkManager::updateChunks(int x, int z) {
    bool dirty{false};
    std::unordered_set<std::pair<int, int>, PairHash> desiredChunks = getDesiredChunks(x, z);
    for (const auto& pos : desiredChunks) {
        if (!chunks.contains(pos)) {
            chunks[pos] = std::make_shared<Chunk>(pos.first, pos.second);
            chunks[pos]->generateTerrain();
            dirty = true;
        }
    }

    for (auto it = chunks.begin(); it != chunks.end();) {
        if (!desiredChunks.contains(it->first)) {
            it = chunks.erase(it);
            dirty = true;
        } else {
            ++it;
        }
    }

    return dirty;
}

std::unordered_set<std::pair<int, int>, PairHash> ChunkManager::getDesiredChunks(int x, int z) const {
    std::unordered_set<std::pair<int, int>, PairHash> positions;
    for (int i = 0; i < CHUNK_SIZE; ++i) {
        for (int j = 0; j < CHUNK_SIZE; ++j) {
            positions.emplace(x + i - CHUNK_SIZE / 2, z + j - CHUNK_SIZE / 2);
        }
    }
    return positions;
}
