#include "chunk.hpp"

Chunk::Chunk(int chunkX, int chunkZ) 
    : m_chunkX(chunkX), m_chunkZ(chunkZ), m_cubes(CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH, false)
{
    // Initialize with some basic terrain generation
    // For now, just create a simple ground layer and some random blocks
    for (int x = 0; x < CHUNK_WIDTH; x++) {
        for (int z = 0; z < CHUNK_DEPTH; z++) {
            // Create ground layer (y = 0 to 10)
            for (int y = 0; y < 10; y++) {
                setCube(x, y, z, true);
            }
            
            // Add some random blocks higher up (simple terrain variation)
            if ((x + z + chunkX + chunkZ) % 3 == 0) {
                for (int y = 10; y < 15; y++) {
                    setCube(x, y, z, true);
                }
            }
        }
    }
}

std::vector<glm::vec3> Chunk::generateCubePositions() const {
    std::vector<glm::vec3> positions;
    positions.reserve(CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH);
    
    for (int x = 0; x < CHUNK_WIDTH; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_DEPTH; z++) {
                if (getCube(x, y, z)) {
                    positions.push_back(localToWorld(x, y, z));
                }
            }
        }
    }
    
    return positions;
}

bool Chunk::getCube(int x, int y, int z) const {
    if (!isValidCoordinate(x, y, z)) {
        return false;
    }
    
    return m_cubes[getIndex(x, y, z)];
}

void Chunk::setCube(int x, int y, int z, bool exists) {
    if (!isValidCoordinate(x, y, z)) {
        return;
    }
    
    m_cubes[getIndex(x, y, z)] = exists;
}

glm::vec3 Chunk::localToWorld(int x, int y, int z) const {
    // Convert local chunk coordinates to world coordinates
    float worldX = m_chunkX * CHUNK_WIDTH + x;
    float worldY = y; // Y is the same (height)
    float worldZ = m_chunkZ * CHUNK_DEPTH + z;
    
    return glm::vec3(worldX, worldY, worldZ);
}

int Chunk::getIndex(int x, int y, int z) const {
    // Convert 3D coordinates to 1D index
    // Layout: x + z * CHUNK_WIDTH + y * CHUNK_WIDTH * CHUNK_DEPTH
    return x + z * CHUNK_WIDTH + y * CHUNK_WIDTH * CHUNK_DEPTH;
}

bool Chunk::isValidCoordinate(int x, int y, int z) const {
    return x >= 0 && x < CHUNK_WIDTH &&
           y >= 0 && y < CHUNK_HEIGHT &&
           z >= 0 && z < CHUNK_DEPTH;
}