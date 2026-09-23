#ifndef GQUESTSERVER_VECTOR3_H
#define GQUESTSERVER_VECTOR3_H

#pragma once
#include <cmath>


//Pi def
constexpr float PI = 3.14159265358979323846f;

struct Vector3
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    Vector3() = default;
    Vector3(float x_, float y_, float z_) : x(x_), y(y_), z(z_) {}

    // Subtraction
    Vector3 operator-(const Vector3& other) const
    {
        return Vector3{ x - other.x, y - other.y, z - other.z };
    }

    // Optional but useful later
    float LengthSquared() const
    {
        return x * x + y * y + z * z;
    }

    float Length() const
    {
        return std::sqrt(LengthSquared());
    }

    Vector3 Normalized() const
    {
        float len = Length();
        if (len < 0.0001f)
            return Vector3{ 0.0f, 0.0f, 0.0f };

        float inv = 1.0f / len;
        return Vector3{ x * inv, y * inv, z * inv };
    }

    float Dot(const Vector3& other) const
    {
        return x * other.x + y * other.y + z * other.z;
    }
};

#endif //GQUESTSERVER_VECTOR3_H
