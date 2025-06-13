//soldier.h
#pragma once
#include "raylib.h"
#include "../Engine/Vec2.h"
#include "../Engine/Matrix.h"
#include "../Engine/GameObject.h"
#include "GameObjectTypes.h"
#include "PlacementTile.h"

enum class SoldierTypes { Ranged, Tank, Melee };
enum class Animals { Squirrel, Snake };
enum class SoldierState { InQueue, Placed, InBattle };

class Soldier : public CS230::GameObject {
public:
    Soldier(Math::vec2 start_position, Animals animal, SoldierTypes type);

    void Update(double dt) override;
    void Draw(Math::TransformationMatrix camera_matrix) override;

    GameObjectTypes Type() override { return GameObjectTypes::Soldier; }
    std::string TypeName() override { return "Soldier"; }
    bool CanCollideWith(GameObjectTypes other_type) override;
    void ResolveCollision(GameObject* other_object) override;

    SoldierTypes GetType() const;
    Animals GetAnimal() const;
    bool win(SoldierTypes other) const;

    void SetOriginalPosition(Math::vec2 pos);
    Math::vec2 GetOriginalPosition() const;

    void SetState(SoldierState new_state);
    SoldierState GetState() const;

    void SetTileList(std::vector<PlacementTile*>* tile_list);

    void SetOccupiedTile(PlacementTile* tile);
    PlacementTile* GetOccupiedTile() const;

    enum class Animations {
        None,
        Attack,
        Move
    };

private:
    CS230::Sprite* sprite = nullptr;
    Animals animal;
    SoldierTypes type;
    Math::vec2 originalPosition;
    SoldierState state = SoldierState::InQueue;
    double scale = 1.0;


    bool is_dragging = false;
    Math::vec2 drag_offset;
    std::vector<PlacementTile*>* tiles = nullptr;
    PlacementTile* occupiedTile = nullptr;
};