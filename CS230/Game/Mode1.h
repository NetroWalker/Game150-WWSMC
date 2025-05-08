/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Mode1.h
Project:    CS230 Engine
Author:     Jonathan Holmes, Junwoo Lee
Created:    March 8, 2023
Updated:    March 23, 2025
*/

#pragma once
#include"../Engine/Engine.h"
#include "../Engine/GameState.h"
#include"../Engine/GameObjectManager.h"
#include "raylib.h"
#include "../Engine/Texture.h"
#include "../Engine/Sprite.h"
#include"../Engine/Camera.h"
#include "Background.h"
class Cat;
class Asteroid;

class Mode1 : public CS230::GameState {
public:
    Mode1();
    void Load() override;
    void Draw() override;

    void Update(double dt) override;
    void Unload() override;

    std::string GetName() override {
        return "Mode1";
    }

public:
    CS230::Camera camera;
    Background background;
    static constexpr double gravity = 800;
    static constexpr double floor = 80;

    static constexpr double timer_max = 30;
private:
    CS230::GameObjectManager gameobjectmanager;
    Cat* cat_ptr{nullptr};

    CS230::Texture* timer_texture;
    double timer;
    int last_timer;
    void update_timer_text(int value);
};
