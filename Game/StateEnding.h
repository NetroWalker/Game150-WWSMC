#pragma once
#include "../Engine/GameState.h"
#include "raylib.h" // Texture2D를 위해 포함

class StateEnding : public CS230::GameState {
public:
    StateEnding();

    void Load() override;
    void Update(double dt) override;
    void Unload() override;
    void Draw() override;
    std::string GetName() override {
        return "StateEnding";
    }
private:
    Texture2D endingImage; // 엔딩 이미지를 저장할 변수
    Texture2D p1_wins_image;
    Texture2D p2_wins_image;
};