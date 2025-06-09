//Splash.h
#pragma once
#include "../Engine/GameState.h"
#include "../Engine/Texture.h"

class Splash : public CS230::GameState {
public:
    Splash();
    void Load() override;
    void Update(double dt) override;
    void Unload() override;
    void Draw() override;

    std::string GetName() override { return "Splash"; }
private:
    bool transition_triggered = false;
    double counter = 0;
    CS230::Texture* texture = nullptr;
};

