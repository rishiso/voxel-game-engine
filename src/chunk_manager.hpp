#pragma once
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include "chunk.hpp"

struct PairHash {
    size_t operator()(const std::pair<int, int>& p) const {
        return std::hash<int>()(p.first) ^ (std::hash<int>()(p.second) << 1);
    }
};

class ChunkManager {
private:
    std::unordered_map<std::pair<int, int>, std::shared_ptr<Chunk>, PairHash> chunks;

public:
    static constexpr int CHUNK_SIZE = 12;

    std::vector<std::shared_ptr<Chunk>> getChunks() const;
    bool updateChunks(int x, int z);
    std::unordered_set<std::pair<int, int>, PairHash> getDesiredChunks(int x, int z) const;
};