#include "SquirrelGen.h"
#include "SnakeGen.h"
#include "States.h"
#include "Castle.h"
#include "BattleMap.h"
#include "MainMapState.h"
#include "../Engine/Engine.h"
#include "LinearMovement.h"
SquirrelGen::SquirrelGen(Math::vec2 position) : CS230::GameObject(position)
{
	AddGOComponent(new CS230::Sprite("Assets/SquirrelGen.spt", this));
	AddGOComponent(new LinearMovement(this));
	GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::None));
}

void SquirrelGen::ResolveCollision(GameObject* other_object) {
    if (GetPosition().x != other_object->GetPosition().x ||
        GetPosition().y != other_object->GetPosition().y) return;

    std::vector<Soldier*>* my_roster = &this->GetSoldierRoster();
    std::vector<Soldier*>* enemy_roster = nullptr;

    if (other_object->Type() == GameObjectTypes::SnakeGen) {
        SnakeGen* snake = dynamic_cast<SnakeGen*>(other_object);
        if (snake) {
            enemy_roster = &snake->GetSoldierRoster();
        }
    }
    else if (other_object->Type() == GameObjectTypes::Castle) {
        Castle* castle = dynamic_cast<Castle*>(other_object);
        if (castle && castle->GetOwner() != Team::P1) {
            enemy_roster = &castle->GetSoldierRoster();
        }
    }

    if (enemy_roster != nullptr) {
        BattleMap::SetCombatRosters(my_roster, enemy_roster);
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
	case GameObjectTypes::Castle:
		return true;
	default: 
		return false;
	}
}

std::vector<Soldier*>& SquirrelGen::GetSoldierRoster() {
	return soldier_roster;
}