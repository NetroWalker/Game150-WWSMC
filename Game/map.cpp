//map.cpp
#include "map.h"
#include <cmath>

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

    for (int x = 0; x < mapW; x++) {
        for (int y = 0; y < mapH; y++) {
            float hexX = center.x + x * xOffset;
            float hexY = center.y + y * yOffset + ((x % 2) * (yOffset / 2.0f));
            float squashedHexY = hexY * squashFactor;

            int random_type_index = rand() % 2;
            TileType random_type = static_cast<TileType>(random_type_index);

            HexTile tile = { x, y, { hexX, squashedHexY }, random_type };
            tiles.push_back(tile);
        }
    }
}

std::vector<HexTile> Map::GetMovableTiles(HexTile* from) {
    std::vector<HexTile> result;
    if (!from) return result;

    int x = from->x;
    int y = from->y;

    if (x % 2 == 1) {
        int dx[] = { 0, +1, -1, -1,  0, +1 };
        int dy[] = { -1,  0,  0, +1, +1, +1 };
        for (int i = 0; i < 6; i++) {
            if (HexTile* t = this->GetTileAt(x + dx[i], y + dy[i])) {
                result.push_back(*t);
            }
        }
    }
    else {
        int dx[] = { 0,  0, +1, -1, -1, +1 };
        int dy[] = { -1, +1,  0,  0, -1, -1 };
        for (int i = 0; i < 6; i++) {
            if (HexTile* t = this->GetTileAt(x + dx[i], y + dy[i])) {
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

void Map::Draw(HexTile* vision_center_tile, const Math::TransformationMatrix& camera_matrix) {
    const int screen_height = GetScreenHeight();

    for (const auto& tile_to_draw : tiles) {
        Texture2D texture_to_use = hidden_tile_texture;

        if (vision_center_tile != nullptr) {
            if (IsNeighborTile(vision_center_tile->x, vision_center_tile->y, tile_to_draw.x, tile_to_draw.y)) {
                texture_to_use = grass_tile_texture;
            }
        }

        Rectangle source = { 0, 0, (float)texture_to_use.width, (float)texture_to_use.height };

        Math::vec2 world_pos(tile_to_draw.center);

        Math::vec2 transformed_pos = camera_matrix * world_pos;
        float final_screen_y = -transformed_pos.y + screen_height;

        Rectangle dest = {
            (float)transformed_pos.x, 
            final_screen_y,           
            radiusX * 2.0f,
            radiusY * 2.0f * squashFactor
        };
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