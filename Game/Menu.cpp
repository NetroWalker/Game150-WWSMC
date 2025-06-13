// menu.cpp
#include "../Engine/Engine.h"
#include "menu.h"
#include "States.h"

Menu::Menu(int screenWidth, int screenHeight)
    : screenWidth(screenWidth), screenHeight(screenHeight), backgroundImage() {
    // 버튼 위치와 크기를 이미지 기준으로 수동 설정
    mainMapButton = {
        590,  // x
        420,  // y
        325,  // width
        110    // height
    };

    tutorialButton = {
        590,
        290,
        325,
        110
    };
}

Menu::~Menu() {}

void Menu::Load() {
    backgroundImage = LoadTexture("Assets/BeastCrossing.png");
    startbutton = LoadTexture("Assets/startbutton.png");
    tutorialbutton = LoadTexture("Assets/tutorial.png");
}

void Menu::Update(double dt) {
    if (StartTutorialClicked()) {
        Engine::Instance().GetGameStateManager().SetNextGameState(STATE_TUTORIAL);
    }
    else if (StartMainMapClicked()) {
        Engine::Instance().GetGameStateManager().SetNextGameState(STATE_MAIN_MAP);
    }
}

void Menu::Unload() {
    UnloadTexture(backgroundImage);
    UnloadTexture(startbutton);
    UnloadTexture(tutorialbutton);
}

void Menu::Draw() {
    DrawTexture(backgroundImage, 0, 0, WHITE);

    // 텍스처(배경 포함된 버튼 텍스트)도 같이 그리되, 사각형 버튼은 따로 클릭 영역용
    DrawTexture(startbutton, 0, 0, WHITE);
    DrawTexture(tutorialbutton, 0, 0, WHITE);

    // 클릭 영역 표시 (디버그용/명확한 클릭 가능 영역 표시)
    DrawRectangleRec(mainMapButton, Fade(LIGHTGRAY, 0.3f));
    

    DrawRectangleRec(tutorialButton, Fade(LIGHTGRAY, 0.3f));
    
}

bool Menu::StartTutorialClicked() {
    Vector2 mousePos = GetMousePosition();
    return CheckCollisionPointRec(mousePos, mainMapButton) && IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
}

bool Menu::StartMainMapClicked() {
    Vector2 mousePos = GetMousePosition();
    return CheckCollisionPointRec(mousePos, tutorialButton) && IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
}
