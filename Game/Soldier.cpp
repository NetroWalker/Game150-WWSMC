#include "Soldier.h"

Soldier::Soldier(Vector2 pos, SoldierType t, double r)
    : position(pos), radius(r), type(t) {
    switch (type) {
    case RANGED:
        sprite = LoadTexture("assets/adc.png");
        break;
    case TANK:
        sprite = LoadTexture("assets/tanker.png");
        break;
    case MELEE:
        sprite = LoadTexture("assets/melee.png");
        break;
    case CAT:
        sprite = LoadTexture("assets/cat.png");
        break;
    }
}

Soldier::~Soldier() {
    UnloadTexture(sprite);
}

void Soldier::Draw() const {
    float scale = 0.35f;
    float width = sprite.width * scale;
    float height = sprite.height * scale;

    DrawTexturePro(
        sprite,
        { 0, 0, (float)sprite.width, (float)sprite.height },
        { position.x, position.y, width, height },
        { width / 2.0f, height / 2.0f },
        0.0f,
        selected ? ORANGE : WHITE
    );
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

double Soldier::GetRadius() const {
    return radius;
}
