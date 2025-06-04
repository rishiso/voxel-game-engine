#include "chunk_manager.hpp"

std::vector<Chunk*> ChunkManager::getChunks() const {
    std::vector<Chunk*> chunkList;
    for (const auto& chunk : chunks) {
        chunkList.push_back(chunk.second.get());
    }
    return chunkList;
}

bool ChunkManager::updateChunks(int x, int z) {
    bool dirty{false};
    std::unordered_set<std::pair<int, int>, PairHash> desiredChunks = getDesiredChunks(x, z);

    std::vector<std::pair<int, int>> newChunks;

    // Create chunks and queue generation tasks
    for (const auto& pos : desiredChunks) {
        if (!chunks.contains(pos)) {
            chunks[pos] = std::make_unique<Chunk>(pos.first, pos.second);
            generationTasks[pos] = std::async(std::launch::async, [this, pos]() {
                chunks[pos]->generateTerrain();
            });
            dirty = true;
        }
    }

    // Erase obsolete chunks
    for (auto it = chunks.begin(); it != chunks.end();) {
        if (!desiredChunks.contains(it->first)) {
            generationTasks.erase(it->first);
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

std::vector<std::pair<int,int>> ChunkManager::pollGeneratedChunks() {
    std::vector<std::pair<int,int>> ready;
    for (auto it = generationTasks.begin(); it != generationTasks.end();) {
        if (it->second.wait_for(std::chrono::milliseconds(0)) == std::future_status::ready) {
            ready.push_back(it->first);
            it = generationTasks.erase(it);
        } else {
            ++it;
        }
    }
    return ready;
}

Chunk* ChunkManager::getChunk(int x, int z) const {
    auto key = std::make_pair(x, z);
    auto it = chunks.find(key);
    return (it != chunks.end()) ? it->second.get() : nullptr;
}
