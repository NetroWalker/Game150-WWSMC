/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Background.cpp
Project:    CS230 Engine
Author:     Jonathan Holmes, Junwoo Lee
Created:    April 6, 2025
*/
#include "Background.h"
#include "../Engine/Engine.h"

void Background::Add(const std::filesystem::path& texture_path, double speed)
{
    auto texture = Engine::GetTextureManager().Load(texture_path);
	backgrounds.emplace_back(ParallaxLayer{ texture, speed });
}

void Background::Unload()
{
	backgrounds.clear();
}


void Background::Draw(const CS230::Camera& camera)
{
    for (size_t i = 0; i < backgrounds.size(); i++) {
        Math::vec2 offset = -camera.GetPosition() * backgrounds[i].speed;
        Math::TranslationMatrix translation(offset);
        backgrounds[i].texture->Draw(translation);
    }
}


Math::ivec2 Background::GetSize()
{
	return Math::ivec2(backgrounds[backgrounds.size() - 1].texture->GetSize());

}