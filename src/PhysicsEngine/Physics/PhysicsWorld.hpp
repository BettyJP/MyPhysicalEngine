#pragma once
#include <vector>
#include <memory>
#include "PhysicsEngine/Math/Vector3.hpp"
#include "PhysicsEngine/Physics/PointMass.hpp"

namespace PhysicsEngine {

class PhysicsWorld {
public:
    PhysicsWorld(const Vector3& gravity);
    void addPointMass(std::unique_ptr<PointMass> pointMass);
    void step(double timeStep);

private:
    Vector3 gravity;
    std::vector<std::unique_ptr<PointMass>> pointMasses;
};

} // namespace PhysicsEngine
