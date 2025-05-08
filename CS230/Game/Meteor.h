/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Meteor.h
Project:    CS230 Engine
Author:     Jonathan Holmes, Junwoo Lee
Created:    April 30, 2025
Updated:    April 30, 2025
*/
#include"../Engine/GameObject.h"
#include<cmath>

class Meteor : public CS230::GameObject {
public:
    Meteor();

    void Update(double dt) override;

private:
    static constexpr double default_velocity = 100;
};
