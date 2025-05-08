/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Mode1.cpp
Project:    CS230 Engine
Author:     Jonathan Holmes, Junwoo Lee
Created:    March 8, 2023
Updated:    April 3, 2025
*/
#include "States.h"
#include "Mode1.h"
#include"Cat.h"
#include"Asteroid.h"
#include"Crates.h"
#include"Robot.h"
#include"Fonts.h"
#include"../Engine/Vec2.h"

Mode1::Mode1() :
    camera({ { 0.15 * Engine::GetWindow().GetSize().x, 0 }, 
        { 0.35 * Engine::GetWindow().GetSize().x, 0 } }),
    timer_texture(nullptr),
    timer(timer_max),
    last_timer(static_cast<int>(timer_max))
{
}

void Mode1::update_timer_text(int value) {
    timer_texture = Engine::GetFont(static_cast<int>(Fonts::Simple)).PrintToTexture("Timer: " + std::to_string(value), 0xFFFFFFFF);
}


void Mode1::Load()
{
    

    cat_ptr = new Cat({ 300, floor }, camera);
    gameobjectmanager.Add(cat_ptr);
    gameobjectmanager.Add(new Asteroid({ 600, floor }, camera));
    gameobjectmanager.Add(new Asteroid({ 1800, floor }, camera));
    gameobjectmanager.Add(new Asteroid({ 2400, floor }, camera));

    gameobjectmanager.Add(new Crates({ 900, floor }, 2));
    gameobjectmanager.Add(new Crates({ 1400, floor }, 1));
    gameobjectmanager.Add(new Crates({ 2000, floor }, 5));
    gameobjectmanager.Add(new Crates({ 4000, floor }, 3));
    gameobjectmanager.Add(new Crates({ 5600, floor }, 5));

    gameobjectmanager.Add(new Robot({ 1200, Mode1::floor }));
    gameobjectmanager.Add(new Robot({ 2200, Mode1::floor }));
    gameobjectmanager.Add(new Robot({ 3400, Mode1::floor }));
    gameobjectmanager.Add(new Robot({ 4200, Mode1::floor }));

    background.Add("Assets/Planets.png", 0.25);
    background.Add("Assets/ships.png", 0.5);
    background.Add("Assets/Foreground.png", 1.0);
    timer = timer_max;
    last_timer = static_cast<int>(timer_max);
    update_timer_text(last_timer);
    camera.SetPosition({ 0, 0 });
    camera.SetLimit({ {0,0}, 
        {background.GetSize() - Engine::GetWindow().GetSize()} });
}



void Mode1::Update(double dt) {
    gameobjectmanager.UpdateAll(dt);
    timer -= dt;
    if (static_cast<int>(timer) < last_timer) {
        last_timer = static_cast<int>(timer);
        update_timer_text(last_timer);
    }

    if (last_timer == 0) {
        Engine::GetGameStateManager().SetNextGameState(static_cast<int>(States::Mode2));
    }


    //if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::R)) {
    //    Engine::GetGameStateManager().ReloadState();
    //}

    camera.Update(cat_ptr->GetPosition());

    if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::Escape)) {
        Engine::GetGameStateManager().SetNextGameState(static_cast<int>(States::MainMenu));
    }
    //if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::Enter)) {
    //    Engine::GetGameStateManager().SetNextGameState(static_cast<int>(States::Mode2));
    //}

    
}

void Mode1::Draw() {
    auto window_size = Engine::GetWindow().GetSize();
    Engine::GetWindow().Clear(0x000000FF); 
    Math::TransformationMatrix camera_matrix = camera.GetMatrix(); 
    background.Draw(camera);

    timer_texture->Draw(Math::TranslationMatrix(Math::ivec2{ window_size.x - 10 - timer_texture->GetSize().x, window_size.y - timer_texture->GetSize().y - 5 }));
        //cat.Draw(camera_matrix); 
    //for (Asteroid& asteroid : asteroids) {
    //    asteroid.Draw(camera_matrix); 
    //}
    gameobjectmanager.DrawAll(camera_matrix);
}


void Mode1::Unload() {
    background.Unload();
    gameobjectmanager.Unload();
    cat_ptr = nullptr;
}
