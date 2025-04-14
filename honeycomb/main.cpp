// main.cpp
#include "raylib.h"
#include "map.h"
#include <cmath> // sqrtf ����� ���� �ʿ�
#include "Battlemap.h"



// ���� �� �𼭸��� ������ ��ġ�� �´� �߽� ��ǥ ��� �Լ�
Vector2 GetTopLeftCenter(float radiusX, float radiusY, float squashFactor = 0.3f) {
    float root3 = sqrtf(3.0f);
    float offsetY = (radiusY / 2 * root3);
    float y = offsetY * squashFactor;

    // ���⿡�� ������(+x), �Ʒ���(+y)���� �̵�
    float paddingX = 300.0f; // ������ ���� ����
    float paddingY = 200.0f;

    return { radiusX + paddingX, y + paddingY };
}

int main() {
    InitWindow(screenWidth, screenHeight, "NoName");
    SetTargetFPS(60);

    float radiusX = 200.0f;
    float radiusY = 200.0f;
    float squashFactor = 0.3f;

    Vector2 Fcenter = GetTopLeftCenter(radiusX, radiusY, squashFactor);

    Map map(Fcenter, radiusX, radiusY);
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
        map.SetPoint();
        map.Draw();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
