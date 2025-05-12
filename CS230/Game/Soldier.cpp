#include "Soldier.h"
#include <stdio.h>

Soldier::Soldier(Vector2 pos, SoldierType t, double r)
    : position(pos), radius(r), type(t)
{
    switch (type) {
    case RANGED:
        sprite = LoadTexture("assets/adc.png");
        hp = 30;
        speed = 2.0f;
        atk = 4;
        break;
    case TANK:
        sprite = LoadTexture("assets/tanker.png");
        hp = 80;
        speed = 1.0f;
        atk = 1;
        break;
    case MELEE:
        sprite = LoadTexture("assets/melee.png");
        hp = 20;
        speed = 4.0f;
        atk = 8;
        break;
    case CAT:
        sprite = LoadTexture("assets/cat.png");
        hp = 50;
        speed = 2.0f;
        atk = 5;
        break;
    }
}

Soldier::~Soldier() {
    // UnloadTexture(sprite); 
}

void Soldier::Draw() const {
    float scale = 0.35f;
    float width = sprite.width * scale;
    float height = sprite.height * scale;

    Color drawColor = selected ? ORANGE : WHITE;
    if (flash) {
        drawColor = RED;
    }

    DrawTexturePro(
        sprite,
        { 0, 0, (float)sprite.width, (float)sprite.height },
        { position.x, position.y, width, height },
        { width / 2.0f, height / 2.0f },
        0.0f,
        drawColor
    );

    int hpFontSize = 20;
    const char* hpText = TextFormat("%d", hp);
    int textWidth = MeasureText(hpText, hpFontSize);
    DrawText(hpText, (int)(position.x - textWidth / 2), (int)(position.y - hpFontSize / 2), hpFontSize, RED);

    if (flash) flash = false;
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

int Soldier::GetAtk() const {
    return atk;
}

void Soldier::ReceiveAttack(int damage) {
    hp -= damage;
    if (hp < 0) hp = 0;
    flash = true;
}

float Soldier::GetSpeed() const {
    return speed;
}

double Soldier::GetRadius() const {
    return radius;
}

int Soldier::GetHP() const {
    return hp;
}

void Soldier::ResetHP() {
    switch (type) {
    case RANGED:
        hp = 30;
        break;
    case TANK:
        hp = 80;
        break;
    }
}
