/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  TextureManager.cpp
Project:    CS230 Engine
Author:     Jonathan Holmes, Junwoo Lee
Created:    April 30, 2025
Updated:    April 30, 2025
*/
#include "TextureManager.h"
#include "Texture.h"
#include "Engine.h"

CS230::Texture* CS230::TextureManager::Load(const std::filesystem::path& file_name)
{
    auto it = textures.find(file_name);
    if (it != textures.end()) {
        return it->second;
    }

    Engine::GetLogger().LogEvent("Loading texture: " + file_name.string());

    CS230::Texture* tex = new CS230::Texture(file_name);
    textures[file_name] = tex;
    return tex;
}

void CS230::TextureManager::Unload()
{
    Engine::GetLogger().LogEvent("Unloading all textures");
    for (auto& pair : textures) {
        delete pair.second;
    }
    textures.clear();
}

namespace {
    RenderTexture2D s_render_texture = {};
}


void CS230::TextureManager::StartRenderTextureMode(int width, int height) {
    s_render_texture = LoadRenderTexture(width, height);
    BeginTextureMode(s_render_texture);
    rlClearColor(0, 0, 0, 0);
    rlClearScreenBuffers();
}

CS230::Texture* CS230::TextureManager::EndRenderTextureMode() {
    EndTextureMode();
    const auto window_size = Engine::GetWindow().GetSize();
    rlSetFramebufferWidth(window_size.x);
    rlSetFramebufferHeight(window_size.y);
    rlUnloadFramebuffer(s_render_texture.id);
    const auto result = new Texture{ s_render_texture.texture };
    rendered_textures.push_back(result);
    return result;
}
