#pragma once
#include "../Engine/Vec2.h"
#include "../Engine/Matrix.h"
#include "../Engine/GameObject.h"
#include "GameObjectTypes.h"

class WaitingZone : public CS230::GameObject {
public:
    WaitingZone(Math::vec2 pos)
        : GameObject(pos) {
        AddGOComponent(new CS230::Sprite("Assets/WaitingZone.spt", this));
    }

    void Update([[maybe_unused]]double dt) override {}
    void Draw(Math::TransformationMatrix camera_matrix) override {
        CS230::GameObject::Draw(camera_matrix);
    }

    GameObjectTypes Type() override { return GameObjectTypes::WaitingZone; }
    std::string TypeName() override { return "WaitingZone"; }
};