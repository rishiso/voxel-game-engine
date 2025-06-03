#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.hpp"
#include "camera.hpp"
#include "geometry.hpp"
#include "chunk.hpp"

// Force NVIDIA GPU usage on laptops with dual graphics
extern "C" {
    __declspec(dllexport) unsigned long NvOptimusEnablement = 0x00000001;
    __declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

constexpr int WIDTH = 1920;
constexpr int HEIGHT = 1200;

Camera camera{};

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

int main() {    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Voxel Engine", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }
    glViewport(0, 0, WIDTH, HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    Shader ourShader("shaders/shader.vs", "shaders/shader.fs");

    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    unsigned int VBOinstance;
    glGenBuffers(1, &VBOinstance);
    glBindBuffer(GL_ARRAY_BUFFER, VBOinstance);

    for (unsigned int i = 0; i < 4; i++) {
        glVertexAttribPointer(2 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(i * sizeof(glm::vec4)));
        glEnableVertexAttribArray(2 + i);
        glVertexAttribDivisor(2 + i, 1);
    }

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cubeIndices), cubeIndices, GL_STATIC_DRAW);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CW);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;
    
    float fpsUpdateTime = 0.0f;
    int frameCount = 0;
    float fps = 0.0f;

    Chunk::initializeNoise();

    const int GRID_SIZE = 12;
    std::vector<Chunk> chunks;
    
    for (int x = 0; x < GRID_SIZE; x++) {
        for (int z = 0; z < GRID_SIZE; z++) {
            chunks.emplace_back(x, z);
        }
    }

    std::vector<glm::mat4> modelMatrices;
    for (const auto& chunk : chunks) {
        for (auto& cubePosition : chunk.generateCubePositions()) { 
            glm::mat4 model = glm::mat4(1.0f);
            modelMatrices.push_back(glm::translate(model, cubePosition));
        }
    }
    glBufferData(GL_ARRAY_BUFFER, modelMatrices.size() * sizeof(glm::mat4), modelMatrices.data(), GL_STATIC_DRAW);


    // Set camera to a good position to view the terrain
    camera.Position = glm::vec3(Chunk::CHUNK_WIDTH/2, 80.0f, Chunk::CHUNK_DEPTH/2);

    while(!glfwWindowShouldClose(window)) {
        //input
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // FPS calculation
        frameCount++;
        fpsUpdateTime += deltaTime;
        
        // Update FPS every second
        if (fpsUpdateTime >= 1.0f) {
            fps = frameCount / fpsUpdateTime;
            std::cout << "FPS: " << fps << std::endl;
            frameCount = 0;
            fpsUpdateTime = 0.0f;
        }
        
        processInput(window, deltaTime);

        //rendering commands here
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();
        
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)WIDTH / (float)HEIGHT, 0.1f, 1000.0f);
        ourShader.setMat4("projection", projection);
        
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("view", view);

        // Render all chunks
        glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, modelMatrices.size());

        //check and call events and swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();    
    }

    Chunk::cleanupNoise();
    glfwTerminate();
    return 0;
}