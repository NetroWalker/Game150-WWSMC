/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Crates.h
Project:    CS230 Engine
Author:     Jonathan Holmes, Junwoo Lee
Created:    April 30, 2025
Updated:    April 30, 2025
*/
#include"../Engine/GameObject.h"

class Crates : public CS230::GameObject {
public:
    Crates(Math::vec2 position, int size);
};
