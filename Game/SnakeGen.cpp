#include "SnakeGen.h"
#include "SquirrelGen.h"
#include "Castle.h"
#include "States.h"
#include "BattleMap.h"
#include "MainMapState.h"
#include "../Engine/Engine.h"
#include "LinearMovement.h"

SnakeGen::SnakeGen(Math::vec2 position) : CS230::GameObject(position)
{
	AddGOComponent(new CS230::Sprite("Assets/SnakeGen.spt", this));
	AddGOComponent(new LinearMovement(this));
	GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::None));
}

void SnakeGen::ResolveCollision(GameObject* other_object) {
    if (GetPosition().x != other_object->GetPosition().x ||
        GetPosition().y != other_object->GetPosition().y) return;

    std::vector<Soldier*>* my_roster = &this->GetSoldierRoster();
    std::vector<Soldier*>* enemy_roster = nullptr;

    if (other_object->Type() == GameObjectTypes::SquirrelGen) {
        SquirrelGen* squirrel = dynamic_cast<SquirrelGen*>(other_object);
        if (squirrel) {
            enemy_roster = &squirrel->GetSoldierRoster();
        }
    }
    else if (other_object->Type() == GameObjectTypes::Castle) {
        Castle* castle = dynamic_cast<Castle*>(other_object);
        if (castle && castle->GetOwner() != Team::P2) {
            enemy_roster = &castle->GetSoldierRoster();
        }
    }

    if (enemy_roster != nullptr) {
        BattleMap::SetCombatRosters(my_roster, enemy_roster);
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
	case GameObjectTypes::Castle:
		return true;
	default:
		return false;
	}
}

std::vector<Soldier*>& SnakeGen::GetSoldierRoster() {
	return soldier_roster;
}