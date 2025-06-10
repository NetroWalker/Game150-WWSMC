#include "Squirrel.h"

Squirrel::Squirrel(Math::vec2 position) : CS230::GameObject(position)
{
	AddGOComponent(new CS230::Sprite("Assets/Squirrel.spt", this));
	GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::None));
}

void Squirrel::ResolveCollision(GameObject* other_object) {
	if (other_object->Type() == GameObjectTypes::Snake) {
		//Fight algorithm ----Yeonseo Job when the unit
	}
}