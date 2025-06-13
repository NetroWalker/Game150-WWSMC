// Game/Castle.h
#pragma once
#include "../Engine/GameObject.h"
#include "GameObjectTypes.h"
#include "../Engine/Engine.h"

#include <vector>
#include "Soldier.h"

class Castle : public CS230::GameObject {
public:
    Castle(Math::vec2 position, bool isSnake, const std::string& spritePath);

    GameObjectTypes Type() override;
    std::string TypeName() override;
    bool CanCollideWith(GameObjectTypes other) override;
    void ResolveCollision(CS230::GameObject* other) override;
    std::vector<Soldier*>& GetSoldierRoster();

private:
    bool isSnake;
    std::vector<Soldier*> soldier_roster;
};