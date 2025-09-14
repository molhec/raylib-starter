#include <iostream>
#include <raylib.h>
#include <raymath.h>
#include "lib/Player.h"
#include "lib/Platform.h"
#include "lib/physics/CollisionWorld.h"

using namespace std;

const int SCREEN_WIDTH = 800;
const int SCREEN_HEIGHT = 300;

int main()
{
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Bouncy Ball");
    SetTargetFPS(60);

    // Create GameObjects
    Player player;
    Platform platform({SCREEN_WIDTH * 0.3f, SCREEN_HEIGHT * 0.6f}, {100, 20}, RED);
    Platform platform2({SCREEN_WIDTH * 0.5f, SCREEN_HEIGHT * 0.4f}, {100, 20}, RED);
    Platform platform3({SCREEN_WIDTH * 0.7f, SCREEN_HEIGHT * 0.6f}, {100, 20}, RED);

    CollisionWorld world;
    // ground
    world.AddStatic(Rectangle{0, SCREEN_HEIGHT - 10, (float)SCREEN_WIDTH, 30});
    // left wall
    world.AddStatic(Rectangle{-30, 0, 30, (float)SCREEN_HEIGHT});
    // right wall
    world.AddStatic(Rectangle{SCREEN_WIDTH, 0, 30, (float)SCREEN_HEIGHT});

    // Start Gameobjects
    player.Start();
    platform.Start(world);
    platform2.Start(world);
    platform3.Start(world);

    while (WindowShouldClose() == false)
    {
        float dt = GetFrameTime();

        // Update Gameobjects
        player.Update(dt, world);
        // platform.Update(dt);

        BeginDrawing();
        ClearBackground(BLACK);

        // Draw GameObjects
        for (auto &p : world.GetStatics())
            DrawRectangle(p.x, p.y, p.width, p.height, GRAY);

        player.Draw();
        platform.Draw();
        platform2.Draw();
        platform3.Draw();

        DrawFPS(0, 0);
        EndDrawing();
    }

    CloseWindow();
}