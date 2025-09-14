#pragma once
#include <raylib.h>
#include "Component.h"
#include "Transform.h"

// Axis-Aligned Bounding Box component (in world space)
struct AABB : public Component
{
    // Size of the collider (width/height) in pixels
    Vector2 size{32, 32};

    // Optional: physics flags
    bool isTrigger = false; // if true → detect but don't resolve
    int layer = 1;          // bitmask layer
    int mask = 0xFFFF;      // what it collides with

    // Convenience: compute the world-space rectangle from a Transform
    Rectangle WorldRect(const TransformComponent &t) const
    {
        return Rectangle{t.position.x, t.position.y, size.x, size.y};
    }
};