// Game/Castle.h
#pragma once
#include "../Engine/GameObject.h"
#include "GameObjectTypes.h"
#include "../Engine/Engine.h"


class Castle : public CS230::GameObject {
public:
    Castle(Math::vec2 position, bool isSnake, const std::string& spritePath);

    GameObjectTypes Type() override;
    std::string TypeName() override;


    bool CanCollideWith(GameObjectTypes other) override;

    void ResolveCollision(CS230::GameObject* other) override;
private:
    bool isSnake;
};

//class Castle : public CS230::GameObject {
//public:
//    Castle(Math::vec2 position, const std::string& spt_file_path);
//
//    GameObjectTypes Type() override { return GameObjectTypes::Squirrel; } // 임시로 Squirrel 타입 사용, 나중에 Castle 타입 추가
//    std::string TypeName() override { return "Castle"; }
//};

//class SquirrelCastle : public CS230::GameObject {
//public:
//    SquirrelCastle(Math::vec2 pos) : GameObject(pos) {  }
//    GameObjectTypes Type() override { return GameObjectTypes::SquirrelCastle; }
//    std::string TypeName() override { return "SquirrelCastle"; }
//};
//class SnakeCastle : public CS230::GameObject {
//public:
//    SnakeCastle(Math::vec2 pos) : GameObject(pos) {  }
//    GameObjectTypes Type() override { return GameObjectTypes::SnakeCastle; }
//    std::string TypeName() override { return "SnakeCastle"; }
//};