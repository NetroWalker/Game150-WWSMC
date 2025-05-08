/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Mode2.h
Project:    CS230 Engine
Author:     Jonathan Holmes, Junwoo Lee
Created:    March 8, 2023
Updated:    March 23, 2025
*/

#pragma once
#include"../Engine/GameObjectManager.h"
#include "../Engine/GameState.h"
#include "raylib.h"
#include "../Engine/Sprite.h"
class Ship;

class Mode2 : public CS230::GameState {
public:
    Mode2();
    void Load() override;
    void Draw() override;
    void Update(double dt) override;
    void Unload() override;

    std::string GetName() override {
        return "Mode2";
    }

public:
    
    double window_width = GetScreenWidth();
    double window_height = GetScreenHeight();
private:
    CS230::GameObjectManager gameobjectmanager;
};