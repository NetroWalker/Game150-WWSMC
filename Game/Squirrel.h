#pragma once
#include"../Engine/GameObject.h"
#include"GameObjectTypes.h"

class Squirrel : public CS230::GameObject {
public:
	enum class Animations
	{
		None,
		Do_Something
	};
	Squirrel(Math::vec2 position);
	GameObjectTypes Type()override { return GameObjectTypes::Squirrel; }
	std::string TypeName()override { return "Squirrel"; }
	void ResolveCollision(GameObject* other_object)override;
};