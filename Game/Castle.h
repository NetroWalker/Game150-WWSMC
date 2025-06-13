// Game/Castle.h
#pragma once
#include "../Engine/GameObject.h"
#include "GameObjectTypes.h"
#include "Soldier.h"

enum class Team { P1, P2 };  // 소유자 구분용 enum

class Castle : public CS230::GameObject {
public:
    Castle(Math::vec2 position, const std::string& spt_file_path, Team owner);
    GameObjectTypes Type() override { return GameObjectTypes::Castle; } // 임시로 Squirrel 타입 사용, 나중에 Castle 타입 추가
    std::string TypeName() override { return "Castle"; }

    std::vector<Soldier*>& GetSoldierRoster();
    Team GetOwner() const;

private:
    std::vector<Soldier*> soldier_roster;
    Team owner;
};