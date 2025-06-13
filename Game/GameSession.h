#pragma once
#include "../Engine/GameObjectManager.h"
#include "Material.h"
#include "Castle.h"
#include <vector>

// 전방 선언
namespace CS230 { class GameObject; }

class GameSession {
public:
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
private:
    GameSession() = default;
    ~GameSession() = default;
    GameSession(const GameSession&) = delete;
    GameSession& operator=(const GameSession&) = delete;
};