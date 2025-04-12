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
    Soldier* SelectedSoldier = nullptr; // ���� ���� ����

    std::vector<bool> lineOccupied; // ���� ���� ����. ��ġ�� �� �Ǵϱ�

    void DrawLines();
    void DrawSideUI();
    float GetDistance(Vector2 a, Vector2 b);
};