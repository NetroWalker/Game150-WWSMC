#pragma once
#include "../Engine/GameObjectManager.h"
#include "Material.h"
#include "Castle.h"
#include <vector>

// 전방 선언
namespace CS230 { class GameObject; }

class GameSession {
public:
    enum class GameResult { None, P1_Victory, P2_Victory };

    // ▼▼▼ 여기에 전투 타입 enum을 추가합니다 ▼▼▼
    enum class BattleType { Field, Siege };

    static GameSession& GetInstance();
    void Load();
    void Unload();

    CS230::GameObjectManager gom;
    CS230::GameObject* player1 = nullptr;
    CS230::GameObject* player2 = nullptr;
    Stone* player1_resources = nullptr;
    Stone* player2_resources = nullptr;
    std::vector<Castle*> player1_castles;
    std::vector<Castle*> player2_castles;
    GameResult last_game_result = GameResult::None;

    // ▼▼▼ 여기에 전투 상태 저장 변수를 추가합니다 ▼▼▼
    BattleType current_battle_type = BattleType::Field;
    Castle* castle_under_siege = nullptr;

private:
    GameSession() = default;
    ~GameSession() = default;
    GameSession(const GameSession&) = delete;
    GameSession& operator=(const GameSession&) = delete;
};