#pragma once
#include "../Engine/Engine.h"
#include "../Engine/GameState.h"
#include "../Engine/GameObjectManager.h"
#include "raylib.h"
#include "../Engine/Texture.h"
#include "../Engine/Sprite.h"
#include "../Engine/Camera.h"
#include "map.h"

class Mode1 : public CS230::GameState 
{
public:
    Mode1();
    void Load() override;
    void Draw() override;
    void Update(double dt) override;
    void Unload() override;

    std::string GetName() override 
    {
        return "Mode1";
    }

public:
    CS230::Camera camera;
    static constexpr double gravity = 800;
    static constexpr double floor = 80;
    Math::vec2  mapCenter = { Engine::GetWindow().GetSize().x / 2.0f, Engine::GetWindow().GetSize().y / 2.0f };

private:
    CS230::GameObjectManager gameobjectmanager;
    Map* hexMap; 
};
