#include <iostream>
#include <iomanip>
#include <memory>
#include <vector>
#include <cmath>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "PhysicsEngine/Math/Vector3.hpp"
#include "PhysicsEngine/Physics/PointMass.hpp"
#include "PhysicsEngine/Physics/PhysicsWorld.hpp"

// Simple Shader Source
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
void main() {
    gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;
uniform vec4 color;
void main() {
    FragColor = color;
}
)";

int main() {
    // Initialize GLFW
    if (!glfwInit()) return -1;

    // Configure Window
    GLFWwindow* window = glfwCreateWindow(800, 600, "Physics Engine - Point Mass", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // Build Shaders
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    // Setup Geometry (Simple Cube for Ball)
    float vertices[] = {
        -0.5f, -0.5f, 0.0f,  0.5f, -0.5f, 0.0f,  0.5f,  0.5f, 0.0f,
         0.5f,  0.5f, 0.0f, -0.5f,  0.5f, 0.0f, -0.5f, -0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,  0.5f, -0.5f, 0.0f,  0.5f,  0.5f, 0.0f
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Simulation Parameters
    using namespace PhysicsEngine;
    const Vector3 gravity(0.0, -9.81, 0.0);
    const double timeStep = 0.016; // ~60 FPS
    PhysicsWorld world(gravity);

    auto ball = std::make_unique<PointMass>(Vector3(0.0, 5.0, 0.0), 1.0);
    PointMass* ballPtr = ball.get();
    world.addPointMass(std::move(ball));

    // Render Loop
    while (!glfwWindowShouldClose(window)) {
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        world.step(timeStep);

        glUseProgram(shaderProgram);

        // Camera / View Setup
        glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -15.0f));
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

        // Ground
        glm::mat4 modelGround = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.0f, 0.0f));
        modelGround *= glm::scale(modelGround, glm::vec3(10.0f, 0.1f, 10.0f));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelGround));
        glUniform4f(glGetUniformLocation(shaderProgram, "color"), 0.5f, 0.5f, 0.5f, 1.0f);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        // Ball
        const Vector3& pos = ballPtr->getPosition();
        // Simple ground collision in main loop (as before)
        if (pos.y < -2.0f) {
            Vector3 vel = ballPtr->getVelocity();
            vel.y = -vel.y * 0.7;
            ballPtr->setVelocity(vel);
            ballPtr->setPosition(Vector3(pos.x, -2.0f, pos.z));
        }

        glm::mat4 modelBall = glm::translate(glm::mat4(1.0f), glm::vec3(pos.x, pos.y, pos.z));
        modelBall *= glm::scale(modelBall, glm::vec3(1.0f, 1.0f, 1.0f));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelBall));
        glUniform4f(glGetUniformLocation(shaderProgram, "color"), 1.0f, 0.0f, 0.0f, 1.0f);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);

    glfwTerminate();
    return 0;
}
