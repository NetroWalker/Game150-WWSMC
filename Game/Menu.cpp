#include "menu.h"

Menu::Menu(int screenWidth, int screenHeight) :
    screenWidth(screenWidth), screenHeight(screenHeight) {
    // Button dimensions and positions
    int buttonWidth = 200;
    int buttonHeight = 50;
    int buttonSpacing = 20; // Space between buttons

    // Calculate positions to center the buttons
    int startY = screenHeight / 2 - (buttonHeight + buttonSpacing) / 2; // Adjusted to center

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
    // No resources to unload in this simple menu
}

void Menu::Update() {
    // Check for button clicks
    Vector2 mousePos = GetMousePosition();
    if (CheckCollisionPointRec(mousePos, tutorialButton) && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        // Start Tutorial button clicked
    }
    if (CheckCollisionPointRec(mousePos, mainMapButton) && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
        // Start Main Map button clicked
    }
}

void Menu::Draw() {
    ClearBackground(RAYWHITE); // Clear to white

    // Draw buttons
    DrawRectangleRec(tutorialButton, LIGHTGRAY);
    DrawText("Start Tutorial", (int)(tutorialButton.x + 10), (int)(tutorialButton.y + 10), 20, BLACK);

    DrawRectangleRec(mainMapButton, LIGHTGRAY);
    DrawText("Start Main Map", (int)(mainMapButton.x + 10), (int)(mainMapButton.y + 10), 20, BLACK);

    // You can add a title or other elements to the menu here
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