// Game/MainMapState.h
#pragma once
#include "../Engine/Engine.h" // For GameState and Engine instance
#include "map.h"
#include "General.h"
#include "../Engine/TurnManager.h" // Assuming TurnManager is part of the engine files you provided
#include "States.h" // For your GameState enum (to map to GSmachine indices)
#include <vector>

// Define indices for GameStateManager (adjust as needed based on add order)
const int BATTLE_MAP_STATE_INDEX = 3; // Example if BattleMap is the 4th state added

class MainMapState : public CS230::GameState {
public:
    MainMapState(int screenWidth, int screenHeight);
    ~MainMapState();

    void Load() override;
    void Update(double dt) override;
    void Draw() override;
    void Unload() override;
    std::string GetName() override { return "MainMapState"; }

private:
    Map gameMap; // map object is now a member
    std::vector<General*> players;
    TurnManager turnManager;
    bool generalSelected;
    std::vector<HexTile> movableTiles;
    int screenWidth, screenHeight;
    float radiusX, radiusY; // If needed for map centering/scaling
};