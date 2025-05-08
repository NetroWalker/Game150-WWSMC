/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Menu.cpp
Project:    CS230 Engine
Author:     Jonathan Holmes, Junwoo Lee
Created:    May 07, 2025
Updated:    May 07, 2025
*/
#include "States.h"
#include "Menu.h"
#include "Fonts.h"
#include "../Engine/Vec2.h"
#include "../Engine/Engine.h"


void Menu::Load()
{
    int simple_font_id = static_cast<int>(Fonts::Outlined);

    Math::vec2 window_center = Engine::GetWindow().GetSize() / 2.0f;
    float item_spacing = 70.0f;

    menulists.clear();

    menulists.push_back({ "Side Scroller", {window_center.x, window_center.y + item_spacing} });
    menulists.push_back({ "Space Shooter", {window_center.x, window_center.y} });
    menulists.push_back({ "Exit",          {window_center.x, window_center.y - item_spacing} });

    for (auto& item : menulists) {
        item.unselected_texture = Engine::GetFont(simple_font_id).PrintToTexture(item.text, off_color);
        item.selected_texture = Engine::GetFont(simple_font_id).PrintToTexture(item.text, on_color);
    }

    title_texture = Engine::GetFont(simple_font_id).PrintToTexture("CS230 Engine Test", 0x50BCDF);
    title_position = { window_center.x, window_center.y + item_spacing * 3.5f };

    index = 0;
}

void Menu::Update([[maybe_unused]]double dt)
{
    if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::Up)) {
        index--;
        if (index < 0) {
            index = static_cast<int>(menulists.size()) - 1;
        }
    }
    if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::Down)) {
        index++;
        if (index >= static_cast<int>(menulists.size())) {
            index = 0;
        }
    }

    if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::Enter)) {
        Engine::GetLogger().LogDebug("Enter pressed. Selected index: " + std::to_string(index));
        if (index == 0) {
            Engine::GetGameStateManager().SetNextGameState(static_cast<int>(States::Mode1));
        }
        else if (index == 1) {
            Engine::GetGameStateManager().SetNextGameState(static_cast<int>(States::Mode2));
        }
        else if (index == 2) {
            Engine::GetGameStateManager().ClearNextGameState();
        }
    }

    if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::Escape)) {
        Engine::GetGameStateManager().ClearNextGameState();
    }
}

void Menu::Draw()
{
    ClearBackground(BLACK);
    if (title_texture) {
        Math::ivec2 title_size = title_texture->GetSize();
        Math::vec2 title_origin_offset = { title_size.x / 2.0f, title_size.y / 2.0f };
        Math::vec2 actual_title_translation = { title_position.x - title_origin_offset.x, title_position.y - title_origin_offset.y };
        Math::TransformationMatrix title_matrix_centered = Math::TranslationMatrix(actual_title_translation);

        title_texture->Draw(title_matrix_centered);
    }
    
    for (size_t i = 0; i < menulists.size(); ++i) {
        const auto& item = menulists[i];
        bool is_selected = (i == index);
        CS230::Texture* texture_to_draw = is_selected ? item.selected_texture : item.unselected_texture;

        if (texture_to_draw) {
            Math::ivec2 item_size = texture_to_draw->GetSize();
            Math::vec2 item_origin_offset = { item_size.x / 2.0f, item_size.y / 2.0f };
            Math::vec2 actual_item_translation = { item.position.x - item_origin_offset.x, item.position.y - item_origin_offset.y };
            Math::TransformationMatrix item_matrix_centered = Math::TranslationMatrix(actual_item_translation);

            texture_to_draw->Draw(item_matrix_centered);
        }
    }
}

void Menu::Unload()
{
    if (title_texture) {
        delete title_texture;
        title_texture = nullptr;
    }

    for (auto& item : menulists) {
        if (item.unselected_texture) {
            delete item.unselected_texture;
            item.unselected_texture = nullptr;
        }
        if (item.selected_texture) {
            delete item.selected_texture;
            item.selected_texture = nullptr;
        }
    }
    menulists.clear();
}
