#pragma once
#include "raylib.h"

class Menu {
public:
    Menu(int screenWidth, int screenHeight);
    ~Menu();

    void Update();
    void Draw();

    bool StartTutorialClicked();
    bool StartMainMapClicked();

private:
    int screenWidth;
    int screenHeight;
    Rectangle tutorialButton;
    Rectangle mainMapButton;
};