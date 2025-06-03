inline constexpr float cubeVertices[] = {
    -0.5f, -0.5f, -0.5f,  0.1f, 0.5f, 0.1f,  // Dark green
    0.5f, -0.5f, -0.5f,   0.2f, 0.7f, 0.2f,  // Medium dark green
    0.5f,  0.5f, -0.5f,   0.3f, 0.9f, 0.3f,  // Bright green 
    -0.5f,  0.5f, -0.5f,  0.1f, 0.6f, 0.1f,  // Medium green
    -0.5f, -0.5f,  0.5f,  0.2f, 0.8f, 0.2f,  // Bright medium green
    0.5f, -0.5f,  0.5f,   0.4f, 1.0f, 0.3f,  // Very bright green
    0.5f,  0.5f,  0.5f,   0.3f, 0.8f, 0.4f,  // Light green
    -0.5f,  0.5f,  0.5f,  0.1f, 0.7f, 0.2f   // Forest green
};

inline constexpr unsigned int cubeIndices[] = {
    0, 1, 2,    2, 3, 0,
    4, 5, 6,    6, 7, 4,
    0, 4, 7,    7, 3, 0,
    1, 5, 6,    6, 2, 1,
    0, 1, 5,    5, 4, 0,
    3, 2, 6,    6, 7, 3
};