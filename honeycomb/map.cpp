//map.cpp
#include "map.h"
#include <cmath>
#include <array>

int HoneyCombo[mapW][mapH] = {
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0},
    {0,1,1,1,1,1,1,1,0,0},
    {0,1,1,1,1,1,1,1,0,0},
    {0,1,1,1,1,1,1,1,0,0},
    {0,1,1,1,1,1,1,1,0,0},
    {0,1,1,1,1,1,1,1,0,0},
    {0,1,1,1,1,1,1,1,0,0},
    {0,0,0,0,0,0,0,0,0,0},
    {0,0,0,0,1,0,0,0,0,0}
};

Honeycomb::Honeycomb(Vector2 center, float radiusX, float radiusY)
    : center(center), radiusX(radiusX), radiusY(radiusY) {
    CalculatePoints(NULL,NULL,NULL);
}

Honeycomb::~Honeycomb() {}

//void Honeycomb::SetPoint() {
//    for (int x = 0; x < mapW; x++) {
//        for (int y = 0; y < mapH; y++) {
//
//            NewX = center.x + radiusX * 2 * x;
//            NewY = center.y + radiusY * 2 * y;
//
//        }
//
//    }
//}
void Honeycomb::CalculatePoints(int x,int y,bool evenNum) {
    float root3 = sqrt(3.0);
    
    for (int i = 0; i < 6; i++) {
        float angle = PI / 3 * i;

        
        if (evenNum == true && HoneyCombo[x][y]== land) {
            points[i] = {
           (x * 2 * radiusX * 3 / 4) + center.x + radiusX * cos(angle),
           (radiusY / 2 * root3) + (y * 2 * (radiusY / 2 * root3)) + center.y + radiusY * sin(angle)

            };
        }
        else if(HoneyCombo[x][y] == land) {
            points[i] = {
            (x * 2 * radiusX * 3 / 4) + center.x + radiusX * cos(angle),
            (y * 2 * (radiusY / 2 * root3)) + center.y + radiusY * sin(angle)

            };
        }
        
    }
    
}



bool Honeycomb::IsPointInHexagon(Vector2 point) {
    int intersections = 0;
    for (int i = 0; i < 6; i++) {
        Vector2 p1 = points[i];
        Vector2 p2 = points[(i + 1) % 6];

        if ((p1.y > point.y) != (p2.y > point.y)) {
            float slope = (p2.x - p1.x) / (p2.y - p1.y);
            float intersectionX = p1.x + slope * (point.y - p1.y);
            if (point.x < intersectionX) intersections++;
        }
    }
    return (intersections % 2) == 1;
}
void Honeycomb::DrawHexagon(Color color) {
    for (int i = 0; i < 6; i++) {
        DrawLineV(points[i], points[(i + 1) % 6], color);
    }
}
void Honeycomb::Draw() {
    
    if (IsMouseOver()) {
        DrawHexagon(RED); //마우스 올라갔을때
    }
    else {
        DrawHexagon(BLUE); //마우스 안올라갔을때
    }
}
void Honeycomb::SetPoint() {
    bool IsEvenNumber;
    
    for (int x = 0; x < mapW; x++) {

        if (x % 2 == 0) {
            IsEvenNumber = true;
        }
        else IsEvenNumber = false;
        for(int y = 0; y < mapH; y++){
            

            CalculatePoints(x,y , IsEvenNumber);

            Draw();
            DrawPoly(center, 6, 60, 0, SKYBLUE);

        }
    }


}

bool Honeycomb::IsMouseOver() {
    Vector2 mousePos = GetMousePosition();
    return IsPointInHexagon(mousePos);
}