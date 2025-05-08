/*
Copyright(C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name : Ship.cpp
Project : CS230 Engine
Author : Jonathan Holmes, Junwoo Lee
Created : March 8, 2025
Updated : April 13, 2025
*/

#include "Ship.h"
#include "../Engine/Engine.h"
#include "../Engine/Matrix.h"
#include <string>
#include <initializer_list>

Ship::Ship(Math::vec2 start_position) :
    GameObject(start_position, 0.0, {scale, scale})
    //position(start_position),
{
    sprite.Load("Assets/Ship.spt");
    sprite.PlayAnimation(0);

    flame_left.Load("Assets/Flame.spt");
    flame_right.Load("Assets/Flame.spt");
    
    flame_left.PlayAnimation(0);
    flame_right.PlayAnimation(0);

    SetVelocity({ 0.0, 0.0 });
    SetRotation(0.0);
}

//void Ship::Load() {
//    sprite.Load("Assets/Ship.spt"); //{ {50, 41}, {-15, -41}, {15, -41} }
//    flame_left.Load("Assets/Flame.spt"); //{ { 8, 15 } }
//    flame_right.Load("Assets/Flame.spt"); //{ { 8, 15 }
//
//    position = start_position;
//    velocity = { 0.0, 0.0 };
//    angle = 0.0;
//    flameon = false;
//    ExtracreditW = false;
//    flame_left.PlayAnimation(0);
//    flame_right.PlayAnimation(0);
//}

void Ship::Update( double dt) {
        bool IsWDown = Engine::GetInput().KeyDown(CS230::Input::Keys::W);
        if (IsWDown != flameon) {
            flameon = IsWDown;
            int anim = flameon
                ? static_cast<int>(Flame_Animations::Flame)
                : static_cast<int>(Flame_Animations::None);

            flame_left.PlayAnimation(anim);
            flame_right.PlayAnimation(anim);
        }

        flame_left.Update(dt);
        flame_right.Update(dt);
        sprite.Update(dt);

    
    if (Engine::GetInput().KeyDown(CS230::Input::Keys::W)) {
        ExtracreditW = true;
        Math::vec2 rotate = Math::RotationMatrix(GetRotation()) * Math::vec2{ 0, speed * dt };
        UpdateVelocity({rotate});
        //velocity += Math::RotationMatrix(angle) * Math::vec2{ 0, speed * dt };
    }

    if (Engine::GetInput().KeyDown(CS230::Input::Keys::A)) {
        UpdateRotation(rotation_speed * dt);
    }

    if (Engine::GetInput().KeyDown(CS230::Input::Keys::D)) {
        UpdateRotation(-rotation_speed * dt);
    }
    UpdateVelocity({ -GetVelocity().x * drag * dt, -GetVelocity().y * drag * dt});
    //velocity -= velocity * drag * dt;
    UpdatePosition({ GetVelocity().x * dt, GetVelocity().y * dt });
    //position += velocity * dt;

    Math::ivec2 windowSize = Engine::GetWindow().GetSize();
    Math::vec2 textureSize = static_cast<Math::vec2>(sprite.GetFrameSize()) * scale;
    double half_width = textureSize.x / 2.0;
    double half_height = textureSize.y / 2.0;

    if (GetPosition().x + half_width < 0) {
        SetPosition({ static_cast<double>(windowSize.x) + half_width, GetPosition().y });
    }
    else if (GetPosition().x - half_width > windowSize.x) {
        SetPosition({ 0.0 - half_width, GetPosition().y });
    }
    if (GetPosition().y + half_height < 0) {
        SetPosition({ GetPosition().x, static_cast<double>(windowSize.y) + half_height });
    }
    else if (GetPosition().y - half_height > windowSize.y) {
        SetPosition({ GetPosition().x, 0.0 - half_height });
    }

    //object_matrix = Math::TranslationMatrix(position) * Math::RotationMatrix(angle) * Math::ScaleMatrix(scale);
}

void Ship::Draw(const Math::TransformationMatrix& camera_matrix)
{
   if (ExtracreditW) {
        flame_left.Draw(camera_matrix * GetMatrix() *
            camera_matrix * Math::TranslationMatrix(static_cast<Math::vec2>(sprite.GetHotSpot(1))));
        flame_right.Draw(camera_matrix * GetMatrix() *
            camera_matrix * Math::TranslationMatrix(static_cast<Math::vec2>(sprite.GetHotSpot(2))));
   }
    sprite.Draw(camera_matrix * GetMatrix());
}

