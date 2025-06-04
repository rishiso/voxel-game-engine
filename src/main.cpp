#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <memory>

#include "shader.hpp"
#include "camera.hpp"
#include "geometry.hpp"
#include "chunk.hpp"
#include "chunk_manager.hpp"

// Force NVIDIA GPU usage on laptops with dual graphics
extern "C" {
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

constexpr int WIDTH = 1920;
constexpr int HEIGHT = 1200;

Camera camera{};
ChunkManager chunkManager{};

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    static bool firstMouse = true;
    static float lastX = WIDTH / 2.0f;
    static float lastY = HEIGHT / 2.0f;

    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
}

void processInput(GLFWwindow *window, float deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void setupOpenGL() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);
}

void globalToChunk(int worldX, int worldZ, int& chunkX, int& chunkZ) {
    chunkX = worldX / Chunk::CHUNK_WIDTH;
    chunkZ = worldZ / Chunk::CHUNK_DEPTH;
}

GLFWwindow* initializeWindow() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Voxel Engine", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return nullptr;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return nullptr;
    }
    
    glViewport(0, 0, WIDTH, HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
    return window;
}

void setupInputCallbacks(GLFWwindow* window) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
}

struct RenderBuffers {
    unsigned int VAO;
    unsigned int VBO;
    unsigned int VBOinstance;
    unsigned int EBO;
};

RenderBuffers setupRenderBuffers() {
    RenderBuffers buffers{};
    
    glGenVertexArrays(1, &buffers.VAO);
    glBindVertexArray(buffers.VAO);

    glGenBuffers(1, &buffers.VBO);
    glBindBuffer(GL_ARRAY_BUFFER, buffers.VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glGenBuffers(1, &buffers.VBOinstance);
    glBindBuffer(GL_ARRAY_BUFFER, buffers.VBOinstance);

    for (unsigned int i = 0; i < 4; i++) {
        glVertexAttribPointer(2 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * sizeof(glm::vec4)));
        glEnableVertexAttribArray(2 + i);
        glVertexAttribDivisor(2 + i, 1);
    }

    glGenBuffers(1, &buffers.EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers.EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);
    
    return buffers;
}

std::vector<glm::mat4> generateModelMatrices(const std::vector<Chunk*>& chunks) {
    std::vector<glm::mat4> modelMatrices;
    for (const auto& chunk : chunks) {
        for (auto& cubePosition : chunk->generateCubePositions()) { 
            glm::mat4 model = glm::mat4(1.0f);
            modelMatrices.push_back(glm::translate(model, cubePosition));
        }
    }
    return modelMatrices;
}

struct FrameTimer {
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    float fpsUpdateTime = 0.0f;
    int frameCount = 0;
    float fps = 0.0f;
    
    void update() {
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        frameCount++;
        fpsUpdateTime += deltaTime;
        
        if (fpsUpdateTime >= 1.0f) {
            fps = frameCount / fpsUpdateTime;
            std::cout << "FPS: " << fps << std::endl;
            frameCount = 0;
            fpsUpdateTime = 0.0f;
        }
    }
};

void render(Shader& shader, const std::vector<glm::mat4>& modelMatrices) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    shader.use();
    
    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 1000.0f);
    shader.setMat4("projection", projection);
    
    glm::mat4 view = camera.GetViewMatrix();
    shader.setMat4("view", view);

    glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, modelMatrices.size());
}

int main() {    
    GLFWwindow* window = initializeWindow();
    if (!window) {
        return -1;
    }

    Shader ourShader("shaders/shader.vs", "shaders/shader.fs");
    RenderBuffers buffers = setupRenderBuffers();
    setupOpenGL();
    setupInputCallbacks(window);
    
    FrameTimer timer;
    Chunk::initializeNoise();

    camera.Position = glm::vec3(Chunk::CHUNK_WIDTH/2, 80.0f, Chunk::CHUNK_DEPTH/2);

    std::vector<glm::mat4> modelMatrices = generateModelMatrices(chunkManager.getChunks());
    glBindBuffer(GL_ARRAY_BUFFER, buffers.VBOinstance);
    glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), modelMatrices.data(), GL_DYNAMIC_DRAW);

    while(!glfwWindowShouldClose(window)) {
        timer.update();
        processInput(window, timer.deltaTime);
        
        int chunkX, chunkZ;
        globalToChunk(camera.Position.x, camera.Position.z, chunkX, chunkZ);
        chunkManager.updateChunks(chunkX, chunkZ);
        auto completed = chunkManager.pollGeneratedChunks();
        
        if (!completed.empty()) {
            std::vector<glm::mat4> newMatrices = generateModelMatrices(chunkManager.getChunks());
            glBindBuffer(GL_ARRAY_BUFFER, buffers.VBOinstance);
            glBufferData(GL_ARRAY_BUFFER, newMatrices.size() * sizeof(glm::mat4), nullptr, GL_DYNAMIC_DRAW);
            glBufferSubData(GL_ARRAY_BUFFER, 0, newMatrices.size() * sizeof(glm::mat4), newMatrices.data());
            modelMatrices.swap(newMatrices);
        }

        render(ourShader, modelMatrices);

        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    Chunk::cleanupNoise();
    glfwTerminate();
    return 0;
}