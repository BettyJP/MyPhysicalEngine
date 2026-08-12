#pragma once
#include <vector>
#include <glad/glad.h>

namespace PhysicsEngine {

class SphereMesh {
public:
    SphereMesh(unsigned int rings = 20, unsigned int sectors = 20);
    ~SphereMesh();

    void setupMesh();
    void draw() const;

private:
    void generateMeshData(unsigned int rings, unsigned int sectors);

    std::vector<float> vertices; // pos (3) + normal (3)
    std::vector<unsigned int> indices;

    unsigned int VAO, VBO, EBO;
    unsigned int indexCount;
};

} // namespace PhysicsEngine
