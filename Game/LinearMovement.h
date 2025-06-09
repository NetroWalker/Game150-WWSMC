#pragma once
#include "../Engine/Component.h"
#include "../Engine/GameObject.h"
#include "raylib.h"

class LinearMovement : public CS230::Component {
public:
    LinearMovement(CS230::GameObject* owner, double speed = 4.0);
    void Update(double dt) override;
    void MoveTo(Math::vec2 target);
    bool IsMoving() const;
    Math::vec2 GetFootPosition() const;

private:
    CS230::GameObject* owner;
    Math::vec2 start_pos;
    Math::vec2 target_pos;
    double progress = 1.0;
    double speed;
};