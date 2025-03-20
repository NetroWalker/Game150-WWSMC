//map.h
#ifndef MAP_H
#define MAP_H

#include "raylib.h"


#define mapH  10
#define mapW  10

#define land 1

extern int HoneyCombo[mapW][mapH];

class Honeycomb {
public:
    Honeycomb(Vector2 center, float radiusX, float radiusY);
    ~Honeycomb();
    void Draw();
    bool IsMouseOver();
    void SetPoint();

private:
    int NewX;
    int NewY;
    Vector2 center;
    float radiusX;
    float radiusY;
    Vector2 points[6];
    
    void CalculatePoints(int x, int y, bool evenNum);
    void DrawHexagon(Color color);
    bool IsPointInHexagon(Vector2 point);
    Color on = RED;
    Color off = BLUE;
};

#endif