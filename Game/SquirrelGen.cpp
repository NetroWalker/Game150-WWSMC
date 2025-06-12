#include "SquirrelGen.h"
#include "States.h"
#include "../Engine/Engine.h"
#include "LinearMovement.h"
SquirrelGen::SquirrelGen(Math::vec2 position) : CS230::GameObject(position)
{
	AddGOComponent(new CS230::Sprite("Assets/SquirrelGen.spt", this));
	AddGOComponent(new LinearMovement(this));
	GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::None));
}

void SquirrelGen::ResolveCollision(GameObject* other_object) {
	Math::vec2 my_tile = this->GetPosition();
	Math::vec2 enemy_tile = other_object->GetPosition();
	if (other_object->Type() == GameObjectTypes::SnakeGen && (my_tile == enemy_tile)) {
		//start battl map
		Engine::GetGameStateManager().SetNextGameState(STATE_BATTLE_MAP);
	}
}

void SquirrelGen::Update(double dt)
{
	GameObject::Update(dt);
}

void SquirrelGen::Draw(Math::TransformationMatrix camera_matrix)
{
	CS230::GameObject::Draw(camera_matrix);
}

bool SquirrelGen::CanCollideWith(GameObjectTypes other_object)
{
	switch (other_object) {
	case GameObjectTypes::SnakeGen:
	case GameObjectTypes::SnakeCastle:  // ¹ì ¼º
		return true;
	default: 
		return false;
	}
}

