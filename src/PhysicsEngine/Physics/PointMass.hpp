#pragma once
#include "PhysicsEngine/Math/Vector3.hpp"
#include <memory>

namespace PhysicsEngine {

class PointMass {
public:
    PointMass(const Vector3& position, double mass, double radius = 1.0);

    const Vector3& getPosition() const;
    void setPosition(const Vector3& position);

    const Vector3& getVelocity() const;
    void setVelocity(const Vector3& velocity);

    double getMass() const;
    double getRadius() const;
    void setRadius(double radius);

private:
    Vector3 position;
    Vector3 velocity;
    double mass;
    double radius;
};

} // namespace PhysicsEngine
