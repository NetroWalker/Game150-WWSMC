//menu.h
#pragma once
#include "raylib.h"
#include"../Engine/GameState.h"
#include"../Engine/Texture.h"

const int MENU_STATE_IDX = 0;
const int TUTORIAL_STATE_IDX = 1;
const int MAIN_MAP_STATE_IDX = 2;
const int BATTLE_MAP_STATE_IDX = 3;

class Menu : public CS230::GameState{
public:
    Menu(int screenWidth, int screenHeight);
    ~Menu();

    void Load()override;
    void Update(double dt)override;
    void Unload()override;
    void Draw()override;
    std::string GetName() override {
        return "Menu";
    }

    bool StartTutorialClicked();
    bool StartMainMapClicked();

private:
    int screenWidth;
    int screenHeight;
    Rectangle tutorialButton;
    Rectangle mainMapButton;
    Texture2D backgroundImage;
};
