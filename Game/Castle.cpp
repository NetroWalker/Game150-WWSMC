

// Game/Castle.cpp
#include "Castle.h"

#include "../Engine/Sprite.h"
#include "../Engine/Collision.h"   // ① 콜라이더
#include "../Engine/ShowCollision.h"   // ① 디버그 시각화

Castle::Castle(Math::vec2 position, bool _isSnake, const std::string& spritePath)
    : CS230::GameObject(position)
    , isSnake(_isSnake)
{
    // 1) 스프라이트 컴포넌트를 생성합니다.
    auto sprite = new CS230::Sprite(spritePath, this);
    AddGOComponent(sprite);

    // 이전에 추가했던 아래 라인을 반드시 삭제해주세요!
    // sprite->SetOffset( { 0, -sprite->GetSize().y / 2.0f } ); // <- 이 줄 삭제
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
    // 뱀 성은 다람쥐 장군만, 다람쥐 성은 뱀 장군만 충돌 대상으로 허용
    return isSnake
        ? (other == GameObjectTypes::SquirrelGen)
        : (other == GameObjectTypes::SnakeGen);
}

void Castle::ResolveCollision(CS230::GameObject* other) {
    // 충돌이 감지되었을 때 처리 (로그 찍기 등)
    Engine::GetLogger().LogEvent(
        TypeName() + " collided with " + other->TypeName()
    );
    // …추가 로직…
}
