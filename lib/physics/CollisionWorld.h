// lib/physics/CollisionWorld.h
#pragma once
#include <vector>
#include <raylib.h>
#include "../components/Transform.h"
#include "../components/AABB.h"

class CollisionWorld
{
public:
    // ---- Static colliders: immutable copies ----
    void AddStatic(const Rectangle &r) { statics.push_back(r); }
    const std::vector<Rectangle> &GetStatics() const { return statics; }

    // ---- Kinematic colliders: live references (no replacing) ----
    struct ColliderRef
    {
        const TransformComponent *t;
        const AABB *a;
    };

    // Returns a handle (index) if you want to remove later; can ignore for now
    int AddKinematic(const TransformComponent *t, const AABB *a)
    {
        kinematics.push_back({t, a});
        return (int)kinematics.size() - 1;
    }

    // Main collision resolve: checks statics + kinematics
    Vector2 MoveAndCollide(TransformComponent &tr, const AABB &col, Vector2 desiredDelta) const
    {
        Vector2 applied{desiredDelta.x, desiredDelta.y};

        auto collideAxis = [&](bool xAxis)
        {
            if ((xAxis ? desiredDelta.x : desiredDelta.y) == 0.0f)
                return;

            if (xAxis)
                tr.position.x += desiredDelta.x;
            else
                tr.position.y += desiredDelta.y;

            Rectangle wr = col.WorldRect(tr);

            auto resolveAgainst = [&](const Rectangle &s)
            {
                if (CheckCollisionRecs(wr, s))
                {
                    if (xAxis)
                    {
                        if (desiredDelta.x > 0)
                            tr.position.x = s.x - col.size.x;
                        else
                            tr.position.x = s.x + s.width;
                        applied.x = tr.position.x - (wr.x - desiredDelta.x);
                    }
                    else
                    {
                        if (desiredDelta.y > 0)
                            tr.position.y = s.y - col.size.y;
                        else
                            tr.position.y = s.y + s.height;
                        applied.y = tr.position.y - (wr.y - desiredDelta.y);
                    }
                    wr = col.WorldRect(tr);
                }
            };

            // Statics (copied rects)
            for (const Rectangle &s : statics)
                resolveAgainst(s);

            // Kinematics (live rects from refs)
            for (const auto &ref : kinematics)
            {
                Rectangle s = ref.a->WorldRect(*ref.t);
                resolveAgainst(s);
            }
        };

        collideAxis(true);  // X
        collideAxis(false); // Y
        return applied;
    }

private:
    std::vector<Rectangle> statics;
    std::vector<ColliderRef> kinematics;
};