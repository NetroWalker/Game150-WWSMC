#pragma once
#include <vector>
#include "raylib.h"

static constexpr int mapW = 5;
static constexpr int mapH = 5;

struct HexTile {
    int x, y;
    Vector2 center;
};

class Map {
public:
    Map(Vector2 center, float radiusX, float radiusY);
    ~Map();

    void Update();
    void Draw();
    bool IsMouseOver();

    const std::vector<HexTile>& GetTiles() const { return tiles; }
    HexTile* GetTileAt(int x, int y);
    HexTile* GetTileAtPosition(Vector2 pos);

    // 이 메서드를 추가했습니다
    std::vector<HexTile> GetMovableTiles(HexTile* from) const;
    float GetRadiusX() const { return radiusX; }

private:
    Vector2 center;
    float   radiusX, radiusY;
    float   squashFactor;

    Vector2 points[6];
    std::vector<HexTile> tiles;

    void DrawHexagon(Color color);
    bool IsPointInHexagon(Vector2 point) const;
    void UpdateMapPosition();
    void HandleMouseWheelInput();

    Texture2D tileTexture;
};
