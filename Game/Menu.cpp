//menu.cpp
#include"../Engine/Engine.h"
#include "menu.h"
#include "States.h"



Menu::Menu(int screenWidth, int screenHeight)
    : screenWidth(screenWidth), screenHeight(screenHeight), backgroundImage() {
}

Menu::~Menu() {
    
}

void Menu::Load()
{
    // 1. 모든 텍스처를 먼저 불러옵니다.
    tutorialButtonTexture = LoadTexture("Assets/tutorial.png");
    mainMapButtonTexture = LoadTexture("Assets/startbutton.png");
    backgroundImage = LoadTexture("Assets/BeastCrossing.png");

    // =============================================================
    // ▼▼▼ 실제 이미지 크기를 기준으로 위치를 다시 계산합니다 ▼▼▼
    // =============================================================

    // 2. 버튼의 클릭 영역(Rectangle) 크기를 실제 텍스처 크기와 일치시킵니다.
    tutorialButton.width = (float)tutorialButtonTexture.width;
    tutorialButton.height = (float)tutorialButtonTexture.height;
    mainMapButton.width = (float)mainMapButtonTexture.width;
    mainMapButton.height = (float)mainMapButtonTexture.height;

    // 3. 두 버튼의 전체 높이를 계산하여 세로 중앙 정렬을 위한 시작 Y좌표를 구합니다.
    int buttonSpacing = 40; // 버튼 사이 간격
    float totalButtonHeight = tutorialButton.height + mainMapButton.height + buttonSpacing;
    float startY = screenHeight / 2.0f - totalButtonHeight / 2.0f;

    // 4. 각 버튼의 최종 위치를 설정합니다.
    tutorialButton.x = screenWidth / 2.0f - tutorialButton.width / 2.0f;
    tutorialButton.y = startY;

    mainMapButton.x = screenWidth / 2.0f - mainMapButton.width / 2.0f;
    mainMapButton.y = startY + tutorialButton.height + buttonSpacing;
}

void Menu::Update(double dt) {

    if (StartTutorialClicked()) {
        Engine::Instance().GetGameStateManager().SetNextGameState(STATE_TUTORIAL);
    }
    else if (StartMainMapClicked()) {
        Engine::Instance().GetGameStateManager().SetNextGameState(STATE_MAIN_MAP);
    }
}

void Menu::Unload()
{
    UnloadTexture(tutorialButtonTexture);
    UnloadTexture(mainMapButtonTexture);
    UnloadTexture(backgroundImage);
}

void Menu::Draw() {
    DrawTexture(backgroundImage, 0, 0, WHITE);

    Vector2 mousePos = GetMousePosition();

    // --- 튜토리얼 버튼 그리기 ---
    // 마우스가 버튼 위에 있는지 확인합니다.
    if (CheckCollisionPointRec(mousePos, tutorialButton)) {
        // 호버 상태: 밝은 회색 색조로 그립니다.
        DrawRectangle(tutorialButton.x, tutorialButton.y, tutorialButton.width, tutorialButton.height ,RED);
        DrawTexture(tutorialButtonTexture, (int)tutorialButton.x, (int)tutorialButton.y, LIGHTGRAY);
    }
    else {
        // 기본 상태: 원래 색상(WHITE)으로 그립니다.
        DrawTexture(tutorialButtonTexture, (int)tutorialButton.x, (int)tutorialButton.y, WHITE);
    }

    // --- 메인 맵 버튼 그리기 ---
    // 마우스가 버튼 위에 있는지 확인합니다.
    if (CheckCollisionPointRec(mousePos, mainMapButton)) {
        // 호버 상태: 밝은 회색 색조로 그립니다.
        DrawTexture(mainMapButtonTexture, (int)mainMapButton.x, (int)mainMapButton.y, LIGHTGRAY);
    }
    else {
        // 기본 상태: 원래 색상(WHITE)으로 그립니다.
        DrawTexture(mainMapButtonTexture, (int)mainMapButton.x, (int)mainMapButton.y, WHITE);
    }
}

bool Menu::StartTutorialClicked() {
    Vector2 mousePos = GetMousePosition();
    return CheckCollisionPointRec(mousePos, tutorialButton) && IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
}

bool Menu::StartMainMapClicked() {
    Vector2 mousePos = GetMousePosition();
    return CheckCollisionPointRec(mousePos, mainMapButton) && IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
}
