//menu.cpp
#include"../Engine/Engine.h"
#include "menu.h"
#include "States.h"



Menu::Menu(int screenWidth, int screenHeight)
    : screenWidth(screenWidth), screenHeight(screenHeight), backgroundImage() {

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
    
}

Menu::~Menu() {
    
}

void Menu::Load()
{
    backgroundImage = LoadTexture("Assets/BeastCrossing.png");
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
    UnloadTexture(backgroundImage);
}

void Menu::Draw() {
    DrawTexture(backgroundImage, 0, 0, WHITE);

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
