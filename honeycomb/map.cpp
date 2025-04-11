// map.cpp
#include "map.h"
#include <cmath>
#include <array>

int HoneyCombo[mapW][mapH] = {
    {1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1},
    {1,1,1,1,1,1,1,1,1,1}
};

Map::Map(Vector2 center, float radiusX, float radiusY)
    : center(center), radiusX(radiusX), radiusY(radiusY), squashFactor(0.5f) {
    CalculatePoints(NULL, NULL, NULL);
}

Map::~Map() {}

void Map::Update() {
    HandleMouseWheelInput();
    UpdateMapPosition();
}

void Map::CalculatePoints(int x, int y, bool evenNum) {
    float root3 = static_cast<float>(sqrt(3.0));

    for (int i = 0; i < 6; i++) {
        float angle = PI / 3 * i;
        float originalX{}, originalY{};

        if (evenNum && HoneyCombo[x][y] == land) {
            originalX = (x * 2 * radiusX * 3 / 4) + center.x + radiusX * cos(angle);
            originalY = (radiusY / 2 * root3) + (y * 2 * (radiusY / 2 * root3)) + center.y + radiusY * sin(angle);
        }
        else if (HoneyCombo[x][y] == land) {
            originalX = (x * 2 * radiusX * 3 / 4) + center.x + radiusX * cos(angle);
            originalY = (y * 2 * (radiusY / 2 * root3)) + center.y + radiusY * sin(angle);
        }

        // Y축 압축 비율 적용
        points[i] = {
            originalX,
            center.y + (originalY - center.y) * squashFactor
        };
    }
}

bool Map::IsPointInHexagon(Vector2 point) const {
    int intersections = 0;
    for (int i = 0; i < 6; i++) {
        Vector2 p1 = points[i];
        Vector2 p2 = points[(i + 1) % 6];

        if ((p1.y > point.y) != (p2.y > point.y)) {
            float slope = (p2.x - p1.x) / (p2.y - p1.y);
            float intersectionX = p1.x + slope * (point.y - p1.y);
            if (point.x < intersectionX) intersections++;
        }
    }
    return (intersections % 2) == 1;
}

void Map::UpdateMapPosition() {
    static Vector2 lastMousePos = { 0, 0 };
    static bool isDragging = false;

    Vector2 mousePos = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        lastMousePos = mousePos;
        isDragging = true;
    }

    if (isDragging && IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        Vector2 delta = { mousePos.x - lastMousePos.x, mousePos.y - lastMousePos.y };
        center.x += delta.x;
        center.y += delta.y;
        lastMousePos = mousePos;
    }

    if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {
        isDragging = false;
    }
}

void Map::DrawHexagon(Color color) {
    for (int i = 0; i < 6; i++) {
        DrawLineV(points[i], points[(i + 1) % 6], color);
    }
}

void Map::Draw() {
    if (IsMouseOver()) {
        DrawHexagon(RED);
    }
    else {
        DrawHexagon(BLUE);
    }
}

void Map::SetPoint() {
    bool IsEvenNumber;
    for (int x = 0; x < mapW; x++) {
        IsEvenNumber = (x % 2 == 0);
        for (int y = 0; y < mapH; y++) {
            CalculatePoints(x, y, IsEvenNumber);
            Draw();
        }
    }
}

bool Map::IsMouseOver() {
    Vector2 mousePos = GetMousePosition();
    return IsPointInHexagon(mousePos);
}

void Map::HandleMouseWheelInput() {
    float wheel = GetMouseWheelMove();
    if (wheel != 0.0f) {
        squashFactor -= wheel * 0.05f;
        // 변경된 상한/하한 적용 (최소 0.4, 최대 1.0)
        if (squashFactor < 0.4f) squashFactor = 0.4f;
        if (squashFactor > 1.0f) squashFactor = 1.0f;

    }
}

