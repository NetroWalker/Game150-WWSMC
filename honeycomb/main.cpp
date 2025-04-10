// main.cpp
#include "raylib.h"
#include "map.h"
#include <cmath> // sqrtf 사용을 위해 필요


#define screenWidth 1500
#define screenHeight 1000

// 왼쪽 위 모서리의 육각형 위치에 맞는 중심 좌표 계산 함수
Vector2 GetTopLeftCenter(float radiusX, float radiusY, float squashFactor = 0.3f) {
    float root3 = sqrtf(3.0f);
    float offsetY = (radiusY / 2 * root3);
    float y = offsetY * squashFactor;

    // 여기에서 오른쪽(+x), 아래쪽(+y)으로 이동
    float paddingX = 300.0f; // 적당히 조정 가능
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
