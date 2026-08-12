#pragma once
#include <vector>
#include <memory>
#include "PhysicsEngine/Math/Vector3.hpp"
#include "PhysicsEngine/Physics/PointMass.hpp"

namespace PhysicsEngine {

class PhysicsWorld {
public:
    PhysicsWorld(const Vector3& gravity = Vector3(0.0, -9.81, 0.0));

    void addPointMass(std::unique_ptr<PointMass> pointMass);
    const std::vector<std::unique_ptr<PointMass>>& getPointMasses() const;
    void clear();

    void setGravity(const Vector3& gravity);
    const Vector3& getGravity() const;

    void step(double timeStep);

private:
    void integrate(double timeStep);
    void resolveCollisions();
    void resolveEnvironmentCollisions();

    Vector3 gravity;
    std::vector<std::unique_ptr<PointMass>> pointMasses;

    // Container boundary limits (Box container)
    Vector3 containerMin = Vector3(-3.0, -1.0, -3.0);
    Vector3 containerMax = Vector3(3.0, 10.0, 3.0);
};

} // namespace PhysicsEngine
