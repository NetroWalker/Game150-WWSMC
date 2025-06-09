#include "LinearMovement.h"

LinearMovement::LinearMovement(CS230::GameObject* owner_obj, double move_speed)
    : owner(owner_obj), speed(move_speed) {
}

void LinearMovement::Update(double dt) {
    if (IsMoving()) {
        progress += speed * dt;
        if (progress >= 1.0) {
            progress = 1.0;
        }
        owner->SetPosition(
            { start_pos.x + (target_pos.x - start_pos.x) * progress,
              start_pos.y + (target_pos.y - start_pos.y) * progress }
        );
    }
}

void LinearMovement::MoveTo(Math::vec2 target) {
    start_pos = owner->GetPosition();
    target_pos = target;
    progress = 0.0;
}

bool LinearMovement::IsMoving() const {
    return progress < 1.0;
}

Math::vec2 LinearMovement::GetFootPosition() const {
    const Math::vec2& current_pos = owner->GetPosition();
    return { current_pos.x, current_pos.y + 20.0 };
}