#include"ShowCollision.h"

CS230::ShowCollision::ShowCollision() : enabled(true) {}

void CS230::ShowCollision::Update([[maybe_unused]]double dt) {
    if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::Tab) == true) {
        enabled = !enabled;
    }
}

bool CS230::ShowCollision::Enabled() {
    return enabled;
}
