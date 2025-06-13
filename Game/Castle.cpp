// Game/Castle.cpp
#include "Castle.h"
#include "../Engine/Sprite.h"
#include "Material.h"
#include "../Engine/Collision.h"

Castle::Castle(Math::vec2 position, const std::string& spt_file_path, Team owner) : CS230::GameObject(position), owner(owner) {
    auto* sprite = new CS230::Sprite(spt_file_path, this);
    AddGOComponent(sprite);

    Math::ivec2 size = sprite->GetFrameSize();
    Math::irect rect = { {0, 0}, {size.x, size.y} };
    AddGOComponent(new CS230::RectCollision(rect, this));
}

std::vector<Soldier*>& Castle::GetSoldierRoster() {
    return soldier_roster;
}

Team Castle::GetOwner() const {
    return owner;
}