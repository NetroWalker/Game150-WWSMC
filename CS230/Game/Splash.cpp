/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Splash.cpp
Project:    CS230 Engine
Author:     Jonathan Holmes, Junwoo Lee
Created:    March 8, 2023
Updated:    March 23, 2025
*/

#include "../Engine/Engine.h"
#include "States.h"
#include "Splash.h"

Splash::Splash(){}

void Splash::Load() {

    counter = 0;
    texture = Engine::GetTextureManager().Load("Assets/DigiPen.png");
    transition_triggered = false;
}

void Splash::Draw() {
    Engine::GetWindow().Clear(UINT_MAX);
    
    texture->Draw(Math::TranslationMatrix({ (Engine::GetWindow().GetSize() - texture->GetSize()) / 2.0 }));

}


void Splash::Update([[maybe_unused]] double dt) {
    if (!transition_triggered) {
        Engine::GetLogger().LogDebug(std::to_string(counter));
        if ((counter >= 2)) {
            Engine::GetGameStateManager().ClearNextGameState();
            Engine::GetGameStateManager().SetNextGameState(static_cast<int>(States::MainMenu));
            Unload();
        }
        counter += dt;
    }
}

void Splash::Unload() {

}
