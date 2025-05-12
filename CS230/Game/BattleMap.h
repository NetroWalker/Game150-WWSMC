#pragma once
#include <vector>
#include "raylib.h"
#include "Soldier.h"

class BattleMap {
public:
    BattleMap(int screenWidth, int screenHeight, int lineCount);
    ~BattleMap();

    void Update();
    void Draw();

    float GetDistance(Vector2 a, Vector2 b);

private:
    int screenWidth;
    int screenHeight;
    int lineCount;
    int lineHeight;
    int boxWidth;
    int boxHeight;

    std::vector<Soldier> LeftBoxSoldiers;
    std::vector<Soldier> RightBoxSoldiers;
    Soldier* SelectedSoldier = nullptr;

    std::vector<bool> lineOccupied;

    Texture2D backgroundTexture;
    bool battleStarted = false;
    float attackTimer = 0.0f;
    Rectangle battleStartButton;

    std::vector<Vector2> leftInitialPositions;
    Vector2 leftAreaCenter;

    void DrawLines();
    void DrawSideUI();
};
