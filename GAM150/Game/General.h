#pragma once
#include "raylib.h"

class General {
public:
    General(Vector2 position);
    ~General();

    void Draw() const;
    void Update(); // ?? »õ·Î Ãß°¡µÊ
    void SetPosition(Vector2 pos);
    Vector2 GetPosition() const;
    Vector2 GetFootPosition() const;

    bool IsMoving() const;

private:
    Vector2 position;
    Vector2 startPos;
    Vector2 targetPos;
    float progress = 0.0f;
    bool isMoving = false;

    Texture2D sprite;
};
