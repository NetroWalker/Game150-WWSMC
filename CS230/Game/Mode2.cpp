/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Mode2.cpp
Project:    CS230 Engine
Author:     Jonathan Holmes, Junwoo Lee
Created:    March 8, 2023
Updated:    March 23, 2025
*/

#include "../Engine/Engine.h"
#include "States.h"
#include "Mode2.h"
#include "Ship.h"
#include "Meteor.h"

Mode2::Mode2()
{
}

void Mode2::Load() {
    gameobjectmanager.Add(new Meteor());
    gameobjectmanager.Add(new Meteor());
    gameobjectmanager.Add(new Meteor());
    gameobjectmanager.Add(new Meteor());
    gameobjectmanager.Add(new Meteor());

    auto size = Engine::GetWindow().GetSize();
    Math::vec2 center = {
        static_cast<double>(size.x) / 2.0,
        static_cast<double>(size.y) / 2.0
    };
    gameobjectmanager.Add(new Ship(center));
}
void Mode2::Draw() {
    Engine::GetWindow().Clear(0x000000FF);
    gameobjectmanager.DrawAll(Math::TransformationMatrix());
}


void Mode2::Update(double dt) {
    gameobjectmanager.UpdateAll(dt);
    //if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::R)) {
    //    Engine::GetGameStateManager().ReloadState();
    //}
    //ship.Update(dt);
    //if (IsKeyPressed(KEY_TWO)) {
    //    Engine::GetGameStateManager().ClearNextGameState();
    //}
    if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::Escape)) {
        Engine::GetGameStateManager().SetNextGameState(static_cast<int>(States::MainMenu));
    }

}

void Mode2::Unload() {
    gameobjectmanager.Unload();
}
