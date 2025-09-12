#include <iostream>
#include <raylib.h>
#include <raymath.h>

using namespace std;

float ClampValue(float value, float min, float max) {
    return fminf(fmaxf(value, min), max);
}

int main () {

    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 600;
    int ball_x = 100;
    int ball_y = 500;
    int ball_speed_x = 500;
    int ball_speed_y = 500;
    int ball_radius = 15;

    cout << "Hello World" << endl;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Bouncy Ball");
    SetTargetFPS(120);

    while (WindowShouldClose() == false){
        float dt = GetFrameTime();

        // Movement input
        int dirX = (IsKeyDown(KEY_RIGHT) - IsKeyDown(KEY_LEFT));
        //int dirY = (IsKeyDown(KEY_DOWN) - IsKeyDown(KEY_UP));

        if(IsKeyPressed(KEY_SPACE))
        {
            ball_y -= 100;
        }

        // Update position
        ball_x += dirX * ball_speed_x * dt;
        ball_y += 400 * dt;     

        // Clamp to screen edges
        ball_x = ClampValue(ball_x, ball_radius, SCREEN_WIDTH - ball_radius);
        ball_y = ClampValue(ball_y, ball_radius, SCREEN_HEIGHT - ball_radius);

        BeginDrawing();
            ClearBackground(BLACK);
            DrawCircle(ball_x,ball_y,ball_radius, WHITE);
        EndDrawing();
    }

    CloseWindow();
}