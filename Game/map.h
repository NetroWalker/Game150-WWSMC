// Game/map.h
#pragma once
#include "raylib.h"
#include <map>
#include <vector>
#include"../Engine/Engine.h"
enum class TileType {
    Grass,
    Water,
    Stone
};

struct HexTile {
    int x, y;
    Vector2 center;
    TileType type;
};

class Map {
public:
    Map(Vector2 center, float radiusX, float radiusY, int width, int height, bool autoTile = true);
    ~Map();

    void Update();
    void Draw(const std::map<HexTile*, TileType>& visionMap, const Math::TransformationMatrix& camera_matrix);

    void SetPoint();
    std::vector<HexTile> GetMovableTiles(HexTile* from);
    bool IsNeighborTile(int x1, int y1, int x2, int y2);

    const std::vector<HexTile>& GetTiles() const { return tiles; }
    HexTile* GetTileAt(int x, int y);
    HexTile* GetTileAtPosition(Vector2 pos);

private:
    Texture2D grass_tile_texture;
    Texture2D water_tile_texture;
    Texture2D stone_tile_texture;
    Texture2D hidden_tile_texture;
    Vector2 center;
    float radiusX, radiusY;
    float squashFactor;
    bool autoTile;
    int mapW, mapH;

    Vector2 points[6];
    std::vector<HexTile> tiles;

    void DrawHexagon(Color color);
    bool IsPointInHexagon(Vector2 point) const;
    void UpdateMapPosition();
    void HandleMouseWheelInput();
};