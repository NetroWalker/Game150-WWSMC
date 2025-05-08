/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Sprite.cpp
Project:    CS230 Engine
Author:     Jonathan Holmes, Junwoo Lee
Created:    March 8, 2023
*/

#include "Sprite.h"
#include "Engine.h" 

namespace CS230 {
    Sprite::Sprite() {
    }
    Sprite::~Sprite()
    {
        for (Animation* anma : animations) {
            delete anma;
        }
        animations.clear();
    }
   
    CS230::Sprite::Sprite(Sprite&& temporary) noexcept :
        texture(std::move(temporary.texture)),
        hotspots(std::move(temporary.hotspots)),
        current_animation(temporary.current_animation),
        frame_size(temporary.frame_size),
        frame_texels(std::move(temporary.frame_texels)),
        animations(std::move(temporary.animations))
    {
    }

    CS230::Sprite& CS230::Sprite::operator=(Sprite&& temporary) noexcept {
        std::swap(texture, temporary.texture);
        std::swap(hotspots, temporary.hotspots);
        std::swap(current_animation, temporary.current_animation);
        std::swap(frame_size, temporary.frame_size);
        std::swap(frame_texels, temporary.frame_texels);
        std::swap(animations, temporary.animations);
        return *this;
    }


    void Sprite::Update(double dt) {
        if (current_animation >= 0 && current_animation < animations.size()) {
            animations[current_animation]->Update(dt);
        }
    }

    void CS230::Sprite::Load(const std::filesystem::path& sprite_file) {
        animations.clear();
        Engine::GetLogger().LogDebug("Loaded " + std::to_string(animations.size()) + " animations for " + sprite_file.generic_string());

        if (sprite_file.extension() != ".spt") {
            throw std::runtime_error(sprite_file.generic_string() + " is not a .spt file");
        }
        std::ifstream in_file(sprite_file);

        if (in_file.is_open() == false) {
            throw std::runtime_error("Failed to load " + sprite_file.generic_string());
        }

        hotspots.clear();
        frame_texels.clear();

        std::string text;
        in_file >> text;
        texture = Engine::GetTextureManager().Load(text);
        frame_size = texture->GetSize();

        in_file >> text;
        while (in_file.eof() == false) {
            if (text == "FrameSize") {
                in_file >> frame_size.x;
                in_file >> frame_size.y;
            }
            else if (text == "NumFrames") { 
                int frame_count;
                in_file >> frame_count;
                for (int i = 0; i < frame_count; i++) {
                    frame_texels.push_back({ frame_size.x * i, 0 });
                }
            }
            else if (text == "Frame") {
                int frame_location_x, frame_location_y;
                in_file >> frame_location_x;
                in_file >> frame_location_y;
                frame_texels.push_back({ frame_location_x, frame_location_y });
            }
            else if (text == "HotSpot") {
                int hotspot_x, hotspot_y;
                in_file >> hotspot_x;
                in_file >> hotspot_y;
                hotspots.push_back({ hotspot_x, hotspot_y });
            }
            else if (text == "Anim") {
                std::string anim_path;
                in_file>>anim_path;
                animations.push_back(new Animation(anim_path));

            }
            else {
                Engine::GetLogger().LogError("Unknown command: " + text);
            }
            in_file >> text;
        }
        if (frame_texels.empty()) {
            frame_texels.push_back({ 0,0 });
        }
        if (animations.empty()) {
            animations.push_back(new Animation());
            PlayAnimation(0);
        }
    }


    void CS230::Sprite::Draw(Math::TransformationMatrix display_matrix) {
        int index = animations[current_animation]->CurrentFrame();
        texture->Draw(display_matrix * Math::TranslationMatrix(-GetHotSpot(0)), GetFrameTexel(index), GetFrameSize());
    }

    Math::ivec2 Sprite::GetHotSpot(int index) {
        if ((index < 0) || (index >= hotspots.size())) {
            return { 0, 0 };
        }
        else {
            return hotspots[index];
            
        }
    }

    Math::ivec2 Sprite::GetFrameSize()
    {
        return frame_size;
    }

    void Sprite::PlayAnimation(int animation)
    {
        if (animation < 0 || animation >= static_cast<int>(animations.size())) {
            Engine::GetLogger().LogError("Animation doesn't exist!!");
            current_animation = 0;
            return;
        }
        current_animation = animation;
        animations[current_animation]->Reset();
    }


    bool Sprite::AnimationEnded()
    {
        if ((current_animation < 0) || (current_animation >= animations.size())) {
            return true;
        }
        return animations[current_animation] -> Ended();
    }

    Math::ivec2 Sprite::GetFrameTexel(int index) const
    {
        if ((index <0) || (index>=frame_texels.size())) {
            Engine::GetLogger().LogError("Error! Index is invalid");
            return { 0,0 };
        }
        return frame_texels[index];
    }
}