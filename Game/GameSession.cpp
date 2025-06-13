#include "GameSession.h"
#include "Material.h" // Stone 클래스를 위해 포함

GameSession& GameSession::GetInstance() {
    static GameSession instance;
    return instance;
}

void GameSession::Load() {
    player1_resources = new Stone();
    player2_resources = new Stone();
}

void GameSession::Unload() {
    gom.Unload();
    delete player1_resources;
    player1_resources = nullptr;
    delete player2_resources;
    player2_resources = nullptr;
    player1_castles.clear();
    player2_castles.clear();
}