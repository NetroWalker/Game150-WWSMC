#pragma once
#include "../Engine/Engine.h"
#include "map.h"
#include "../Engine/TurnManager.h"
#include "../Engine/GameObjectManager.h"
#include <vector>
#include <set>
#include <string>

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

private:
    bool godMode = false;
    float camera_speed = 500.0f;
    Map gameMap;
    TurnManager turnManager;

    // 객체 포인터
    CS230::GameObject* player1 = nullptr;
    CS230::GameObject* player2 = nullptr;

    // 중앙 자원 관리자
    Stone* player1_resources = nullptr;
    Stone* player2_resources = nullptr;

    // 성 목록 관리
    std::vector<Castle*> player1_castles;
    std::vector<Castle*> player2_castles;

    // UI 상태 관리
    bool generalSelected = false;
    std::vector<HexTile> movableTiles;
    HexTile* startingTile = nullptr;
    std::string notification_message;
    double notification_timer = 0.0;

    int screenWidth, screenHeight;
    float radiusX, radiusY;
    static constexpr int MAX_CASTLES = 5;
};