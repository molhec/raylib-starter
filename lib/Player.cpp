#include "Player.h"

Player::Player()
{
    // Start on "ground"
    collider.size = {30, 50};
    transform.position = {100, 100};

    // Optional: scale remains {1,1}
}

void Player::Start()
{
    // collider.isTrigger = true;
}

void Player::HandleInput(float dt)
{
    float dirX = (IsKeyDown(KEY_RIGHT) - IsKeyDown(KEY_LEFT)) + (IsKeyDown(KEY_D) - IsKeyDown(KEY_A));
    velocity.x = dirX * moveSpeed;

    // Jump keys state
    bool pressed = IsKeyPressed(KEY_SPACE) || IsKeyPressed(KEY_W) || IsKeyPressed(KEY_UP);
    jumpHeld = IsKeyDown(KEY_SPACE) || IsKeyDown(KEY_W) || IsKeyDown(KEY_UP);
    jumpJustReleased = IsKeyReleased(KEY_SPACE) || IsKeyReleased(KEY_W) || IsKeyReleased(KEY_UP);

    // Buffer a jump press (consumed in Update when we can actually jump)
    if (pressed)
        jumpBufferTimer = jumpBufferTime;
}

void Player::Update(float dt, const CollisionWorld &world)
{
    HandleInput(dt);

    // --- timers ---
    if (coyoteTimer > 0.0f)
        coyoteTimer -= dt;
    if (jumpBufferTimer > 0.0f)
        jumpBufferTimer -= dt;

    // --- jump start (buffer + coyote) ---
    bool canJump = (isGrounded || coyoteTimer > 0.0f);
    if (jumpBufferTimer > 0.0f && canJump)
    {
        velocity.y = jumpV0; // negative = up
        isGrounded = false;
        coyoteTimer = 0.0f;
        jumpBufferTimer = 0.0f;
        jumpHoldTimer = 0.0f; // open the hold window
    }

    // --- pick gravity scale for this frame ---
    bool ascending = (velocity.y < 0.0f);
    bool holdActive = (jumpHeld && ascending && (jumpHoldTimer < maxHoldTime));

    float gScale;
    if (ascending)
    {
        gScale = holdActive ? ascendGravScale // reduced gravity while holding
                            : 1.0f;           // normal gravity while rising
    }
    else
    {
        gScale = fallMult; // faster fall
    }

    // apply gravity once, with the chosen scale
    velocity.y += gravity * gScale * dt;

    // advance hold timer if active
    if (holdActive)
    {
        jumpHoldTimer += dt;
        // Optional: see it in the console for 1s
        if (GetTime() < 1.0f)
        {
            TraceLog(LOG_INFO, "HOLD dt=%.3f  holdTimer=%.3f/%.3f  gScale=%.2f",
                     dt, jumpHoldTimer, maxHoldTime, gScale);
        }
    }

    // Early release: cut jump if still ascending
    if (jumpJustReleased && velocity.y < 0.0f)
    {
        velocity.y *= jumpCutMultiplier; // e.g., 0.35
        jumpHoldTimer = maxHoldTime;     // stop further hold effect
    }

    // Cap terminal fall (downwards only). REMOVE/raise for testing jump tuning.
    if (velocity.y > maxFall)
        velocity.y = maxFall;

    // Integrate & collide
    Vector2 delta{velocity.x * dt, velocity.y * dt};
    Vector2 applied = world.MoveAndCollide(transform, collider, delta);

    // Grounded if intended to go down but got clamped
    bool landed = (delta.y > 0.0f) && (applied.y < delta.y - 1e-5f);
    isGrounded = landed;
    if (isGrounded)
    {
        velocity.y = 0.0f;
        coyoteTimer = coyoteTime;
        jumpHoldTimer = 0.0f; // reset window on ground
    }

    // Stop horizontal if hit wall
    bool hitWall = ((delta.x > 0.0f && applied.x < delta.x - 1e-5f) ||
                    (delta.x < 0.0f && applied.x > delta.x + 1e-5f));
    if (hitWall)
        velocity.x = 0.0f;
}

void Player::Draw() const
{
    // Player body
    DrawRectangleV(transform.position, collider.size, WHITE);

    // Debug collider outline
    Rectangle r = collider.WorldRect(transform);
    DrawRectangleLines((int)r.x, (int)r.y, (int)r.width, (int)r.height, YELLOW);
}