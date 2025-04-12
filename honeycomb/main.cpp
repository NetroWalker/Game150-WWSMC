//main.cpp
#include "raylib.h"
#include "map.h"
#include "Battlemap.h"



int main() {
    const int screenWidth = 1280;
    const int screenHeight = 720;
    const int lineCount = 4;

    InitWindow(screenWidth, screenHeight, "Honeycomb");
    SetTargetFPS(60);

    //Vector2 Fcenter = { 100, 100 };
    //float radiusX = 60.0f;
    //float radiusY = 60.0f;

    //Honeycomb honeycomb(Fcenter, radiusX, radiusY);

    //while (!WindowShouldClose()) {
    //    BeginDrawing();
    //    ClearBackground(RAYWHITE);
    //    
    //    honeycomb.SetPoint();

    //    EndDrawing();
    //}

    BattleMap map(screenWidth, screenHeight, lineCount);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        map.Update();
        map.Draw();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}