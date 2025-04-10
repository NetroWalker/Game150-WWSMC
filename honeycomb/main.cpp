// main.cpp
#include "raylib.h"
#include "map.h"
#include <cmath> // sqrtf ����� ���� �ʿ�


#define screenWidth 1500
#define screenHeight 1000

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

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        map.Update();
        map.SetPoint();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
