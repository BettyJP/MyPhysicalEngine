#include "PhysicsEngine\Physics\PointMass.hpp"

namespace PhysicsEngine {

PointMass::PointMass(const Vector3& position, double mass, double radius)
    : position(position), mass(mass), radius(radius) {
    velocity = Vector3(0.0, 0.0, 0.0);
}

const Vector3& PointMass::getPosition() const {
    return position;
}

void PointMass::setPosition(const Vector3& position) {
    this->position = position;
}

const Vector3& PointMass::getVelocity() const {
    return velocity;
}

void PointMass::setVelocity(const Vector3& velocity) {
    this->velocity = velocity;
}

double PointMass::getMass() const {
    return mass;
}

double PointMass::getRadius() const {
    return radius;
}

void PointMass::setRadius(double radius) {
    this->radius = radius;
}

} // namespace PhysicsEngine
