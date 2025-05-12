/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Cat.h
Project:    CS230 Engine
Author:     Jonathan Holmes, Junwoo Lee
Created:    March 24, 2025
*/

#pragma once
#include"..\Engine\GameObject.h"
#include "..\Engine\Input.h"
#include "..\Engine\Camera.h"
#include "..\Engine\Matrix.h"
#include <string> 
#include <vector> 

class Cat : public CS230::GameObject{
private:
    enum class Animations {
        Idle = 0,
        Running,
        Jumping,
        Falling,
        Skidding
    };

public:
    Cat(Math::vec2 start_position, const CS230::Camera& camera);
    //void Load();
    void Update(double dt) override;
    //void Draw(Math::TransformationMatrix camera_matrix) override;
    const Math::vec2& GetPosition() const { return GameObject::GetPosition(); }


private:
    
    void update_x_velocity(double dt);

    //class State {
    //public:
    //    virtual ~State() = default; 
    //    virtual void Enter(Cat* cat) = 0;
    //    virtual void Update(Cat* cat, double dt) = 0;
    //    virtual void CheckExit(Cat* cat) = 0;
    //    virtual std::string GetName() = 0;
    //};

    class State_Idle : public CS230::State {
    public:
        virtual void Enter(GameObject* object) override;
        virtual void Update(GameObject* object, double dt) override;
        virtual void CheckExit(GameObject* object) override;
        std::string GetName() override { return "Idle"; }
    };
    State_Idle state_idle; 

    class State_Running : public CS230::State {
    public:
        virtual void Enter(GameObject* object) override;
        virtual void Update(GameObject* object, double dt) override;
        virtual void CheckExit(GameObject* objectt) override;
        std::string GetName() override { return "Running"; }
    };
    State_Running state_running;

    class State_Jumping : public CS230::State {
    public:
        virtual void Enter(GameObject* objectt) override;
        virtual void Update(GameObject* object, double dt) override;
        virtual void CheckExit(GameObject* object) override;
        std::string GetName() override { return "Jumping"; }
    };
    State_Jumping state_jumping;

    class State_Falling : public CS230::State {
    public:
        virtual void Enter(GameObject* object) override;
        virtual void Update(GameObject* object, double dt) override;
        virtual void CheckExit(GameObject* object) override;
        std::string GetName() override { return "Falling"; }
    };
    State_Falling state_falling;

    class State_Skidding : public CS230::State{
    public:
        virtual void Enter(GameObject* object) override;
        virtual void Update(GameObject* object, double dt) override;
        virtual void CheckExit(GameObject* object) override;
        std::string GetName() override { return "Skidding"; }
    };
    State_Skidding state_skidding; 

   //void change_state(State* new_state);
    
    //State* current_state = nullptr;

    const CS230::Camera& camera;
    //CS230::Sprite sprite;
    //Math::vec2 start_position;
    //Math::vec2 position;
    //Math::vec2 velocity;
    //Math::TransformationMatrix object_matrix;
    
    //bool flipped = false; 
  
    static constexpr double jump_velocity = 650;
    static constexpr double x_acceleration = 350;
    static constexpr double x_drag = 450; 
    static constexpr double max_velocity = 350;
};
