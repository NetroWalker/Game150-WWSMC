#pragma once
#include "raylib.h"

enum SoldierType { RANGED, TANK, MELEE, CAT };

class Soldier {
public:
    Soldier(Vector2 pos, SoldierType type, double radius = 20.0);
    ~Soldier();

    void Draw() const;
    void SetPosition(Vector2 pos);
    Vector2 GetPosition() const;

    void SetSelected(bool sel);
    bool IsSelected() const;

    double GetRadius() const;
    Rectangle GetHitbox() const;

private:
    Vector2 position;
    double radius;
    bool selected = false;
    SoldierType type;

    Texture2D sprite;
    Rectangle hitbox;  // ✅ 히트박스 멤버 추가

    void UpdateHitbox();  // ✅ 내부에서 히트박스 갱신용
};
