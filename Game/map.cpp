#include "map.h"
#include <cmath>

Map::Map(Vector2 center, float radiusX, float radiusY, int width, int height, bool autoTile)
    : center(center), radiusX(radiusX), radiusY(radiusY),
    squashFactor(0.5f), autoTile(autoTile), mapW(width), mapH(height) {
    tileTexture = LoadTexture("assets/150map.png");
    tile1 = LoadTexture("Assets/Tile1.png"); // 기본 타일
}

Map::~Map() {
    UnloadTexture(tileTexture);
    UnloadTexture(tile1);
}

void Map::Update() {
    HandleMouseWheelInput();
    UpdateMapPosition();
}

void Map::SetPoint() {
    tiles.clear();

    float root3 = sqrt(3.0f);
    float xOffset = radiusX * 1.5f;
    float yOffset = radiusY * root3;

    for (int x = 0; x < mapW; x++) {
        for (int y = 0; y < mapH; y++) {
            float hexX = center.x + x * xOffset;
            float hexY = center.y + y * yOffset + ((x % 2) * (yOffset / 2.0f));
            float squashedHexY = hexY * squashFactor;

            HexTile tile = { x, y, { hexX, squashedHexY } };
            tiles.push_back(tile);
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
    // 끔: 맵 드래그 비활성
}

void Map::DrawHexagon(Color color) {
    for (int i = 0; i < 6; i++) {
        DrawLineV(points[i], points[(i + 1) % 6], color);
    }
}

void Map::Draw() {
    if (autoTile) SetPoint();

    for (const auto& tile : tiles) {
        Rectangle source = { 0, 0, (float)tileTexture.width, (float)tileTexture.height };
        Rectangle dest = {
            tile.center.x,
            tile.center.y,
            radiusX * 2.0f,
            radiusY * 2.0f * squashFactor
        };
        Vector2 origin = { radiusX, radiusY * squashFactor };

        DrawTexturePro(tile1, source, dest, origin, 0.0f, WHITE);  // 기본 돌 타일
        DrawHexagon(DARKGREEN);
    }
}

void Map::Draw(General* general) {
    if (autoTile) SetPoint();

    // 기본 타일 먼저 출력
    for (const auto& tile : tiles) {
        Rectangle source = { 0, 0, (float)tileTexture.width, (float)tileTexture.height };
        Rectangle dest = {
            tile.center.x,
            tile.center.y,
            radiusX * 2.0f,
            radiusY * 2.0f * squashFactor
        };
        Vector2 origin = { radiusX, radiusY * squashFactor };

        DrawTexturePro(tile1, source, dest, origin, 0.0f, WHITE);
        DrawHexagon(DARKGREEN);
    }

    // 장군 주변 강조
    Vector2 generalPos = general->GetFootPosition();
    HexTile* generalTile = GetTileAtPosition(generalPos);
    if (!generalTile) return;

    for (const auto& tile : tiles) {
        int dx = tile.x - generalTile->x;
        int dy = tile.y - generalTile->y;

        bool isEven = (generalTile->x % 2 == 0);
        bool isNeighbor = false;

        if ((dx == 0 && dy == 1) || (dx == 0 && dy == -1) ||
            (dx == 1 && dy == 0) || (dx == -1 && dy == 0) ||
            (dx == 1 && ((isEven && dy == -1) || (!isEven && dy == 1))) ||
            (dx == -1 && ((isEven && dy == -1) || (!isEven && dy == 1))) ||
            (dx == 0 && dy == 0)) {
            isNeighbor = true;
        }

        if (isNeighbor) {
            Rectangle source = { 0, 0, (float)tileTexture.width, (float)tileTexture.height };
            Rectangle dest = {
                tile.center.x,
                tile.center.y,
                radiusX * 2.0f,
                radiusY * 2.0f * squashFactor
            };
            Vector2 origin = { radiusX, radiusY * squashFactor };

            DrawTexturePro(tileTexture, source, dest, origin, 0.0f, WHITE);
            DrawHexagon(DARKGREEN);
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
        Vector2 screenCenter = { (float)GetScreenWidth() / 2, (float)GetScreenHeight() / 2 };
        float oldY = center.y + (screenCenter.y - center.y) * squashFactor;
        float newY = center.y + (screenCenter.y - center.y) * squashFactor;
        center.y += (oldY - newY);
    }
}

HexTile* Map::GetTileAt(int x, int y) {
    for (auto& tile : tiles) {
        if (tile.x == x && tile.y == y) return &tile;
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
