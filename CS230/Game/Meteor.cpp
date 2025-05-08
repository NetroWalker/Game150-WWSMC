/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Meteor.cpp
Project:    CS230 Engine
Author:     Jonathan Holmes, Junwoo Lee
Created:    April 30, 2025
Updated:    April 30, 2025
*/
#include"Meteor.h"
#include"../Engine/Engine.h"

Meteor::Meteor() : CS230::GameObject({0,0})
{
    sprite.Load("Assets/Meteor.spt");

    double vx = ((double)rand() / RAND_MAX * 2.0 - 1.0) * default_velocity;
    double vy = ((double)rand() / RAND_MAX * 2.0 - 1.0) * default_velocity;
    SetVelocity({ vx, vy });

    double rand_angle = (double)rand() / RAND_MAX * 2.0 * PI;
    SetRotation(rand_angle);

    double x = (double)rand() / RAND_MAX * Engine::GetWindow().GetSize().x;
    double y = (double)rand() / RAND_MAX * Engine::GetWindow().GetSize().y;
    SetPosition({ x, y });


}

    void Meteor::Update(double dt) {
        GameObject::Update(dt);
        Math::vec2 textureSize = static_cast<Math::vec2>(sprite.GetFrameSize());
        double half_width = textureSize.x / 2.0;
        double half_height = textureSize.y / 2.0;

        if (GetPosition().x + half_width < 0) {
            SetPosition({ static_cast<double>(Engine::GetWindow().GetSize().x) + half_width, GetPosition().y });
        }
        else if (GetPosition().x - half_width > Engine::GetWindow().GetSize().x) {
            SetPosition({ 0.0 - half_width, GetPosition().y });
        }
        if (GetPosition().y + half_height < 0) {
            SetPosition({ GetPosition().x, static_cast<double>(Engine::GetWindow().GetSize().y) + half_height });
        }
        else if (GetPosition().y - half_height > Engine::GetWindow().GetSize().y) {
            SetPosition({ GetPosition().x, 0.0 - half_height });
        }
        Engine::GetLogger().LogDebug("Rotation: " + std::to_string(GetRotation()));
}