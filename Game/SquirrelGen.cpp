#include "SquirrelGen.h"
#include "States.h"
#include "../Engine/Engine.h"
SquirrelGen::SquirrelGen(Math::vec2 position) : CS230::GameObject(position)
{
	AddGOComponent(new CS230::Sprite("Assets/SquirrelGen.spt", this));
	GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::None));
}

void SquirrelGen::ResolveCollision(GameObject* other_object) {
	if (other_object->Type() == GameObjectTypes::EnemyGen) {
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
	case GameObjectTypes::EnemyGen:
		return true;
	default: 
		return false;
	}
}

