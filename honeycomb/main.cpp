// main.cpp
#include "raylib.h"
#include "map.h"
#include "Battlemap.h"
#include <cmath>



Vector2 GetTopLeftCenter(float radiusX, float radiusY, float squashFactor = 0.3f) {
    float root3 = sqrtf(3.0f);
    float offsetY = (radiusY / 2 * root3);
    float y = offsetY * squashFactor;

 
    float paddingX = 300.0f;
    float paddingY = 200.0f;

    return { radiusX + paddingX, y + paddingY };
}

int main() {
    const int screenWidth = 1280;
    const int screenHeight = 720;
    const int lineCount = 4;

    InitWindow(screenWidth, screenHeight, "NoName");
    SetTargetFPS(60);

    //Vector2 Fcenter = { 100, 100 };
    //foat radiusY = 60.0f;

    //Honeycomb honeycomb(Fcenter, radiusX, radiusY);

    //while (!WindowShouldClose()) {
    //    BeginDrawing();
    //    ClearBackground(RAYWHITE);
    //    
    //    honeycomb.SetPoint();

    //    EndDrawing();
    //}
    //fl
    BattleMap map(screenWidth, screenHeight, lineCount);


    float radiusX = 200.0f;
    float radiusY = 200.0f;
    float squashFactor = 0.3f;

    Vector2 Fcenter = GetTopLeftCenter(radiusX, radiusY, squashFactor);

    Map map(Fcenter, radiusX, radiusY);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        map.Update();
        map.Draw();
        map.SetPoint();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
