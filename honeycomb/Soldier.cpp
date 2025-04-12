#include "Soldier.h"

Soldier::Soldier(Vector2 pos, double r)
    : position(pos), radius(r) {
}

void Soldier::Draw() const {
    Color color;
    if (selected) {
        color = ORANGE;
    }
    else {
        color = DARKGREEN;
    }
    DrawCircle((int)position.x, (int)position.y, (float)radius, color);
}

void Soldier::SetPosition(Vector2 pos) {
    position = pos;
}

Vector2 Soldier::GetPosition() const {
    return position;
}

void Soldier::SetSelected(bool sel) {
    selected = sel;
}

bool Soldier::IsSelected() const {
    return selected;
}