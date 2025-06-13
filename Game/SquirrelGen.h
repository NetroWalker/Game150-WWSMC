//SquirrelGen.h
#pragma once
#include"../Engine/GameObject.h"
#include"GameObjectTypes.h"
#include "Soldier.h"
class SquirrelGen : public CS230::GameObject {
public:
	enum class Animations
	{
		None,
		Do_Something
	};
	SquirrelGen(Math::vec2 position);
	GameObjectTypes Type()override { return GameObjectTypes::SquirrelGen; }
	std::string TypeName()override { return "SquirrelGen"; }
	void ResolveCollision(GameObject* other_object)override;
	void Update(double dt) override;
	void Draw(Math::TransformationMatrix camera_matrix)override;
	bool CanCollideWith(GameObjectTypes other_object)override;

	std::vector<Soldier*>& GetSoldierRoster();

private:
	std::vector<Soldier*> soldier_roster;
};