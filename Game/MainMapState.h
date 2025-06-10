// Game/MainMapState.h
#pragma once
#include "../Engine/Engine.h"
#include "map.h"
#include "../Engine/TurnManager.h"
#include "Castle.h"
#include "Material.h"
#include "../Engine/GameObjectManager.h" 
#include "SquirrelGen.h"
#include "SnakeGen.h"  
#include <vector>

namespace CS230 { class GameObject; }
struct HexTile;
class Castle;

class MainMapState : public CS230::GameState {
public:
    MainMapState(int screenWidth, int screenHeight);
    ~MainMapState() = default;

    void Load() override;
    void Update(double dt) override;
    void Draw() override;
    void Unload() override;
    std::string GetName() override { return "MainMapState"; }

private:
    float camera_speed = 1000.0f;
    Map gameMap;
    TurnManager turnManager;

    CS230::GameObject* player1 = nullptr;
    CS230::GameObject* player2 = nullptr;
    Stone* player1_resources;
    Stone* player2_resources;
    std::vector<Castle*> player1_castles;
    std::vector<Castle*> player2_castles;
    bool generalSelected = false;
    std::vector<HexTile> movableTiles;
    HexTile* startingTile = nullptr;
    Castle* castle1 = nullptr;
    Castle* castle2 = nullptr;
    Castle* friendlyCastle;
    Castle* enemyCastle;
    int screenWidth, screenHeight;
    float radiusX, radiusY;
};