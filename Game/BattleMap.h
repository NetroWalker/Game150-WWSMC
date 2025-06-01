#pragma once
#include "raylib.h"
#include <vector>
#include <string>
#include"../Engine/GameState.h"
#include "../Engine/TurnManager.h"

struct UnitIcon {
    Texture2D texture;
    Vector2 position;
    Vector2 originalPosition;
    bool dragging = false;
    bool placed = false;
};

class BattleMap : public CS230::GameState{
public:
    BattleMap(int screenWidth, int screenHeight);
    ~BattleMap();

    void Load()override;
    void Update(double dt)override;
    void Draw()override;
    void Unload()override;
    void LoadSoldiersForTurn(Turn turn);

private:
    Texture2D background;
    Texture2D placementTileTexture;
    Texture2D waitingZoneTexture;

    std::vector<UnitIcon> unitIcons;
    std::vector<UnitIcon> enemyIcons;

    std::vector<Vector2> slotPositions;
    std::vector<bool> slotOccupied;

    Rectangle startButton;

    bool draggingSomething = false;
    int draggingIndex = -1;

    float offsetX = 0.0f;
    bool transitioning = false;
};
