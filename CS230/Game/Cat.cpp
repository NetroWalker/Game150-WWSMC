/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Cat.cpp
Project:    CS230 Engine
Author:     Jonathan Holmes, Junwoo Lee
Created:    March 24, 2025
*/
#include "Mode1.h"
#include "Cat.h"
#include "../Engine/Engine.h"
#include <cmath>

Cat::Cat(Math::vec2 start_position, const CS230::Camera& camera) :
	GameObject(start_position),
	camera(camera)
{
	sprite.Load("Assets/Cat.spt");

	change_state(&state_idle);

	SetVelocity({ 0.0, 0.0 });
	SetScale({ 1,1 });

	//current_state = &state_idle;
	//current_state->Enter(this);
}

//void Cat::Load() {
//	sprite.Load("Assets/Cat.spt");
//	position = start_position;
//	velocity = { 0, 0 };
//	flipped = false;
//	current_state = &state_idle;
//	current_state->Enter(this);
//}

void Cat::update_x_velocity(double dt) {
	bool pressing_left = Engine::GetInput().KeyDown(CS230::Input::Keys::Left);
	bool pressing_right = Engine::GetInput().KeyDown(CS230::Input::Keys::Right);

	if (pressing_right && !pressing_left) {
		UpdateVelocity({ x_acceleration * dt, 0 });
		if (GetVelocity().x > max_velocity) {
			SetVelocity({ max_velocity, GetVelocity().y });
		}
		//flipped = false;
		SetScale({1,1});
	}
	else if (pressing_left && !pressing_right) {
		UpdateVelocity({ -x_acceleration * dt, 0 });
		if (GetVelocity().x < -max_velocity) {
			SetVelocity({ -max_velocity, GetVelocity().y });
		}
		//flipped = true;
		SetScale({ -1 , 1 });
	}
	else {
		if ( GetVelocity().x > x_drag * dt) {
			UpdateVelocity({ -x_drag * dt, 0 });
		}
		else if (GetVelocity().x < -x_drag * dt) {
			UpdateVelocity({ x_drag * dt, 0 });
		}
		else {
			SetVelocity({ 0, GetVelocity().y });
		}
	}
}

void Cat::Update(double dt) {
	GameObject::Update(dt);
	// Boundary Check
	if (GetPosition().x < camera.GetPosition().x + sprite.GetFrameSize().x / 2) {
		SetPosition({ camera.GetPosition().x + sprite.GetFrameSize().x / 2, GetPosition().y });
		SetVelocity({ 0, GetVelocity().y });
	}
	if (GetPosition().x + sprite.GetFrameSize().x / 2 > camera.GetPosition().x + Engine::GetWindow().GetSize().x) {
		SetPosition({ camera.GetPosition().x + Engine::GetWindow().GetSize().x - sprite.GetFrameSize().x / 2, GetPosition().y });
		SetVelocity({ 0, GetVelocity().y });
	}
}



//void Cat::change_state(State* new_state) {
//	if (!current_state || !new_state) {
//		Engine::GetLogger().LogError("Attempted to change Cat state with null pointer!");
//		return;
//	}
//	Engine::GetLogger().LogDebug("Cat Leaving State: " + current_state->GetName());
//	Engine::GetLogger().LogDebug("Cat Entering State: " + new_state->GetName());
//	current_state = new_state;
//	current_state->Enter(this);
//}

//void Cat::Draw(Math::TransformationMatrix camera_matrix) {
//	sprite.Draw(camera_matrix * object_matrix);
//}


void Cat::State_Idle::Enter(GameObject* object) {
	Cat* cat = static_cast<Cat*>(object);
	int idx = static_cast<int>(Animations::Idle);
	Engine::GetLogger().LogDebug(
		"State_Idle::Enter: calling PlayAnimation(" + std::to_string(idx) + ")"
	);
	cat->sprite.PlayAnimation(idx);
	cat->SetVelocity({ 0,0 });
}

void Cat::State_Idle::Update(GameObject* object, double dt) {
	Cat* cat = static_cast<Cat*>(object);
	if (cat->GetPosition().y > Mode1::floor) {
		cat->UpdateVelocity({ 0, -Mode1::gravity * dt });
	}
	else {
		cat->SetVelocity({ cat->GetVelocity().x, 0 });
		cat->SetPosition({ cat->GetPosition().x, Mode1::floor});
	}
}
void Cat::State_Idle::CheckExit(GameObject* object) {
	Cat* cat = static_cast<Cat*>(object);
	if (Engine::GetInput().KeyDown(CS230::Input::Keys::Left) || Engine::GetInput().KeyDown(CS230::Input::Keys::Right)) {
		cat->change_state(&cat->state_running);
	}
	else if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Up)) {
		cat->change_state(&cat->state_jumping);
	}
	else if (cat->GetPosition().y > Mode1::floor) {
		cat->change_state(&cat->state_falling);
	}
}

void Cat::State_Jumping::Enter(GameObject* object) {
	Cat* cat = static_cast<Cat*>(object);
	cat->sprite.PlayAnimation(static_cast<int>(Animations::Jumping));
	cat->SetVelocity({ cat->GetVelocity().x, Cat::jump_velocity });
}
void Cat::State_Jumping::Update(GameObject* object, double dt) {
	Cat* cat = static_cast<Cat*>(object);
	cat->UpdateVelocity({ 0, -Mode1::gravity * dt });
	cat->update_x_velocity(dt);
}
void Cat::State_Jumping::CheckExit(GameObject* object) {
	Cat* cat = static_cast<Cat*>(object);
	if (!Engine::GetInput().KeyDown(CS230::Input::Keys::Up)) {
		Engine::GetLogger().LogDebug("Released Jump Early : " + std::to_string(cat->GetPosition().y));
		cat->change_state(&cat->state_falling);
	}
	else if (cat->GetVelocity().y <= 0) {
		Engine::GetLogger().LogDebug("Max Height: " + std::to_string(cat->GetPosition().y));
		cat->change_state(&cat->state_falling);
	}
}

void Cat::State_Falling::Enter(GameObject* object) {
	Cat* cat = static_cast<Cat*>(object);
	cat->sprite.PlayAnimation(static_cast<int>(Animations::Falling));
}
void Cat::State_Falling::Update(GameObject* object, double dt) {
	Cat* cat = static_cast<Cat*>(object);
	cat->UpdateVelocity({ 0, -Mode1::gravity * dt });
	cat->update_x_velocity(dt);
}
void Cat::State_Falling::CheckExit(GameObject* object) {
	Cat* cat = static_cast<Cat*>(object);
	if (cat->GetPosition().y <= Mode1::floor) {
		cat->SetPosition({ cat->GetPosition().x, Mode1::floor });
		cat->SetVelocity({ cat->GetVelocity().x, 0 });

		Engine::GetLogger().LogDebug("Landing: " + std::to_string(cat->GetPosition().y));

		if (std::abs(cat->GetVelocity().x) < 0.1) {
			cat->change_state(&cat->state_idle);
		}
		else {
			bool pressing_opposite = (cat->GetVelocity().x > 0 && Engine::GetInput().KeyDown(CS230::Input::Keys::Left)) ||
				(cat->GetVelocity().x < 0 && Engine::GetInput().KeyDown(CS230::Input::Keys::Right));
			bool not_pressing_direction = !Engine::GetInput().KeyDown(CS230::Input::Keys::Left) && !Engine::GetInput().KeyDown(CS230::Input::Keys::Right);

			if (pressing_opposite || not_pressing_direction) {
				cat->change_state(&cat->state_skidding);
			}
			else {
				cat->change_state(&cat->state_running);
			}
		}
	}
}

void Cat::State_Running::Enter(GameObject* object) {
	Cat* cat = static_cast<Cat*>(object);
	cat->sprite.PlayAnimation(static_cast<int>(Animations::Running));
}
void Cat::State_Running::Update(GameObject* object, double dt) {
	Cat* cat = static_cast<Cat*>(object);
	cat->update_x_velocity(dt);
	if (cat->GetPosition().y > Mode1::floor) {
		cat->UpdateVelocity({ 0,-Mode1::gravity * dt });
	}
	else {
		cat->SetVelocity({ cat->GetVelocity().x, 0 });
		cat->SetPosition({ cat->GetPosition().x, Mode1::floor });
	}
}
void Cat::State_Running::CheckExit(GameObject* object) {
	Cat* cat = static_cast<Cat*>(object);
	if (cat->GetPosition().y > Mode1::floor) {
		cat->change_state(&cat->state_falling);
		return;
	}

	if (!Engine::GetInput().KeyDown(CS230::Input::Keys::Left) && !Engine::GetInput().KeyDown(CS230::Input::Keys::Right)) {
		if (std::abs(cat->GetVelocity().x) > 10.0) {
			cat->change_state(&cat->state_skidding);
		}
		else {
			cat->change_state(&cat->state_idle);
		}
	}
	else if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Up)) {
		cat->change_state(&cat->state_jumping);
	}
	else if ((cat->GetVelocity().x > 0 && Engine::GetInput().KeyDown(CS230::Input::Keys::Left)) ||
		(cat->GetVelocity().x < 0 && Engine::GetInput().KeyDown(CS230::Input::Keys::Right))) {
		cat->change_state(&cat->state_skidding);
	}
}

void Cat::State_Skidding::Enter(GameObject* object) {
	Cat* cat = static_cast<Cat*>(object);
	cat->sprite.PlayAnimation(static_cast<int>(Animations::Skidding));
	cat->SetVelocity({ cat->GetVelocity().x,0});
}
void Cat::State_Skidding::Update(GameObject* object, double dt) {
	Cat* cat = static_cast<Cat*>(object);
	if (cat->GetVelocity().x > 0) {
		cat->UpdateVelocity({ -(Cat::x_drag + Cat::x_acceleration) * dt , 0});
		if (cat->GetVelocity().x < 0) cat->SetVelocity({0, cat->GetVelocity().y});
		//cat->flipped = false;
		cat->SetScale({ 1,1 });
	}
	else if (cat->GetVelocity().x < 0) {
		cat->UpdateVelocity({ (Cat::x_drag + Cat::x_acceleration) * dt , 0 });
		if (cat->GetVelocity().x > 0) cat->SetVelocity({ 0, cat->GetVelocity().y });
		//cat->flipped = true;
		cat->SetScale({ -1,1 });
	}
	if (cat->GetVelocity().y > Mode1::floor) {
		cat->UpdateVelocity({ 0, -Mode1::gravity * dt });
	}
	else {
		cat->SetVelocity({ cat->GetVelocity().x, 0 });
		cat->SetPosition({cat->GetPosition().x, Mode1::floor});
	}
}
void Cat::State_Skidding::CheckExit(GameObject* object) {
	Cat* cat = static_cast<Cat*>(object);
	if (cat->GetPosition().y > Mode1::floor) {
		cat->change_state(&cat->state_falling);
		return;
	}

	if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Up)) {
		cat->change_state(&cat->state_jumping);
	}
	//else if (std::abs(cat->velocity.x) < 0.1) {
	//	cat->velocity.x = 0;
	//	cat->change_state(&cat->state_idle);
	//}
	else if ((cat->GetVelocity().x >= 0 && Engine::GetInput().KeyDown(CS230::Input::Keys::Right) && !Engine::GetInput().KeyDown(CS230::Input::Keys::Left)) ||
		(cat->GetVelocity().x <= 0 && Engine::GetInput().KeyDown(CS230::Input::Keys::Left) && !Engine::GetInput().KeyDown(CS230::Input::Keys::Right))) {
		cat->change_state(&cat->state_running);
	}
	else if (!Engine::GetInput().KeyDown(CS230::Input::Keys::Left) && !Engine::GetInput().KeyDown(CS230::Input::Keys::Right) && std::abs(cat->GetVelocity().x) < 1.0) {
		cat->SetVelocity({ 0, cat->GetVelocity().y });
		cat->change_state(&cat->state_idle);
	}
}