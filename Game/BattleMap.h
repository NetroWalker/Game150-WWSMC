#pragma once
#include "raylib.h"
#include <vector>

struct UnitIcon {
    Texture2D texture;
    Vector2 position;
    Vector2 originalPosition;
    bool dragging = false;
    bool placed = false;
};

class BattleMap {
public:
    BattleMap(int screenWidth, int screenHeight);
    ~BattleMap();

    void Update();
    void Draw();

private:
    Texture2D background;
    Texture2D placementTileTexture;

    std::vector<UnitIcon> unitIcons;
    std::vector<Vector2> slotPositions;
    std::vector<bool> slotOccupied;

    Rectangle startButton;

    bool draggingSomething = false;
    int draggingIndex = -1;
};
