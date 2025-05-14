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

    int GetAtk() const;
    void ReceiveAttack(int damage);

    float GetSpeed() const;
    double GetRadius() const;

    int GetHP() const;

    void ResetHP();

private:
    Vector2 position;
    double radius;
    bool selected = false;
    SoldierType type;

    Texture2D sprite;
    int hp;
    float speed;
    int atk;

    mutable bool flash = false;
};
