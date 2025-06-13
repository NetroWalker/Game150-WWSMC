//map.cpp
#include "map.h"
#include <cmath>
#include "States.h"

Map::Map(Vector2 center, float radiusX, float radiusY, int width, int height, bool autoTile)
    : center(center), radiusX(radiusX), radiusY(radiusY),
    squashFactor(0.5f), autoTile(autoTile), mapW(width), mapH(height) {
    grass_tile_texture = LoadTexture("assets/TileGrass.png");
    water_tile_texture = LoadTexture("assets/TileWater.png");
    stone_tile_texture = LoadTexture("Assets/TileStone.png");
    hidden_tile_texture = LoadTexture("Assets/TileHidden.png");
    if (this->autoTile) {
        SetPoint();
    }
}

Map::~Map() {
    UnloadTexture(grass_tile_texture);
    UnloadTexture(water_tile_texture);
    UnloadTexture(stone_tile_texture);
    UnloadTexture(hidden_tile_texture);
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
    const float MAX_WATER_TILES_PERCENTAGE = 0.25f;
    int maxWaterTiles = static_cast<int>((mapW * mapH) * MAX_WATER_TILES_PERCENTAGE);

    int currentWaterCount = 0;
    for (int x = 0; x < mapW; x++) {
        for (int y = 0; y < mapH; y++) {
            float hexX = center.x + x * xOffset;
            float hexY = center.y + y * yOffset + ((x % 2) * (yOffset / 2.0f));
            float squashedHexY = hexY * squashFactor;

            TileType generated_type = TileType::Grass; // 기본값은 풀 타일

            // 약 20% 확률로 물 타일 생성 시도
            if ((rand() % 5) == 0) {
                // 단, 최대 물 타일 개수를 넘지 않았을 경우에만 생성
                if (currentWaterCount < maxWaterTiles) {
                    generated_type = TileType::Water;
                    currentWaterCount++; // 물 타일 카운터 증가
                }
            }

            HexTile tile = { x, y, { hexX, squashedHexY }, generated_type };
            tiles.push_back(tile);
        }
    }
}

// in map.cpp
// in map.cpp

// [수정] GetAllNeighbors 함수를 아래 코드로 교체합니다.
// 이웃 좌표 계산 오류를 수정하고 코드를 단순화했습니다.
std::vector<HexTile> Map::GetAllNeighbors(HexTile* from) {
    std::vector<HexTile> result;
    if (!from) return result;

    // 육각 타일의 6방향 이웃 오프셋을 정의합니다.
    // 홀수 열과 짝수 열에 따라 y 오프셋이 달라집니다.
    int dx[] = { 0,  1, 1, 0, -1, -1 };
    int dy_even[] = { -1, -1, 0, 1,  0, -1 }; // 짝수 열(x)의 y 오프셋
    int dy_odd[] = { -1,  0, 1, 1,  1,  0 }; // 홀수 열(x)의 y 오프셋

    bool is_odd_col = (from->x % 2 != 0);
    const int* dy = is_odd_col ? dy_odd : dy_even;

    for (int i = 0; i < 6; i++) {
        int neighbor_x = from->x + dx[i];
        int neighbor_y = from->y + dy[i];

        if (HexTile* t = this->GetTileAt(neighbor_x, neighbor_y)) {
            result.push_back(*t);
        }
    }
    return result;
}


// [수정] GetMovableTiles 함수를 아래 코드로 교체합니다.
// 이제 이 함수는 수정된 GetAllNeighbors를 호출한 뒤, 물 타일만 걸러냅니다.
std::vector<HexTile> Map::GetMovableTiles(HexTile* from) {
    // 1. 먼저 모든 이웃 타일을 가져옵니다.
    std::vector<HexTile> all_neighbors = GetAllNeighbors(from);
    std::vector<HexTile> movable_neighbors;

    // 2. 그 중에서 물 타일이 아닌 것만 골라냅니다.
    for (const auto& tile : all_neighbors) {
        if (tile.type != TileType::Water) {
            movable_neighbors.push_back(tile);
        }
    }
    return movable_neighbors;
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

void Map::Draw(const std::map<HexTile*, TileType>& visionMap, const Math::TransformationMatrix& camera_matrix) {
    const int screen_height = GetScreenHeight();

    for (const auto& tile_to_draw : tiles) {
        Texture2D texture_to_use;
        auto it = visionMap.find(const_cast<HexTile*>(&tile_to_draw));
        // ===== 수정된 시야 확인 로직 =====
        // 현재 그리려는 타일이 visibleTiles 목록에 포함되어 있는지 확인합니다.
        // C++20 부터는 set.contains(&tile_to_draw) 를 사용할 수 있습니다.
        if (it != visionMap.end()) {
            // 2. 시야 지도에 있다면, 지도에 지정된 타입으로 텍스처 선택
            TileType type_to_draw = it->second;
            switch (type_to_draw) {
            case TileType::Grass: texture_to_use = grass_tile_texture; break;
            case TileType::Water: texture_to_use = water_tile_texture; break;
            case TileType::Stone: texture_to_use = stone_tile_texture; break;
            }
        }
        else {
            // 3. 시야 지도에 없다면, 안 보이는 타일 텍스처 사용
            texture_to_use = hidden_tile_texture;
        }
        Rectangle source = { 0, 0, (float)texture_to_use.width, (float)texture_to_use.height };
        Math::vec2 world_pos(tile_to_draw.center);
        Math::vec2 transformed_pos = camera_matrix * world_pos;
        float final_screen_y = -transformed_pos.y + screen_height;
        Rectangle dest = { (float)transformed_pos.x, final_screen_y, radiusX * 2.0f, radiusY * 2.0f * squashFactor };
        Vector2 origin = { radiusX, radiusY * squashFactor };
        DrawTexturePro(texture_to_use, source, dest, origin, 0.0f, WHITE);
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