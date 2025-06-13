#pragma once
#include "../Engine/Engine.h"
#include "map.h"
#include "../Engine/TurnManager.h"
#include "../Engine/GameObjectManager.h"
#include <vector>
#include <set>
#include "States.h" 
#include <string>
#include "../Engine/TurnManager.h" 

// 전방 선언
namespace CS230 { class GameObject; }
class SquirrelGen;
class SnakeGen;
class Castle;
class Stone;
struct HexTile;

class MainMapState : public CS230::GameState {
public:
    MainMapState(int screenWidth, int screenHeight);
    ~MainMapState() override;

    void Load() override;
    void Update(double dt) override;
    void Draw() override;
    void Unload() override;
    std::string GetName() override { return "MainMapState"; }

    void SetBattleOutcome(BattleOutcome outcome);
private:
    void HandleBattleAftermath();
    bool godMode = false;
    float camera_speed = 500.0f;
    Map gameMap;
    TurnManager turnManager;

    // UI 상태 관리
    bool generalSelected = false;
    std::vector<HexTile> movableTiles;
    HexTile* startingTile = nullptr;
    std::string notification_message;
    double notification_timer = 0.0;

    int screenWidth, screenHeight;
    float radiusX, radiusY;
    static constexpr int MAX_CASTLES = 5;
    static constexpr int MIN_CASRLES = 1;
    bool battle_ended = false;
    BattleOutcome battle_outcome;
    bool victory = false;
};