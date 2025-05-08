/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Ship.h
Project:    CS230 Engine
Author:     Jonathan Holmes, Junwoo Lee
Created:    March 24, 2025
*/

#pragma once
//#include "..\Engine\Sprite.h"
#include "..\Engine\Input.h"
//#include "..\Engine\Vec2.h"
#include "..\Engine\Matrix.h"
#include"..\Engine\GameObject.h"

class Ship : public CS230::GameObject{
public:
    enum class Flame_Animations {
        None,
        Flame
    };
public:
    Ship(Math::vec2 start_position);
    //void Load();
    void Update(double dt)override;
    void Draw(const Math::TransformationMatrix& camera_matrix)override;
private:
    //CS230::Sprite sprite;      
    CS230::Sprite flame_left;  
    CS230::Sprite flame_right; 
    //Math::vec2 start_position;
    //Math::vec2 position;
    //Math::vec2 velocity;
    //Math::TransformationMatrix object_matrix;
    //double angle = 0;      
    bool flameon = false;
    bool ExtracreditW = false;
    static constexpr double scale = 0.75;         
    static constexpr double rotation_speed = 3.0; 
    static constexpr double speed = 700;          
    static constexpr double drag = 1.0;           
};