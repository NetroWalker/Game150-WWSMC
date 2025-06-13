
// Game/Castle.cpp
#include "Castle.h"
#include "Material.h"
#include "../Engine/Sprite.h"
#include "../Engine/Collision.h"     
#include "../Engine/ShowCollision.h" 

Castle::Castle(Math::vec2 position, bool _isSnake, const std::string& spritePath)
    : CS230::GameObject(position)
    , isSnake(_isSnake)
{
    auto sprite = new CS230::Sprite(spritePath, this);
    AddGOComponent(sprite);
     Math::ivec2 size = sprite->GetFrameSize();
    Math::irect rect = { {0, 0}, {size.x, size.y} };
    AddGOComponent(new CS230::RectCollision(rect, this));
}

GameObjectTypes Castle::Type() {
    return isSnake
        ? GameObjectTypes::SnakeCastle
        : GameObjectTypes::SquirrelCastle;
}

std::string Castle::TypeName() {
    return isSnake
        ? "SnakeCastle"
        : "SquirrelCastle";
}

bool Castle::CanCollideWith(GameObjectTypes other) {
    return isSnake
        ? (other == GameObjectTypes::SquirrelGen)
        : (other == GameObjectTypes::SnakeGen);
}

void Castle::ResolveCollision(CS230::GameObject* other) {
    Engine::GetLogger().LogEvent(
        TypeName() + " collided with " + other->TypeName()
    );
}

std::vector<Soldier*>& Castle::GetSoldierRoster() {
    return soldier_roster;
}

Team Castle::GetOwner() const {
    return owner;
}