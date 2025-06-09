#include "SnakeGen.h"
#include "States.h"
#include "../Engine/Engine.h"
SnakeGen::SnakeGen(Math::vec2 position) : CS230::GameObject(position)
{
	AddGOComponent(new CS230::Sprite("Assets/SnakeGen.spt", this));
	GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::None));
}

void SnakeGen::ResolveCollision(GameObject* other_object) {
	if (other_object->Type() == GameObjectTypes::EnemyGen) {
		//start battl map
		Engine::GetGameStateManager().SetNextGameState(STATE_BATTLE_MAP);
	}
}

void SnakeGen::Update(double dt)
{
	GameObject::Update(dt);
}

void SnakeGen::Draw(Math::TransformationMatrix camera_matrix)
{	//texture draw 에 박을 것...!
	//float scale = 0.3f;
	//float width = sprite.width * scale;
	//float height = sprite.height * scale;
	//float positionOffset = 20.0f;
	//
	//DrawTexturePro(
	//	sprite,
	//	{ 0, 0, (float)sprite.width, (float)sprite.height },
	//	{ position.x, position.y + positionOffset, width, height },
	//	{ width / 2.0f, height },
	//	0.0f,
	//	WHITE
	//);
	CS230::GameObject::Draw(camera_matrix);
}

bool SnakeGen::CanCollideWith(GameObjectTypes other_object)
{
	switch (other_object) {
	case GameObjectTypes::EnemyGen:
		return true;
	default:
		return false;
	}
}