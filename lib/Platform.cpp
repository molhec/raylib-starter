#include "Platform.h"

Platform::Platform(Vector2 pos, Vector2 scale, Color color) : color(color)
{
    transform.position = pos;
    transform.scale = scale;

    collider.size = transform.scale;
}

void Platform::Start(CollisionWorld &world) const
{
    world.AddKinematic(&transform, &collider);
}

void Platform::Draw()
{
    DrawRectangle(transform.position.x, transform.position.y, transform.scale.x, transform.scale.y, color);
}