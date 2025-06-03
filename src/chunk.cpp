#include "chunk.hpp"

// Initialize static noise generator
FastNoise::SmartNode<FastNoise::FractalFBm> Chunk::s_noiseGenerator = nullptr;

void Chunk::initializeNoise() {
    // Configure for Perlin noise
    auto perlin = FastNoise::New<FastNoise::Perlin>();
    
    // Scale noise to make it terrain-like
    auto fractal = FastNoise::New<FastNoise::FractalFBm>();
    fractal->SetSource(perlin);
    fractal->SetOctaveCount(5);  // More octaves = more detail
    
    // Set the final generator
    s_noiseGenerator = fractal;
}

void Chunk::cleanupNoise() {
    s_noiseGenerator = nullptr;
}

Chunk::Chunk(int chunkX, int chunkZ) 
    : m_chunkX(chunkX), m_chunkZ(chunkZ), m_cubes(CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH, false) {
    
    // Initialize noise generator if not already initialized
    if (!s_noiseGenerator) {
        initializeNoise();
    }
    
    // Generate terrain using Perlin noise
    generateTerrain();
}

void Chunk::generateTerrain() {
    // Clear existing blocks
    std::fill(m_cubes.begin(), m_cubes.end(), false);
    
    // Generate new terrain
    for (int x = 0; x < CHUNK_WIDTH; x++) {
        for (int z = 0; z < CHUNK_DEPTH; z++) {
            // Get world coordinates for noise sampling
            int worldX = m_chunkX * CHUNK_WIDTH + x;
            int worldZ = m_chunkZ * CHUNK_DEPTH + z;
            
            // Get terrain height at this position
            int height = getHeightAt(worldX, worldZ);
            
            // Ensure height is within chunk boundaries
            height = std::min(height, CHUNK_HEIGHT - 1);
            height = std::max(height, 0);
            
            // Set all blocks from bottom to height as solid
            for (int y = 0; y <= height; y++) {
                setCube(x, y, z, true);
            }
        }
    }
}

int Chunk::getHeightAt(int worldX, int worldZ) const {
    // Sample 2D noise at the world coordinates
    float noiseValue = s_noiseGenerator->GenSingle2D(worldX * 0.01f, worldZ * 0.01f, 0);
    
    // Convert from [-1,1] to [0,1] range
    noiseValue = (noiseValue + 1.0f) * 0.5f;
    
    // Scale to desired height range
    int baseHeight = 30;  // Base height of the terrain
    int heightVariation = 40;  // Maximum height variation
    
    return baseHeight + static_cast<int>(noiseValue * heightVariation);
}

std::vector<glm::vec3> Chunk::generateCubePositions() const {
    if (!m_positionsDirty) {
        return m_cubePositions;
    }

    m_cubePositions.clear();
    m_cubePositions.reserve(CHUNK_WIDTH * CHUNK_HEIGHT * CHUNK_DEPTH);
    
    for (int x = 0; x < CHUNK_WIDTH; x++) {
        for (int y = 0; y < CHUNK_HEIGHT; y++) {
            for (int z = 0; z < CHUNK_DEPTH; z++) {
                if (getCube(x, y, z)) {
                    m_cubePositions.push_back(localToWorld(x, y, z));
                }
            }
        }
    }
    
    m_positionsDirty = false;
    return m_cubePositions;
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
    m_positionsDirty = true;
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