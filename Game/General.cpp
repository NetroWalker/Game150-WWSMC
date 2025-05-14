//General.cpp
#include "General.h"

General::General(Vector2 pos, const char* text_path)
    : position(pos), targetPos(pos), startPos(pos)
{
    sprite = LoadTexture(text_path);
}

General::~General() {
    UnloadTexture(sprite);
}

void General::Draw() const {
    float scale = 0.3f;
    float width = sprite.width * scale;
    float height = sprite.height * scale;
    float positionOffset = 20.0f;

    DrawTexturePro(
        sprite,
        { 0, 0, (float)sprite.width, (float)sprite.height },
        { position.x, position.y + positionOffset, width, height },
        { width / 2.0f, height },
        0.0f,
        WHITE
    );
}

void General::Update() {
    if (isMoving) {
        progress += 0.05f; // 속도 조절 가능
        if (progress >= 1.0f) {
            progress = 1.0f;
            isMoving = false;
        }

        // LERP
        position.x = startPos.x + (targetPos.x - startPos.x) * progress;
        position.y = startPos.y + (targetPos.y - startPos.y) * progress;
    }
}

void General::SetPosition(Vector2 pos) {
    startPos = position;
    targetPos = pos;
    progress = 0.0f;
    isMoving = true;
}

Vector2 General::GetPosition() const {
    return position;
}

Vector2 General::GetFootPosition() const {
    return { position.x, position.y + 20.0f };
}

bool General::IsMoving() const {
    return isMoving;
}

void General::CollectResources(HexTile* tile)
{
    if (!tile) return;

    wood += tile->woodCount;
    stone += tile->stoneCount;

    tile->woodCount = 0;
    tile->stoneCount = 0;
}
