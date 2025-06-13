#pragma once
#include "../Engine/GameObject.h"
#include "GameObjectTypes.h"

class Button : public CS230::GameObject {
public:
	Button(Math::vec2 position, int type);
    GameObjectTypes Type() override { return GameObjectTypes::Button; }
    std::string TypeName() override { return "Button"; }
};