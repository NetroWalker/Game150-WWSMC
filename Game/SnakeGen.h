//SnakeGen.h
#pragma once
#include"../Engine/GameObject.h"
#include"GameObjectTypes.h"

class SnakeGen : public CS230::GameObject {
public:
	enum class Animations
	{
		None,
		Do_Something
	};
	SnakeGen(Math::vec2 position);
	GameObjectTypes Type()override { return GameObjectTypes::SnakeGen; }
	std::string TypeName()override { return "SnakeGen"; }
	void ResolveCollision(GameObject* other_object)override;
	void Update(double dt) override;
	void Draw(Math::TransformationMatrix camera_matrix)override;
	bool CanCollideWith(GameObjectTypes other_object)override;
	
};