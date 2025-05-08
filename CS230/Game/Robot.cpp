/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Robot.cpp
Project:    CS230 Engine
Author:     Jonathan Holmes, Junwoo Lee
Created:    April 30, 2025
Updated:    April 30, 2025
*/
#include"Robot.h"

Robot::Robot(Math::vec2 position) : CS230::GameObject(position) {
        sprite.Load("Assets/Robot.spt");
}
