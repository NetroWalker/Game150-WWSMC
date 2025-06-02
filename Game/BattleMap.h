// Game/BattleMap.h

#pragma once
#include "raylib.h"
#include <vector>
#include <string>
#include "../Engine/GameState.h" // CS230::GameState 상속
#include "../Engine/TurnManager.h" // TurnManager 사용 (경로 확인 필요)

struct UnitIcon {
    Texture2D texture;
    Vector2 position;
    Vector2 originalPosition;
    bool dragging = false;
    bool placed = false;
};

class BattleMap : public CS230::GameState { // CS230::GameState 상속
public:
    BattleMap(int screenWidth, int screenHeight);
    ~BattleMap();

    // CS230::GameState 로부터 상속받은 순수 가상 함수들 (반드시 구현 필요)
    void Load() override;
    void Update(double dt) override;
    void Draw() override;
    void Unload() override;
    std::string GetName() override;

    // BattleMap 고유의 메소드
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
    // int screenWidth, screenHeight; // 생성자에서 초기화되므로 멤버 변수로 필요할 수 있음
};