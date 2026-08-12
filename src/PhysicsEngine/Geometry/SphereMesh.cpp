#include "PhysicsEngine/Geometry/SphereMesh.hpp"
#include <cmath>

namespace PhysicsEngine {

SphereMesh::SphereMesh(unsigned int rings, unsigned int sectors) : VAO(0), VBO(0), EBO(0), indexCount(0) {
    generateMeshData(rings, sectors);
    setupMesh();
}

SphereMesh::~SphereMesh() {
    if (VAO != 0) glDeleteVertexArrays(1, &VAO);
    if (VBO != 0) glDeleteBuffers(1, &VBO);
    if (EBO != 0) glDeleteBuffers(1, &EBO);
}

void SphereMesh::generateMeshData(unsigned int rings, unsigned int sectors) {
    vertices.clear();
    indices.clear();

    const float PI = 3.14159265358979323846f;
    float const R = 1.0f / static_cast<float>(rings);
    float const S = 1.0f / static_cast<float>(sectors);

    for (unsigned int r = 0; r <= rings; ++r) {
        float const phi = static_cast<float>(r) * PI * R;
        float const y = std::cos(phi);
        float const sinPhi = std::sin(phi);

        for (unsigned int s = 0; s <= sectors; ++s) {
            float const theta = static_cast<float>(s) * 2.0f * PI * S;
            float const x = sinPhi * std::sin(theta);
            float const z = sinPhi * std::cos(theta);

            // Position
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);

            // Normal (For unit sphere, normal equals normalized position)
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
        }
    }

    for (unsigned int r = 0; r < rings; ++r) {
        for (unsigned int s = 0; s < sectors; ++s) {
            unsigned int curRow = r * (sectors + 1);
            unsigned int nextRow = (r + 1) * (sectors + 1);

            indices.push_back(curRow + s);
            indices.push_back(nextRow + s);
            indices.push_back(nextRow + s + 1);

            indices.push_back(curRow + s);
            indices.push_back(nextRow + s + 1);
            indices.push_back(curRow + s + 1);
        }
    }

    indexCount = static_cast<unsigned int>(indices.size());
}

void SphereMesh::setupMesh() {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    // Position attribute (location = 0)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Normal attribute (location = 1)
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindVertexArray(0);
}

void SphereMesh::draw() const {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

} // namespace PhysicsEngine
