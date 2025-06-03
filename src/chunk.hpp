#pragma once

#include <vector>
#include <glm/glm.hpp>

class Chunk
{
public:
    // Chunk dimensions
    static constexpr int CHUNK_WIDTH = 16;
    static constexpr int CHUNK_DEPTH = 16;
    static constexpr int CHUNK_HEIGHT = 256;
    
    // Constructor
    Chunk(int chunkX = 0, int chunkZ = 0);
    
    // Generate all cube positions within this chunk
    std::vector<glm::vec3> generateCubePositions() const;
    
    // Get/Set individual cube at local coordinates (0-15, 0-255, 0-15)
    bool getCube(int x, int y, int z) const;
    void setCube(int x, int y, int z, bool exists);
    
    // Get chunk world coordinates
    int getChunkX() const { return m_chunkX; }
    int getChunkZ() const { return m_chunkZ; }
    
    // Convert local coordinates to world coordinates
    glm::vec3 localToWorld(int x, int y, int z) const;
    
private:
    // Chunk position in chunk coordinates (not world coordinates)
    int m_chunkX;
    int m_chunkZ;
    
    // 3D array to store which cubes exist (true = cube exists, false = air)
    // Using a 1D vector for better memory layout
    std::vector<bool> m_cubes;
    
    // Helper function to convert 3D coordinates to 1D index
    int getIndex(int x, int y, int z) const;
    
    // Check if coordinates are valid
    bool isValidCoordinate(int x, int y, int z) const;
}; 