/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Menu.h
Project:    CS230 Engine
Author:     Jonathan Holmes, Junwoo Lee
Created:    May 07, 25
Updated:    May 07, 2025
*/

#pragma once
#include"../Engine/Engine.h"
#include "../Engine/GameState.h"
#include "raylib.h"
#include "../Engine/Texture.h"

class Menu :public CS230::GameState{
public:
	void Load()override;
	void Update([[maybe_unused]]double dt)override;
	void Draw()override;
	void Unload()override;
	std::string GetName() override {
		return "Menu";
	}
private:
	struct MenuList {
		std::string text;
		Math::vec2 position;
		CS230::Texture* unselected_texture = nullptr;
		CS230::Texture* selected_texture = nullptr;
	};

	std::vector<MenuList> menulists;
	int index = 0;
	unsigned int on_color = 0xFF87CEEB;
	unsigned int off_color = 0xFFFFFFFF;
	CS230::Texture* title_texture = nullptr;
	Math::vec2 title_position;
	CS230::Texture* exit_texture = nullptr;
	Math::vec2 exit_position;

};