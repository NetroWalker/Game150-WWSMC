/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Asteroid.cpp
Project:    CS230 Engine
Author:     Jonathan Holmes, Junwoo Lee
Created:    March 24, 2025
Updated:    April 12, 2025
*/
#include "Mode1.h"
#include "Asteroid.h"
#include "../Engine/Engine.h" 

Asteroid::Asteroid(Math::vec2 start_position, const CS230::Camera& camera) :
	GameObject(start_position),
	camera(camera)
{
	sprite.Load("Assets/Asteroid.spt");
	change_state(&state_bouncing);
}


//void Asteroid::Draw(Math::TransformationMatrix camera_matrix)
//
//{
//	sprite.Draw(camera_matrix * object_matrix);
//}

void Asteroid::State_Bouncing::Enter(GameObject* object)
{
	Asteroid* asteroid = static_cast<Asteroid*>(object);

	if (asteroid->GetPosition().y <= Mode1::floor) {
		asteroid->sprite.PlayAnimation(static_cast<int>(Animations::None));
		asteroid->SetVelocity({asteroid->GetVelocity().x, Asteroid::bounce_velocity});
	}
	else {
		asteroid->SetVelocity({ asteroid->GetVelocity().x, 0 });
	}
}

void Asteroid::State_Bouncing::Update(GameObject* object, double dt)
{
	Asteroid* asteroid = static_cast<Asteroid*>(object);
	asteroid->UpdateVelocity({ 0,-Mode1::gravity * dt });
}


void Asteroid::State_Bouncing::CheckExit(GameObject* object) {
	Asteroid* asteroid = static_cast<Asteroid*>(object);
	if (asteroid->GetPosition().y < Mode1::floor) {
		asteroid->SetPosition({ asteroid->GetPosition().x, Mode1::floor });
		asteroid->SetVelocity({ asteroid->GetVelocity().x, 0 });
		asteroid->change_state(&asteroid->state_landing);
	}
}


void Asteroid::State_Landing::Enter(GameObject* object)
{
	Asteroid* asteroid = static_cast<Asteroid*>(object);
	asteroid->sprite.PlayAnimation(static_cast<int>(Animations::Landing));
	asteroid->SetVelocity({ asteroid->GetVelocity().x, 0 });
	asteroid->SetPosition({ asteroid->GetPosition().x, Mode1::floor });
}

void Asteroid::State_Landing::Update(GameObject* object, double dt)
{
	Asteroid* asteroid = static_cast<Asteroid*>(object);
	asteroid; dt;
}

void Asteroid::State_Landing::CheckExit(GameObject* object)
{
	Asteroid* asteroid = static_cast<Asteroid*>(object);
	if (asteroid->sprite.AnimationEnded()) {
	asteroid->change_state(&asteroid->state_bouncing);
	}
}