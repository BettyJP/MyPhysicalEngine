#include "PhysicsEngine/Physics/PointMass.hpp"

namespace PhysicsEngine {

PointMass::PointMass(const Vector3& position, double mass) 
    : position(position), velocity(Vector3(0, 0, 0)), mass(mass) {}

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

} // namespace PhysicsEngine
