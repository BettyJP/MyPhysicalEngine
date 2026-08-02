#include "PhysicsEngine/Physics/PhysicsWorld.hpp"

namespace PhysicsEngine {

PhysicsWorld::PhysicsWorld(const Vector3& gravity) : gravity(gravity) {}

void PhysicsWorld::addPointMass(std::unique_ptr<PointMass> pointMass) {
    pointMasses.push_back(std::move(pointMass));
}

void PhysicsWorld::step(double timeStep) {
    for (auto& pm : pointMasses) {
        // Update velocity: v = v + g * dt
        Vector3 currentVelocity = pm->getVelocity();
        Vector3 acceleration = gravity; // Simple gravity
        Vector3 newVelocity = currentVelocity + (acceleration * timeStep);
        pm->setVelocity(newVelocity);

        // Update position: p = p + v * dt
        Vector3 currentPosition = pm->getPosition();
        Vector3 newPosition = currentPosition + (newVelocity * timeStep);
        pm->setPosition(newPosition);
    }
}

} // namespace PhysicsEngine
