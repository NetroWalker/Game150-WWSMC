//main.cpp
#include "raylib.h"
#include "map.h"

#define screenWidth 1500
#define screenHeight 1000

int main() {
    InitWindow(screenWidth, screenHeight, "Honeycomb");
    SetTargetFPS(60);

    Vector2 Fcenter = { screenWidth / 5, screenHeight / 5 };
    float radiusX = 60.0f;
    float radiusY = 60.0f;

    Honeycomb honeycomb(Fcenter, radiusX, radiusY);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        honeycomb.SetPoint();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}