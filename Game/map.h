#pragma once
#include "raylib.h"
#include <vector>
#include "General.h"

struct HexTile {
    int x, y;
    Vector2 center;
};

class Map {
public:
    Map(Vector2 center, float radiusX, float radiusY, int width, int height, bool autoTile = true);
    ~Map();

    void Update();
    void Draw();                      // 기본 출력용
    void Draw(General* general);      // 장군 포함 이동 범위 강조용
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
    bool autoTile;

    int mapW;
    int mapH;

    Vector2 points[6];
    std::vector<HexTile> tiles;

    void DrawHexagon(Color color);
    bool IsPointInHexagon(Vector2 point) const;
    void UpdateMapPosition();
    void HandleMouseWheelInput();

    Texture2D tileTexture;
    Texture2D tile1;
};
