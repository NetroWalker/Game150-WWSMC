//map.cpp
#include "map.h"
#include <cmath>

Map::Map(Vector2 center, float radiusX, float radiusY, int width, int height, bool autoTile)
    : center(center), radiusX(radiusX), radiusY(radiusY),
    squashFactor(0.5f), autoTile(autoTile), mapW(width), mapH(height) {
    tileTexture = LoadTexture("assets/150map.png");
    tile1 = LoadTexture("Assets/Tile1.png");
    // SetPoint()는 생성자에서 한 번만 호출하는 것이 더 효율적입니다.
    if (this->autoTile) {
        SetPoint();
    }
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

std::vector<HexTile> Map::GetMovableTiles(HexTile* from) {
    std::vector<HexTile> result;
    if (!from) return result;

    int x = from->x;
    int y = from->y;

    // 더 이상 'map' 파라미터를 받지 않고, 'this' 객체의 멤버 함수를 직접 호출합니다.
    if (x % 2 == 1) {
        int dx[] = { 0, +1, -1, -1,  0, +1 };
        int dy[] = { -1,  0,  0, +1, +1, +1 };
        for (int i = 0; i < 6; i++) {
            if (HexTile* t = this->GetTileAt(x + dx[i], y + dy[i])) { // this-> 사용
                result.push_back(*t);
            }
        }
    }
    else {
        int dx[] = { 0,  0, +1, -1, -1, +1 };
        int dy[] = { -1, +1,  0,  0, -1, -1 };
        for (int i = 0; i < 6; i++) {
            if (HexTile* t = this->GetTileAt(x + dx[i], y + dy[i])) { // this-> 사용
                result.push_back(*t);
            }
        }
    }
    return result;
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
    
}

void Map::DrawHexagon(Color color) {
    for (int i = 0; i < 6; i++) {
        DrawLineV(points[i], points[(i + 1) % 6], color);
    }
}

void Map::Draw() {
    for (const auto& tile : tiles) {
        Rectangle source = { 0, 0, (float)tile1.width, (float)tile1.height };
        Rectangle dest = {
            tile.center.x,
            tile.center.y,
            radiusX * 2.0f,
            radiusY * 2.0f * squashFactor
        };
        Vector2 origin = { radiusX, radiusY * squashFactor };

        DrawTexturePro(tile1, source, dest, origin, 0.0f, WHITE);
        // DrawHexagon(DARKGREEN); // 필요하다면 타일 외곽선 그리기
    }
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

bool Map::IsNeighborTile(int x1, int y1, int x2, int y2) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    bool isEven = (x1 % 2 == 0);

    if (dx == 0 && dy == 0) return true;
    if ((dx == 0 && (dy == 1 || dy == -1))) return true;
    if ((dx == 1 && dy == 0) || (dx == -1 && dy == 0)) return true;
    if (dx == 1 && ((isEven && dy == -1) || (!isEven && dy == 1))) return true;
    if (dx == -1 && ((isEven && dy == -1) || (!isEven && dy == 1))) return true;

    return false;
}