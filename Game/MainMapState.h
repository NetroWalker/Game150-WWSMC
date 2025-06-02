// Game/MainMapState.h
#pragma once
#include "../Engine/Engine.h"
#include "map.h"
#include "General.h"
#include "../Engine/TurnManager.h"
#include "States.h"
#include <vector>

const int BATTLE_MAP_STATE_INDEX = 3;

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
    Map gameMap;
    std::vector<General*> players;
    TurnManager turnManager;
    bool generalSelected;
    std::vector<HexTile> movableTiles;
    int screenWidth, screenHeight;
    float radiusX, radiusY;
};