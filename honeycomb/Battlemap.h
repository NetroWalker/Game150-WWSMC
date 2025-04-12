#pragma once
#include <vector>
#include "raylib.h"
#include "Soldier.h"

struct SoldierSlot {
    Vector2 position;
    double radius = 20.0;
};

class BattleMap {
public:
    BattleMap(int screenWidth, int screenHeight, int lineCount);
    void Update();
    void Draw();


private:
    int screenWidth;
    int screenHeight;
    int lineCount;
    int lineHeight;
    int boxWidth;
    int boxHeight;

    std::vector<Soldier> LeftBoxSoldiers;
    Soldier* SelectedSoldier = nullptr; // 병사 선택 유무

    std::vector<bool> lineOccupied; // 라인 병사 유무. 겹치면 안 되니까

    void DrawLines();
    void DrawSideUI();
    float GetDistance(Vector2 a, Vector2 b);
};