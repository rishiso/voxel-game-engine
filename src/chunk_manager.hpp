#pragma once
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <future>
#include "chunk.hpp"

struct PairHash {
    size_t operator()(const std::pair<int, int>& p) const {
        return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 1);
    }
};

class ChunkManager {
private:
    std::unordered_map<std::pair<int, int>, std::unique_ptr<Chunk>, PairHash> chunks;
    // pending async generation tasks
    std::unordered_map<std::pair<int,int>, std::future<void>, PairHash> generationTasks;

public:
    static constexpr int CHUNK_SIZE = 6;

    std::vector<Chunk*> getChunks() const;
    bool updateChunks(int x, int z);
    std::unordered_set<std::pair<int, int>, PairHash> getDesiredChunks(int x, int z) const;
    // Poll and return any chunk positions whose async generation just completed
    std::vector<std::pair<int,int>> pollGeneratedChunks();
    // Access a chunk pointer by its grid coordinates
    Chunk* getChunk(int x, int z) const;
};