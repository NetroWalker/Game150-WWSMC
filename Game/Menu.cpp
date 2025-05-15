#include "menu.h"

Menu::Menu(int screenWidth, int screenHeight)
    : screenWidth(screenWidth), screenHeight(screenHeight) {

    int buttonWidth = 200;
    int buttonHeight = 50;
    int buttonSpacing = 20;
    int startY = screenHeight / 2 - (buttonHeight + buttonSpacing) / 2;

    tutorialButton = {
        screenWidth / 2 - buttonWidth / 2.0f,
        (float)startY,
        (float)buttonWidth,
        (float)buttonHeight
    };

    mainMapButton = {
        screenWidth / 2 - buttonWidth / 2.0f,
        (float)startY + buttonHeight + buttonSpacing,
        (float)buttonWidth,
        (float)buttonHeight
    };

    // ✅ 배경 이미지 로드
    backgroundImage = LoadTexture("Assets/BeastCrossing.png");
}

Menu::~Menu() {
    UnloadTexture(backgroundImage); // ✅ 배경 이미지 언로드
}

void Menu::Update() {
    Vector2 mousePos = GetMousePosition();
    // 클릭 이벤트는 외부에서 StartTutorialClicked(), StartMainMapClicked()로 처리
}

void Menu::Draw() {
    // ✅ 배경 먼저 그림
    DrawTexture(backgroundImage, 0, 0, WHITE);

    // 버튼 출력
    DrawRectangleRec(tutorialButton, LIGHTGRAY);
    DrawText("Start Tutorial", (int)(tutorialButton.x + 10), (int)(tutorialButton.y + 10), 20, BLACK);

    DrawRectangleRec(mainMapButton, LIGHTGRAY);
    DrawText("Start Main Map", (int)(mainMapButton.x + 10), (int)(mainMapButton.y + 10), 20, BLACK);

    DrawText("Game Menu", screenWidth / 2 - MeasureText("Game Menu", 40) / 2, 50, 40, DARKGRAY);
}

bool Menu::StartTutorialClicked() {
    Vector2 mousePos = GetMousePosition();
    return CheckCollisionPointRec(mousePos, tutorialButton) && IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
}

bool Menu::StartMainMapClicked() {
    Vector2 mousePos = GetMousePosition();
    return CheckCollisionPointRec(mousePos, mainMapButton) && IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
}
