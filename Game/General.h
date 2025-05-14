//general.h
#pragma once
#include "raylib.h"
#include "map.h"

class General {
public:
    General(Vector2 position, const char* text_path);
    ~General();

    void Draw() const;
    void Update(); // ?? »õ·Î Ãß°¡µÊ
    void SetPosition(Vector2 pos);
    Vector2 GetPosition() const;
    Vector2 GetFootPosition() const;

    bool IsMoving() const;

    int GetWood() const { return wood; }
    int GetStone() const { return stone; }
    bool CanBuildCastle() const { return wood >= 10 && stone >= 10; }
    void UseResourcesForCastle() { wood -= 10; stone -= 10; }
    void CollectResources(HexTile* tile);
private:
    int wood = 0;
    int stone = 0;

    Vector2 position;
    Vector2 startPos;
    Vector2 targetPos;
    float progress = 0.0f;
    bool isMoving = false;

    Texture2D sprite;
};
