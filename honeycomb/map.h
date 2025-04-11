// map.h

#pragma once
#include "raylib.h"

const int mapW = 10;
const int mapH = 10;
const int land = 1;

class Map {
public:
    Map(Vector2 center, float radiusX, float radiusY);
    ~Map();

    void Update();
    void Draw();
    void SetPoint();
    bool IsMouseOver();

private:
    Vector2 center;
    float radiusX;
    float radiusY;
    float squashFactor;

    Vector2 points[6];

    void CalculatePoints(int x, int y, bool evenNum);
    bool IsPointInHexagon(Vector2 point) const;
    void UpdateMapPosition();
    void DrawHexagon(Color color);
    void HandleMouseWheelInput();
};
