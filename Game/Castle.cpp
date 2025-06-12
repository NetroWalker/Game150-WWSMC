// Game/Castle.cpp
#include "Castle.h"
#include "../Engine/Sprite.h"
#include "Material.h"


Castle::Castle(Math::vec2 position, bool _isSnake, const std::string& spritePath)
    : CS230::GameObject(position)
    , isSnake(_isSnake)
{
    // 스프라이트 컴포넌트 추가
    AddGOComponent(new CS230::Sprite(spritePath, this));
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
    if (isSnake) {
        // 뱀의 성이면 다람쥐 장군하고만 충돌
        return (other == GameObjectTypes::SquirrelGen);
    }
    else {
        // 다람쥐의 성이면 뱀 장군하고만 충돌
        return (other == GameObjectTypes::SnakeGen);
    }
}

void Castle::ResolveCollision(CS230::GameObject* other) {
    // 1) 로그 남기기
    Engine::GetLogger().LogEvent(
        TypeName() + " collided with " + other->TypeName()
    );
}

//Castle::Castle(Math::vec2 position, const std::string& spt_file_path) : CS230::GameObject(position) {
//    AddGOComponent(new CS230::Sprite(spt_file_path, this));
//}
