#pragma once
#include "PhysicsEngine/Math/Vector3.hpp"
#include <memory>

namespace PhysicsEngine {

class PointMass {
public:
    PointMass(const Vector3& position, double mass, double radius = 1.0, const Vector3& color = Vector3(1.0, 1.0, 1.0));

    const Vector3& getPosition() const;
    void setPosition(const Vector3& position);

    const Vector3& getVelocity() const;
    void setVelocity(const Vector3& velocity);

    double getMass() const;
    double getInverseMass() const;
    void setMass(double mass);

    double getRadius() const;
    void setRadius(double radius);

    double getRestitution() const;
    void setRestitution(double restitution);

    const Vector3& getColor() const;
    void setColor(const Vector3& color);

    void addForce(const Vector3& force);
    void clearForces();
    const Vector3& getAccumulatedForce() const;

    bool isStatic() const;

private:
    Vector3 position;
    Vector3 velocity;
    Vector3 forceAccumulator;
    Vector3 color;
    double mass;
    double invMass;
    double radius;
    double restitution;
};

} // namespace PhysicsEngine
