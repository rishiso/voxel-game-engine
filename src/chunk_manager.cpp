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

    std::vector<std::future<void>> futures;
    std::vector<std::pair<int, int>> newChunks;

    // First pass: create chunks and collect new ones
    for (const auto& pos : desiredChunks) {
        if (!chunks.contains(pos)) {
            chunks[pos] = std::make_unique<Chunk>(pos.first, pos.second);
            newChunks.push_back(pos);
            dirty = true;
        }
    }
    
    // Second pass: launch parallel terrain generation on separate threads
    for (const auto& pos : newChunks) {
        futures.push_back(std::async(std::launch::async, [this, pos]() {
            chunks[pos]->generateTerrain();
        }));
    }
    
    // Wait for all threads to complete
    for (auto& future : futures) {
        future.wait();
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
