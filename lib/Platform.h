#pragma once
#include <raylib.h>
#include "physics/CollisionWorld.h"
#include "components/Transform.h"

class Platform
{
public:
    Platform(Vector2 pos, Vector2 scale, Color color);

    void Start(CollisionWorld &world) const;
    void Update(float dt, const CollisionWorld &world);
    void Draw();

    const TransformComponent &GetTransform() const { return transform; }
    const AABB &GetCollider() const { return collider; }

private:
    // Components
    TransformComponent transform;
    AABB collider;

    Color color;
};