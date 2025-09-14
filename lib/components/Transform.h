#pragma once
#include <raylib.h>
#include "Component.h"

struct TransformComponent : public Component
{
    Vector2 position{0, 0};
    Vector2 scale{1, 1};
    float rotation = 0.0f;
};