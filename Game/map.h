// Game/map.h
#pragma once
#include "raylib.h"
#include <vector>

struct HexTile {
    int x, y;
    Vector2 center;
};

class Map {
public:
    Map(Vector2 center, float radiusX, float radiusY, int width, int height, bool autoTile = true);
    ~Map();

    void Update();
    void Draw();  

    void SetPoint(); // 타일 위치를 계산하고 생성하는 함수
    std::vector<HexTile> GetMovableTiles(HexTile* from); // 멤버 함수 시그니처 수정
    bool IsNeighborTile(int x1, int y1, int x2, int y2);

    const std::vector<HexTile>& GetTiles() const { return tiles; }
    HexTile* GetTileAt(int x, int y);
    HexTile* GetTileAtPosition(Vector2 pos);

private:
    Vector2 center;
    float radiusX, radiusY;
    float squashFactor;
    bool autoTile; // 생성자에서만 사용한다면 멤버 변수가 아닐 수도 있음
    int mapW, mapH;

    Vector2 points[6]; // DrawHexagon 등에서 사용
    std::vector<HexTile> tiles;
    Texture2D tileTexture; // 특수 타일 텍스처
    Texture2D tile1;       // 기본 타일 텍스처

    void DrawHexagon(Color color); // private 헬퍼 함수
    bool IsPointInHexagon(Vector2 point) const; // private 헬퍼 함수
    void UpdateMapPosition();
    void HandleMouseWheelInput();
};