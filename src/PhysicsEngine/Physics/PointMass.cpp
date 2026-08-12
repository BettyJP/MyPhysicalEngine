#include "PhysicsEngine/Physics/PointMass.hpp"

namespace PhysicsEngine {

PointMass::PointMass(const Vector3& position, double mass, double radius, const Vector3& color)
    : position(position), mass(mass), radius(radius), color(color), restitution(0.8) {
    velocity = Vector3(0.0, 0.0, 0.0);
    forceAccumulator = Vector3(0.0, 0.0, 0.0);
    invMass = (mass > 0.0) ? (1.0 / mass) : 0.0;
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

double PointMass::getInverseMass() const {
    return invMass;
}

void PointMass::setMass(double mass) {
    this->mass = mass;
    this->invMass = (mass > 0.0) ? (1.0 / mass) : 0.0;
}

double PointMass::getRadius() const {
    return radius;
}

void PointMass::setRadius(double radius) {
    this->radius = radius;
}

double PointMass::getRestitution() const {
    return restitution;
}

void PointMass::setRestitution(double restitution) {
    this->restitution = restitution;
}

const Vector3& PointMass::getColor() const {
    return color;
}

void PointMass::setColor(const Vector3& color) {
    this->color = color;
}

void PointMass::addForce(const Vector3& force) {
    forceAccumulator += force;
}

void PointMass::clearForces() {
    forceAccumulator = Vector3(0.0, 0.0, 0.0);
}

const Vector3& PointMass::getAccumulatedForce() const {
    return forceAccumulator;
}

bool PointMass::isStatic() const {
    return invMass == 0.0;
}

} // namespace PhysicsEngine
