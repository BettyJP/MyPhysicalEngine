#include "PhysicsEngine/Physics/PhysicsWorld.hpp"
#include "PhysicsEngine/Physics/PointMass.hpp"
#include "PhysicsEngine/Math/Vector3.hpp"
#include <cmath>

namespace PhysicsEngine {

PhysicsWorld::PhysicsWorld(const Vector3& gravity) : gravity(gravity) {}

void PhysicsWorld::addPointMass(std::unique_ptr<PointMass> pointMass) {
    pointMasses.push_back(std::move(pointMass));
}

const std::vector<std::unique_ptr<PointMass>>& PhysicsWorld::getPointMasses() const {
    return pointMasses;
}

void PhysicsWorld::clear() {
    pointMasses.clear();
}

void PhysicsWorld::setGravity(const Vector3& gravity) {
    this->gravity = gravity;
}

const Vector3& PhysicsWorld::getGravity() const {
    return gravity;
}

void PhysicsWorld::step(double timeStep) {
    integrate(timeStep);
    resolveCollisions();
    resolveEnvironmentCollisions();
}

void PhysicsWorld::integrate(double timeStep) {
    for (auto& pm : pointMasses) {
        if (pm->isStatic()) continue;

        // Calculate total acceleration: a = g + (F / m)
        Vector3 accel = gravity + (pm->getAccumulatedForce() * pm->getInverseMass());

        // Symplectic Euler: Update velocity first, then position with new velocity
        Vector3 newVel = pm->getVelocity() + (accel * timeStep);
        pm->setVelocity(newVel);

        Vector3 newPos = pm->getPosition() + (newVel * timeStep);
        pm->setPosition(newPos);

        pm->clearForces();
    }
}

void PhysicsWorld::resolveCollisions() {
    size_t count = pointMasses.size();
    for (size_t i = 0; i < count; ++i) {
        for (size_t j = i + 1; j < count; ++j) {
            auto& pm1 = pointMasses[i];
            auto& pm2 = pointMasses[j];

            if (pm1->isStatic() && pm2->isStatic()) continue;

            Vector3 pos1 = pm1->getPosition();
            Vector3 pos2 = pm2->getPosition();

            Vector3 delta = pos2 - pos1;
            double dist = delta.length();
            double radiusSum = pm1->getRadius() + pm2->getRadius();

            if (dist < radiusSum) {
                Vector3 normal;
                if (dist > 1e-6) {
                    normal = delta / dist;
                } else {
                    normal = Vector3(0.0, 1.0, 0.0);
                }

                double overlap = radiusSum - dist;
                double invM1 = pm1->getInverseMass();
                double invM2 = pm2->getInverseMass();
                double totalInvMass = invM1 + invM2;

                if (totalInvMass > 0.0) {
                    // Position correction proportional to inverse mass
                    Vector3 corr1 = normal * (overlap * (invM1 / totalInvMass));
                    Vector3 corr2 = normal * (overlap * (invM2 / totalInvMass));

                    pm1->setPosition(pos1 - corr1);
                    pm2->setPosition(pos2 + corr2);
                }

                // Impulse-based velocity response
                Vector3 relVel = pm2->getVelocity() - pm1->getVelocity();
                double velAlongNormal = relVel.dot(normal);

                if (velAlongNormal < 0.0) {
                    double e = pm1->getRestitution() * pm2->getRestitution();
                    double j_impulse = -(1.0 + e) * velAlongNormal;
                    j_impulse /= totalInvMass;

                    Vector3 impulse = normal * j_impulse;
                    pm1->setVelocity(pm1->getVelocity() - impulse * invM1);
                    pm2->setVelocity(pm2->getVelocity() + impulse * invM2);
                }
            }
        }
    }
}

void PhysicsWorld::resolveEnvironmentCollisions() {
    const double envMinX = -2.5;
    const double envMaxX = 2.5;
    const double envMinY = -1.0;
    const double envMinZ = -2.5;
    const double envMaxZ = 2.5;

    for (auto& pm : pointMasses) {
        if (pm->isStatic()) continue;

        Vector3 pos = pm->getPosition();
        Vector3 vel = pm->getVelocity();
        double radius = pm->getRadius();
        double e = pm->getRestitution();

        // Floor (y = envMinY)
        if (pos.y - radius < envMinY) {
            pos.y = envMinY + radius;
            if (vel.y < 0.0) vel.y = -vel.y * e;
        }

        // Left Wall (x = envMinX)
        if (pos.x - radius < envMinX) {
            pos.x = envMinX + radius;
            if (vel.x < 0.0) vel.x = -vel.x * e;
        }

        // Right Wall (x = envMaxX)
        if (pos.x + radius > envMaxX) {
            pos.x = envMaxX - radius;
            if (vel.x > 0.0) vel.x = -vel.x * e;
        }

        // Back Wall (z = envMinZ)
        if (pos.z - radius < envMinZ) {
            pos.z = envMinZ + radius;
            if (vel.z < 0.0) vel.z = -vel.z * e;
        }

        // Front Wall (z = envMaxZ)
        if (pos.z + radius > envMaxZ) {
            pos.z = envMaxZ - radius;
            if (vel.z > 0.0) vel.z = -vel.z * e;
        }

        pm->setPosition(pos);
        pm->setVelocity(vel);
    }
}

} // namespace PhysicsEngine
