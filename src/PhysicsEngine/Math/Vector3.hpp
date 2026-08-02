#pragma once
#include <iostream>
#include <cmath>

namespace PhysicsEngine {

struct Vector3 {
    double x, y, z;

    Vector3() : x(0.0), y(0.0), z(0.0) {}
    Vector3(double x, double y, double z) : x(x), y(y), z(z) {}

    Vector3 operator+(const Vector3& other) const { return Vector3(x + other.x, y + other.y, z + other.z); }
    Vector3 operator-(const Vector3& other) const { return Vector3(x - other.x, y - other.y, z - other.z); }
    Vector3 operator*(double scalar) const { return Vector3(x * scalar, y * scalar, z * scalar); }
    
    double dot(const Vector3& other) const { return x * other.x + y * other.y + z * other.z; }
    double length() const { return std::sqrt(x * x + y * y + z * z); }
};

} // namespace PhysicsEngine
