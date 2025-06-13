#include "StateEnding.h"
#include "../Engine/Engine.h"
#include "States.h" // 상태 번호를 위해 포함

StateEnding::StateEnding() {}

// Load 함수: 엔딩 이미지를 불러옵니다.
void StateEnding::Load() {
    // Assets 폴더에 ending.png 파일이 있어야 합니다.
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

    // 이미지를 화면 왼쪽 상단(0, 0)에 맞게 그립니다.
    DrawTexture(endingImage, 0, 0, WHITE);
}