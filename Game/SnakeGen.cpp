#include "SnakeGen.h"
#include "States.h"
#include "../Engine/Engine.h"
#include "LinearMovement.h"

SnakeGen::SnakeGen(Math::vec2 position) : CS230::GameObject(position)
{
	AddGOComponent(new CS230::Sprite("Assets/SnakeGen.spt", this));
	AddGOComponent(new LinearMovement(this));
	GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::None));
}

void SnakeGen::ResolveCollision(GameObject* other_object) {
	Math::vec2 my_tile = this->GetPosition();
	Math::vec2 enemy_tile = other_object->GetPosition();
	if (other_object->Type() == GameObjectTypes::SquirrelGen && (my_tile == enemy_tile)) {
		//start battl map
		Engine::GetGameStateManager().SetNextGameState(STATE_BATTLE_MAP);
	}
}

void SnakeGen::Update(double dt)
{
	GameObject::Update(dt);
}

void SnakeGen::Draw(Math::TransformationMatrix camera_matrix)
{
	CS230::GameObject::Draw(camera_matrix);
}

bool SnakeGen::CanCollideWith(GameObjectTypes other_object)
{
	switch (other_object) {
	case GameObjectTypes::SquirrelGen:
		return true;
	default:
		return false;
	}
}

std::vector<Soldier*>& SnakeGen::GetSoldierRoster() {
	return soldier_roster;
}