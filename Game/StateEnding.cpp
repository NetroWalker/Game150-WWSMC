#include "StateEnding.h"
#include "../Engine/Engine.h"
#include "States.h" // 상태 번호를 위해 포함
#include"GameSession.h"

StateEnding::StateEnding() {}

// Load 함수: 엔딩 이미지를 불러옵니다.
void StateEnding::Load() {
    p1_wins_image = LoadTexture("Assets/player1wins.png");
    p2_wins_image = LoadTexture("Assets/player2wins.png");
    endingImage = LoadTexture("Assets/ending.png");
}

// Update 함수: 화면을 클릭하면 메인 메뉴로 돌아갑니다.
void StateEnding::Update(double dt) {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        Engine::GetGameStateManager().SetNextGameState(STATE_MENU);
    }
}

// Unload 함수: 불러온 이미지를 메모리에서 해제합니다.
void StateEnding::Unload() {
    UnloadTexture(endingImage);
}

// Draw 함수: 화면에 엔딩 이미지를 그립니다.
void StateEnding::Draw() {
    ClearBackground(BLACK); // 배경을 검은색으로 지웁니다.
    GameSession::GameResult result = GameSession::GetInstance().last_game_result;
    // 이미지를 화면 왼쪽 상단(0, 0)에 맞게 그립니다.
    if (result == GameSession::GameResult::P1_Victory) {
        DrawTexture(p1_wins_image, 0, 0, WHITE);
    }
    else if (result == GameSession::GameResult::P2_Victory) {
        DrawTexture(p2_wins_image, 0, 0, WHITE);
    }
    else {
        // 혹시 모를 예외 상황을 위한 기본 텍스트
        DrawText("Game Over", 100, 100, 40, WHITE);
    }
}