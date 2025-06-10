// Game/Castle.cpp
#include "Castle.h"
#include "../Engine/Sprite.h"
#include "Material.h"

Castle::Castle(Math::vec2 position, const std::string& spt_file_path) : CS230::GameObject(position) {
    AddGOComponent(new CS230::Sprite(spt_file_path, this));
}
