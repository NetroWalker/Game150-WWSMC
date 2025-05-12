// map.cpp
#include "map.h"
#include <cmath>

Map::Map(Vector2 center, float radiusX, float radiusY)
    : center(center), radiusX(radiusX), radiusY(radiusY), squashFactor(0.5f) {
    tileTexture = LoadTexture("assets/150map.png");
}

Map::~Map() {
    UnloadTexture(tileTexture);
}

void Map::Update() {
    HandleMouseWheelInput();
    UpdateMapPosition();
}
                              
void Map::SetPoint() {
    tiles.clear(); // 중복 저장 방지

    float root3 = sqrt(3.0f);
    float xOffset = radiusX * 1.5f;
    float yOffset = radiusY * root3;

    for (int x = 0; x < mapW; x++) {
        for (int y = 0; y < mapH; y++) {
            float hexX = center.x + x * xOffset;
            float hexY = center.y + y * yOffset + ((x % 2) * (yOffset / 2.0f));

            // squash 기준 변경: center.y 제거 → 화면에 더 위쪽에 뜸
            float squashedHexY = hexY * squashFactor;

            // 타일 정보 저장
            HexTile tile;
            tile.x = x;
            tile.y = y;
            tile.center = { hexX, squashedHexY };
            tiles.push_back(tile);

            // 꼭짓점 계산 (그리기용)
            for (int i = 0; i < 6; i++) {
                float angle = PI / 3 * i;
                float px = hexX + radiusX * cosf(angle);
                float py = hexY + radiusY * sinf(angle);

                points[i] = {
                    px,
                    py * squashFactor // 여기도 동일하게 squash 적용
                };
            }

            Rectangle source = { 0, 0, (float)tileTexture.width, (float)tileTexture.height };
            Rectangle dest = {
                hexX,
                squashedHexY,
                radiusX * 2.0f,
                radiusY * 2.0f * squashFactor
            };
            Vector2 origin = { radiusX, radiusY * squashFactor };

            DrawTexturePro(tileTexture, source, dest, origin, 0.0f, WHITE);
            DrawHexagon(DARKGREEN);
        }
    }
}



bool Map::IsPointInHexagon(Vector2 point) const {
    int intersections = 0;
    for (int i = 0; i < 6; i++) {
        Vector2 p1 = points[i];
        Vector2 p2 = points[(i + 1) % 6];

        if ((p1.y > point.y) != (p2.y > point.y)) {
            float slope = (p2.x - p1.x) / (p2.y - p1.y);
            float intersectX = p1.x + slope * (point.y - p1.y);
            if (point.x < intersectX) intersections++;
        }
    }
    return (intersections % 2) == 1;
}

void Map::UpdateMapPosition() {
   /* static Vector2 lastMousePos = {0, 0};
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
    }*/
}

void Map::DrawHexagon(Color color) {
    for (int i = 0; i < 6; i++) {
        DrawLineV(points[i], points[(i + 1) % 6], color);
    }
}

void Map::Draw() {
    SetPoint(); 
}

bool Map::IsMouseOver() {
    Vector2 mousePos = GetMousePosition();
    return IsPointInHexagon(mousePos); 
}

void Map::HandleMouseWheelInput() {
    float wheel = GetMouseWheelMove();
    if (wheel != 0.0f) {
        Vector2 screenCenter = { (float)GetScreenWidth() / 2, (float)GetScreenHeight() / 2 };

        // squashFactor 변경 전의 기준 y 계산
        float oldY = center.y + (screenCenter.y - center.y) * squashFactor;

        //float oldSquash = squashFactor;
        //squashFactor -= wheel * 0.05f;
        //if (squashFactor < 0.5f) squashFactor = 0.5f;
        //if (squashFactor > 1.0f) squashFactor = 1.0f;

        // squashFactor 변경 후의 기준 y 계산
        float newY = center.y + (screenCenter.y - center.y) * squashFactor;

        // 변경 전후의 차이만큼 center를 반대로 이동
        center.y += (oldY - newY);
    }
}


HexTile* Map::GetTileAt(int x, int y) {
    for (auto& tile : tiles) {
        if (tile.x == x && tile.y == y) {
            return &tile;
        }
    }
    return nullptr;
}

HexTile* Map::GetTileAtPosition(Vector2 pos) {
    for (auto& tile : tiles) {
        float dx = pos.x - tile.center.x;
        float dy = pos.y - tile.center.y;
        float dist = sqrtf(dx * dx + dy * dy);
        if (dist <= radiusX * 0.9f) {
            return &tile;
        }
    }
    return nullptr;
}


