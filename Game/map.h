//map.h
#pragma once
#include "raylib.h"
#include <vector>
#include "General.h"

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
    void Draw();                      // 기본 출력용
    void Draw(General* general);      // 장군 포함 이동 범위 강조용
    void SetPoint();
    bool IsMouseOver();

    const std::vector<HexTile>& GetTiles() const { return tiles; }
    HexTile* GetTileAt(int x, int y);
    HexTile* GetTileAtPosition(Vector2 pos);
    void DistributeResources();

    int GetMapW() const { return mapW; }
    int GetMapH() const { return mapH; }

    // --- 성 그리기에 필요한 정보 Getter 추가 ---
    float GetRadiusX() const { return radiusX; }
    float GetRadiusY() const { return radiusY; }
    float GetSquashFactor() const { return squashFactor; }
    Texture2D GetMeTexture() const { return meTexture; } // 아군 성 텍스처
    Texture2D GetEnemeTexture() const { return enemeTexture; } // 적군 성 텍스처

private:
    Vector2 center;
    float radiusX;
    float radiusY;
    float squashFactor;
    bool autoTile;


    int mapW;
    int mapH;
    bool isMapInitialized = false;


    Vector2 points[6];
    std::vector<HexTile> tiles;

    void DrawHexagon(Color color);
    bool IsPointInHexagon(Vector2 point) const;
    void UpdateMapPosition();
    void HandleMouseWheelInput();

    Texture2D tileTexture;
    Texture2D tile1;
    Texture2D meTexture;
    Texture2D enemeTexture;

};
