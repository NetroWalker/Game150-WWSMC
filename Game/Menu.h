//menu.h
#pragma once
#include "raylib.h"
#include"../Engine/GameState.h"
#include"../Engine/Texture.h"

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
    Texture2D tutorialButtonTexture;
    Texture2D mainMapButtonTexture;
    int screenWidth;
    int screenHeight;
    Rectangle tutorialButton;
    Rectangle mainMapButton;
    Texture2D backgroundImage;
};
