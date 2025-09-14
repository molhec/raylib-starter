#pragma once
#include <raylib.h>
#include "components/Transform.h"
#include "components/AABB.h"
#include "physics/CollisionWorld.h"

class Player
{
public:
    Player();

    void Start();
    void Update(float dt, const CollisionWorld &world);
    void Draw() const;

    // Optional getters
    const TransformComponent &GetTransform() const { return transform; }
    const AABB &GetCollider() const { return collider; }

private:
    // Components
    TransformComponent transform;
    AABB collider;

    const float moveSpeed = 300;

    // Quality-of-life (optional but recommended)
    float coyoteTime = 0.08f;     // grace period after leaving ground
    float jumpBufferTime = 0.12f; // buffer a jump press just before ground

    float gravity = 2200.0f; // px/s^2
    float jumpV0 = -600.0f;  // px/s   (upward is negative)
    float fallMult = 1.7f;   // faster fall
    float maxFall = 1800.0f; // terminal fall

    float maxHoldTime = 0.22f;       // seconds
    float ascendGravScale = 0.38f;   // 0.3..0.5 typical
    float jumpCutMultiplier = 0.35f; // 0..1 (smaller = shorter quick-tap)

    // ---- Runtime state ----
    bool isGrounded = false;
    Vector2 velocity{0, 0};

    bool jumpHeld = false;
    bool jumpJustReleased = false;
    float jumpHoldTimer = 0.0f;
    float coyoteTimer = 0.0f;
    float jumpBufferTimer = 0.0f;

    void HandleInput(float dt);
};