#pragma once
#include "raylib.h"

// ����(�ִ� 4��, ���׶��)
class Soldier {
public:
    Soldier(Vector2 pos, double radius = 20.0);
    void Draw() const;
    void SetPosition(Vector2 pos);
    Vector2 GetPosition() const;

    void SetSelected(bool sel);
    bool IsSelected() const;

private:
    Vector2 position;
    double radius;
    bool selected = false;
};