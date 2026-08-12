#pragma once
#include <iostream>
#include <cmath>

namespace PhysicsEngine {

struct Vector3 {
    double x, y, z;

    Vector3() : x(0.0), y(0.0), z(0.0) {}
    Vector3(double x, double y, double z) : x(x), y(y), z(z) {}

    Vector3 operator-() const { return Vector3(-x, -y, -z); }

    Vector3 operator+(const Vector3& other) const { return Vector3(x + other.x, y + other.y, z + other.z); }
    Vector3 operator-(const Vector3& other) const { return Vector3(x - other.x, y - other.y, z - other.z); }
    Vector3 operator*(double scalar) const { return Vector3(x * scalar, y * scalar, z * scalar); }
    Vector3 operator/(double scalar) const { return Vector3(x / scalar, y / scalar, z / scalar); }

    Vector3& operator+=(const Vector3& other) { x += other.x; y += other.y; z += other.z; return *this; }
    Vector3& operator-=(const Vector3& other) { x -= other.x; y -= other.y; z -= other.z; return *this; }
    Vector3& operator*=(double scalar) { x *= scalar; y *= scalar; z *= scalar; return *this; }
    Vector3& operator/=(double scalar) { x /= scalar; y /= scalar; z /= scalar; return *this; }

    double dot(const Vector3& other) const { return x * other.x + y * other.y + z * other.z; }
    Vector3 cross(const Vector3& other) const {
        return Vector3(
            y * other.z - z * other.y,
            z * other.x - x * other.z,
            x * other.y - y * other.x
        );
    }

    double lengthSquared() const { return x * x + y * y + z * z; }
    double length() const { return std::sqrt(lengthSquared()); }

    Vector3 normalized() const {
        double len = length();
        if (len > 1e-9) {
            return *this / len;
        }
        return Vector3(0.0, 0.0, 0.0);
    }

    friend std::ostream& operator<<(std::ostream& os, const Vector3& v) {
        os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
        return os;
    }
};

inline Vector3 operator*(double scalar, const Vector3& vec) {
    return vec * scalar;
}

} // namespace PhysicsEngine
