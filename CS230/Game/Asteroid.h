/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Asteroid.h
Project:    CS230 Engine
Author:     Jonathan Holmes, Junwoo Lee
Created:    March 24, 2025
Updated:    April 12, 2025
*/
#pragma once
#include"..\Engine\GameObject.h"
//#include "..\Engine\Sprite.h"
//#include "..\Engine\Vec2.h"
#include "..\Engine\Matrix.h"
#include "..\Engine\Camera.h"
#include <string> 
#include <vector> 

class Asteroid : public CS230::GameObject{
    enum class Animations {
        Landing,
        None
    };
public:
    Asteroid(Math::vec2 start_position, const CS230::Camera& camera);
    //void Load();
    //void Update(double dt) override;
    //void Draw(Math::TransformationMatrix camera_matrix) override;

private:
    const CS230::Camera& camera;
    //CS230::Sprite sprite;
    //Math::vec2 start_position;
    //Math::vec2 position;
    //Math::vec2 velocity;
    //Math::TransformationMatrix object_matrix;
    static constexpr double bounce_velocity = 500;
    static constexpr double landing = 0.5; 
    //class State {
    //public:
    //    virtual ~State() = default; 
    //    virtual void Enter(Asteroid* asteroid) = 0;
    //    virtual void Update(Asteroid* asteroid, double dt) = 0;
    //    virtual void CheckExit(Asteroid* asteroid) = 0;
    //    virtual std::string GetName() = 0;
    //};

    class State_Bouncing : public CS230::State {
    public:
        virtual void Enter(GameObject* object) override;
        virtual void Update(GameObject* object, double dt) override;
        virtual void CheckExit(GameObject* object) override;
        std::string GetName() override { return "Bouncing"; }
    };
    State_Bouncing state_bouncing; 
    class State_Landing : public CS230::State {
    public:
        virtual void Enter(GameObject* object) override;
        virtual void Update(GameObject* object, double dt) override;
        virtual void CheckExit(GameObject* object) override;
        std::string GetName() override { return "Landing"; }
    };
    State_Landing state_landing; 


    //State* current_state = nullptr; 
    //void change_state(State* new_state);
};