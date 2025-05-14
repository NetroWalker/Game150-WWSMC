//map.h
#pragma once
#include "raylib.h"
#include <vector>

struct HexTile {
    int x, y;
    Vector2 center;

    int stoneCount = 0;
    int woodCount = 0;

    bool hasCastle = false;
    int castleOwner = -1;  // 0: player1, 1: player2, -1: 없음
};

class Map {
public:
    Map(Vector2 center, float radiusX, float radiusY, int width, int height, bool autoTile = true);
    ~Map();

    void Update();
    void Draw();
    void SetPoint();
    bool IsMouseOver();

    const std::vector<HexTile>& GetTiles() const { return tiles; }
    HexTile* GetTileAt(int x, int y);
    HexTile* GetTileAtPosition(Vector2 pos);
    void DistributeResources();

private:
    Vector2 center;
    float radiusX;
    float radiusY;
    float squashFactor;
    bool autoTile;

    int mapW;  // 🔹 맵 가로 타일 수
    int mapH;  // 🔹 맵 세로 타일 수
    bool isMapInitialized = false;

    Vector2 points[6];
    std::vector<HexTile> tiles;

    void DrawHexagon(Color color);
    bool IsPointInHexagon(Vector2 point) const;
    void UpdateMapPosition();
    void HandleMouseWheelInput();
    Texture2D tileTexture;
    Texture2D meTexture;
    Texture2D enemeTexture;
};
