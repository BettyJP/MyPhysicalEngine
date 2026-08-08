#include "PhysicsEngine/Physics/PhysicsWorld.hpp"
#include "PhysicsEngine/Physics/PointMass.hpp"
#include "PhysicsEngine/Math/Vector3.hpp"
#include <cmath>

namespace PhysicsEngine {

PhysicsWorld::PhysicsWorld(const Vector3& gravity) : gravity(gravity) {}

void PhysicsWorld::addPointMass(std::unique_ptr<PointMass> pointMass) {
    pointMasses.push_back(std::move(pointMass));
}

void PhysicsWorld::step(double timeStep) {
    // 1. Update physics for all objects (Gravity and Integration)
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

    // 2. Collision Detection and Response
    for (size_t i = 0; i < pointMasses.size(); ++i) {
        for (size_t j = i + 1; j < pointMasses.size(); ++j) {
            auto& pm1 = pointMasses[i];
            auto& pm2 = pointMasses[j];

            Vector3 pos1 = pm1->getPosition();
            Vector3 pos2 = pm2->getPosition();
            
            // Calculate distance between centers
            Vector3 delta = pos2 - pos1;
            double dist = delta.length();
            double radiusSum = pm1->getRadius() + pm2->getRadius();

            if (dist < radiusSum) {
                // Collision detected!
                Vector3 normal;
                if (dist > 0.0001) {
                    normal = delta * (1.0 / dist); // Normalized direction from 1 to 2
                } else {
                    // Fallback if objects are at the exact same position
                    normal = Vector3(0, 1, 0);
                }

                // Position correction (resolve overlap)
                double overlap = radiusSum - dist;
                double totalMass = pm1->getMass() + pm2->getMass();
                
                // Weight the correction by mass so heavier objects move less.
                // Each object moves proportional to the other's mass relative to the total mass.
                Vector3 correction1 = normal * (overlap * (pm2->getMass() / totalMass));
                Vector3 correction2 = normal * (overlap * (pm1->getMass() / totalMass));

                pm1->setPosition(pos1 - correction1);
                pm2->setPosition(pos2 + correction2);

                // Velocity response (Impulse calculation)
                Vector3 relativeVelocity = pm2->getVelocity() - pm1->getVelocity();
                double velocityAlongNormal = relativeVelocity.dot(normal);

                // Only apply impulse if objects are moving towards each other
                if (velocityAlongNormal < 0) {
                    double restitution = 1.0; // Perfectly elastic collision
                    double j_val = -(1.0 + restitution) * velocityAlongNormal;
                    j_val /= (1.0 / pm1->getMass() + 1.0 / pm2->getMass());

                    Vector3 impulse = normal * j_val;
                    pm1->setVelocity(pm1->getVelocity() - impulse * (1.0 / pm1->getMass()));
                    pm2->setVelocity(pm2->getVelocity() + impulse * (1.0 / pm2->getMass()));
                }
            }
        }
    }
}

} // namespace PhysicsEngine
