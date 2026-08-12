#include <iostream>
#include <iomanip>
#include <memory>
#include <vector>
#include <cmath>
#include <cstdlib>
#include <ctime>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "PhysicsEngine/Math/Vector3.hpp"
#include "PhysicsEngine/Physics/PointMass.hpp"
#include "PhysicsEngine/Physics/PhysicsWorld.hpp"
#include "PhysicsEngine/Geometry/SphereMesh.hpp"
#include "PhysicsEngine/Utils/MP4Recorder.hpp"

// Lighting Shader Sources
const char* vertexShaderSource = R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out vec3 FragPos;
out vec3 Normal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;
    gl_Position = projection * view * vec4(FragPos, 1.0);
}
)";

const char* fragmentShaderSource = R"(
#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;

uniform vec3 color;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main() {
    // Ambient
    float ambientStrength = 0.35;
    vec3 ambient = ambientStrength * color;

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * color;

    // Specular
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32.0);
    vec3 specular = specularStrength * spec * vec3(1.0, 1.0, 1.0);

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);
}
)";

// Global state for controls
float cameraAngle = 0.0f;
float cameraHeight = 3.0f;
float cameraDistance = 12.0f;
bool isPaused = false;
bool spacePressedLast = false;
bool rPressedLast = false;
bool pPressedLast = false;
bool vPressedLast = false;

void populateInitialScene(PhysicsEngine::PhysicsWorld& world) {
    world.clear();
    using namespace PhysicsEngine;

    // Create 4 initial spheres with different positions, radii, masses, and colors
    auto sphere1 = std::make_unique<PointMass>(Vector3(-0.8, 6.0, -0.5), 1.0, 0.5, Vector3(0.9, 0.2, 0.2)); // Red
    sphere1->setRestitution(0.85);

    auto sphere2 = std::make_unique<PointMass>(Vector3(0.8, 7.5, 0.5), 1.5, 0.6, Vector3(0.2, 0.6, 0.9)); // Blue
    sphere2->setRestitution(0.80);

    auto sphere3 = std::make_unique<PointMass>(Vector3(0.0, 9.0, 0.0), 0.8, 0.4, Vector3(0.2, 0.9, 0.3)); // Green
    sphere3->setRestitution(0.90);

    auto sphere4 = std::make_unique<PointMass>(Vector3(-0.3, 11.0, 0.3), 2.0, 0.7, Vector3(0.9, 0.8, 0.2)); // Gold
    sphere4->setRestitution(0.75);

    world.addPointMass(std::move(sphere1));
    world.addPointMass(std::move(sphere2));
    world.addPointMass(std::move(sphere3));
    world.addPointMass(std::move(sphere4));
}

void spawnRandomSphere(PhysicsEngine::PhysicsWorld& world) {
    using namespace PhysicsEngine;
    double rx = (std::rand() % 200 - 100) / 100.0 * 1.5;
    double rz = (std::rand() % 200 - 100) / 100.0 * 1.5;
    double ry = 7.0 + (std::rand() % 50) / 10.0;
    double radius = 0.3 + (std::rand() % 40) / 100.0;
    double mass = radius * radius * radius * 4.0;

    double cr = (std::rand() % 100) / 100.0;
    double cg = (std::rand() % 100) / 100.0;
    double cb = (std::rand() % 100) / 100.0;

    auto sphere = std::make_unique<PointMass>(Vector3(rx, ry, rz), mass, radius, Vector3(cr, cg, cb));
    sphere->setVelocity(Vector3((std::rand() % 100 - 50) / 50.0, -1.0, (std::rand() % 100 - 50) / 50.0));
    sphere->setRestitution(0.8);
    world.addPointMass(std::move(sphere));
}

int main() {
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    const int windowWidth = 1024;
    const int windowHeight = 768;

    GLFWwindow* window = glfwCreateWindow(windowWidth, windowHeight, "Physics Engine - Phase 1 Demo", NULL, NULL);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);

    // Print Controls
    std::cout << "========================================================\n";
    std::cout << " Physics Engine Simulation - Phase 1 Demo\n";
    std::cout << "========================================================\n";
    std::cout << " Controls:\n";
    std::cout << "  [LEFT / RIGHT] Arrow Keys: Orbit camera horizontally\n";
    std::cout << "  [UP / DOWN]    Arrow Keys: Adjust camera height\n";
    std::cout << "  [SPACE]                  : Spawn a new random sphere\n";
    std::cout << "  [R]                      : Reset simulation\n";
    std::cout << "  [P]                      : Pause / Resume simulation\n";
    std::cout << "  [V]                      : Toggle MP4 Video Recording (ON/OFF)\n";
    std::cout << "========================================================\n";

    // Build Shader Program
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

    // Initialize 3D Mesh Geometry and MP4 Recorder
    using namespace PhysicsEngine;
    SphereMesh sphereMesh(24, 24);
    MP4Recorder recorder;

    // Setup Physics World
    PhysicsWorld world(Vector3(0.0, -9.81, 0.0));
    populateInitialScene(world);

    const double timeStep = 0.016; // 60 FPS simulation step

    while (!glfwWindowShouldClose(window)) {
        // Handle User Input
        if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)  cameraAngle -= 0.03f;
        if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) cameraAngle += 0.03f;
        if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)    cameraHeight += 0.1f;
        if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)  cameraHeight -= 0.1f;

        // Key Press Triggers (Space, R, P, V)
        bool spacePressed = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);
        if (spacePressed && !spacePressedLast) {
            spawnRandomSphere(world);
        }
        spacePressedLast = spacePressed;

        bool rPressed = (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS);
        if (rPressed && !rPressedLast) {
            populateInitialScene(world);
        }
        rPressedLast = rPressed;

        bool pPressed = (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS);
        if (pPressed && !pPressedLast) {
            isPaused = !isPaused;
        }
        pPressedLast = pPressed;

        bool vPressed = (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS);
        if (vPressed && !vPressedLast) {
            if (!recorder.isRecording()) {
                recorder.startRecording("build/Debug/output.mp4", windowWidth, windowHeight, 60);
            } else {
                recorder.stopRecording();
            }
        }
        vPressedLast = vPressed;

        // Step Physics Simulation
        if (!isPaused) {
            world.step(timeStep);
        }

        // Render Frame
        glClearColor(0.12f, 0.12f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glUseProgram(shaderProgram);

        // Camera setup
        float camX = std::sin(cameraAngle) * cameraDistance;
        float camZ = std::cos(cameraAngle) * cameraDistance;
        glm::vec3 cameraPos(camX, cameraHeight, camZ);
        glm::vec3 cameraTarget(0.0f, 2.0f, 0.0f);
        glm::vec3 upVector(0.0f, 1.0f, 0.0f);

        glm::mat4 view = glm::lookAt(cameraPos, cameraTarget, upVector);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f), static_cast<float>(windowWidth) / windowHeight, 0.1f, 100.0f);

        glm::vec3 lightPos(5.0f, 12.0f, 5.0f);

        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "view"), 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniform3fv(glGetUniformLocation(shaderProgram, "lightPos"), 1, glm::value_ptr(lightPos));
        glUniform3fv(glGetUniformLocation(shaderProgram, "viewPos"), 1, glm::value_ptr(cameraPos));

        // Render Container Floor
        {
            glm::mat4 modelFloor = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -1.05f, 0.0f));
            modelFloor = glm::scale(modelFloor, glm::vec3(5.0f, 0.1f, 5.0f));
            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(modelFloor));
            glm::vec3 floorColor(0.35f, 0.35f, 0.4f);
            glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, glm::value_ptr(floorColor));
            sphereMesh.draw();
        }

        // Render All Spheres
        for (const auto& pm : world.getPointMasses()) {
            Vector3 pos = pm->getPosition();
            double radius = pm->getRadius();
            Vector3 col = pm->getColor();

            glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(pos.x, pos.y, pos.z));
            model = glm::scale(model, glm::vec3(radius, radius, radius));

            glUniformMatrix4fv(glGetUniformLocation(shaderProgram, "model"), 1, GL_FALSE, glm::value_ptr(model));
            glm::vec3 glColor(col.x, col.y, col.z);
            glUniform3fv(glGetUniformLocation(shaderProgram, "color"), 1, glm::value_ptr(glColor));

            sphereMesh.draw();
        }

        // Capture frame if MP4 recording is active
        if (recorder.isRecording()) {
            recorder.captureFrame();
            std::string title = "Physics Engine - Phase 1 Demo [REC: " + std::to_string(recorder.getRecordedFrames()) + " frames]";
            glfwSetWindowTitle(window, title.c_str());
        } else {
            glfwSetWindowTitle(window, "Physics Engine - Phase 1 Demo");
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    if (recorder.isRecording()) {
        recorder.stopRecording();
    }

    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}
