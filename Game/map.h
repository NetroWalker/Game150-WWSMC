#pragma once
#include "raylib.h"
#include <vector>

const int mapW = 5;
const int mapH = 5;

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
    void SetPoint();
    bool IsMouseOver();

    const std::vector<HexTile>& GetTiles() const { return tiles; }
    HexTile* GetTileAt(int x, int y);
    HexTile* GetTileAtPosition(Vector2 pos);

private:
    Vector2 center;
    float radiusX;
    float radiusY;
    float squashFactor;

    Vector2 points[6];
    std::vector<HexTile> tiles;

    void DrawHexagon(Color color);
    bool IsPointInHexagon(Vector2 point) const;
    void UpdateMapPosition();
    void HandleMouseWheelInput();
    Texture2D tileTexture;
};
