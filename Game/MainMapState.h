// Game/MainMapState.h
#pragma once
#include "../Engine/Engine.h"
#include "map.h"
#include "../Engine/TurnManager.h"
#include "../Engine/GameObjectManager.h" // GameObjectManager 포함
#include "SquirrelGen.h" // 구체적인 타입 생성을 위해 포함
#include "SnakeGen.h"  // 구체적인 타입 생성을 위해 포함
#include <vector>

namespace CS230 { class GameObject; }
struct HexTile;

class MainMapState : public CS230::GameState {
public:
    MainMapState(int screenWidth, int screenHeight);
    ~MainMapState() = default; // GameObjectManager가 컴포넌트로 관리되므로 특별한 소멸 로직 불필요

    void Load() override;
    void Update(double dt) override;
    void Draw() override;
    void Unload() override;
    std::string GetName() override { return "MainMapState"; }

private:
    float camera_speed = 500.0f;
    Map gameMap;
    TurnManager turnManager;

    // 플레이어들을 직접 가리키는 포인터 (편의상)
    CS230::GameObject* player1 = nullptr; // SquirrelGen
    CS230::GameObject* player2 = nullptr; // SnakeGen

    bool generalSelected = false;
    std::vector<HexTile> movableTiles;
    HexTile* startingTile = nullptr;
    int screenWidth, screenHeight;
    float radiusX, radiusY;
};