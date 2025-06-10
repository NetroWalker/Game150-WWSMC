// Soldierl.cpp
#include "Soldier.h"
#include "../Engine/Engine.h"
#include "../Engine/Camera.h"

static Math::vec2 mouse_pos;

Soldier::Soldier(Math::vec2 start_position, Animals animal, SoldierTypes type)
    : GameObject(start_position), animal(animal), type(type), originalPosition(start_position) {

    std::string path;
    if (animal == Animals::Squirrel) {
        if (type == SoldierTypes::Melee)
            path = "Assets/SquirrelM.spt";
        else if (type == SoldierTypes::Ranged)
            path = "Assets/SquirrelR.spt";
        else if (type == SoldierTypes::Tank)
            path = "Assets/SquirrelT.spt";
    }
    else if (animal == Animals::Snake) {
        if (type == SoldierTypes::Melee)
            path = "Assets/SnakeM.spt";
        else if (type == SoldierTypes::Ranged)
            path = "Assets/SnakeR.spt";
        else if (type == SoldierTypes::Tank)
            path = "Assets/SnakeT.spt";
    }

    sprite = new CS230::Sprite(path, this);
    AddGOComponent(sprite);
    GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Animations::None));
    SetScale({ 0.25, 0.25 });
    SetState(SoldierState::InQueue);
}

void Soldier::SetTileList(std::vector<PlacementTile*>* tileList) {
    tiles = tileList;
}

void Soldier::Update(double dt) {
    CS230::GameObject::Update(dt);

    Vector2 raw_mouse = GetMousePosition();
    float flipped_y = Engine::GetWindow().GetSize().y - raw_mouse.y;

    Math::vec2 cam_pos = Engine::GetGameStateManager().GetGSComponent<CS230::Camera>()->GetPosition();
    mouse_pos = Math::vec2{ raw_mouse.x + cam_pos.x, flipped_y + cam_pos.y };

    Math::vec2 size = static_cast<Math::vec2>(sprite->GetFrameSize());
    Math::vec2 scale = GetScale();
    Math::vec2 halfSize = { size.x * std::abs(scale.x) * 0.5, size.y * std::abs(scale.y) * 0.5 };
    Math::vec2 topLeft = { GetPosition().x - halfSize.x,GetPosition().y };
    Math::vec2 bottomRight = { GetPosition().x + halfSize.x, GetPosition().y + halfSize.y * 2 };

    bool mouseOver = (mouse_pos.x >= topLeft.x && mouse_pos.x <= bottomRight.x &&
        mouse_pos.y <= bottomRight.y && mouse_pos.y >= topLeft.y);


    if (state == SoldierState::InQueue) {
        if (!is_dragging && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && mouseOver) {
            is_dragging = true;
            drag_offset = Math::vec2{ GetPosition().x - mouse_pos.x, GetPosition().y - mouse_pos.y };
            if (occupiedTile != nullptr) {
                occupiedTile->SetOccupied(false);
                occupiedTile = nullptr;
            }
        }

        if (is_dragging && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            SetPosition(mouse_pos + drag_offset);
        }

        if (is_dragging && IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            is_dragging = false;

            PlacementTile* targetTile = nullptr;
            if (this->tiles) {
                for (auto tile : *tiles) {
                    if (tile->IsPointInside(mouse_pos) && !tile->IsOccupied()) {
                        targetTile = tile;
                        break;
                    }
                }
            }

            if (targetTile != nullptr) {
                Math::vec2 tilePos = targetTile->GetPosition();
                tilePos.y -= 50;
                SetPosition(tilePos);
                SetState(SoldierState::Placed);
                targetTile->SetOccupied(true);
                SetOccupiedTile(targetTile);
            }
            else {
                SetPosition(originalPosition);
                SetState(SoldierState::InQueue);
            }
        }
    }
}

void Soldier::Draw(Math::TransformationMatrix camera_matrix) {
    CS230::GameObject::Draw(camera_matrix);
}

bool Soldier::CanCollideWith(GameObjectTypes) {
    return false;
}

void Soldier::ResolveCollision([[maybe_unused]]GameObject* other_object) {
}

SoldierTypes Soldier::GetType() const {
    return type; 
}

Animals Soldier::GetAnimal() const {
    return animal; 
}

bool Soldier::win(SoldierTypes other) const {
    return (type == SoldierTypes::Tank && other == SoldierTypes::Ranged) ||
        (type == SoldierTypes::Ranged && other == SoldierTypes::Melee) ||
        (type == SoldierTypes::Melee && other == SoldierTypes::Tank);
}

void Soldier::SetOriginalPosition(Math::vec2 pos) {
    originalPosition = pos;
}

Math::vec2 Soldier::GetOriginalPosition() const {
    return originalPosition;
}

void Soldier::SetState(SoldierState new_state) {
    state = new_state;

    switch (state) {
    case SoldierState::InQueue:
        if (animal == Animals::Squirrel)
            SetScale({ 0.25, 0.25 });
        else
            SetScale({ -0.25, 0.25 });

        if (occupiedTile != nullptr) {
            occupiedTile->SetOccupied(false);
            occupiedTile = nullptr;
        }
        break;
    case SoldierState::Placed:
        if (animal == Animals::Squirrel)
            SetScale({ 0.5, 0.5 });
        else
            SetScale({ -0.5, 0.5 });
        break;
    case SoldierState::InBattle:
        if (animal == Animals::Squirrel)
            SetScale({ 1, 1 });
        else
            SetScale({ -1, 1 });

        if (occupiedTile != nullptr) {
            occupiedTile->SetOccupied(false);
            occupiedTile = nullptr;
        }
        break;
    }
}

SoldierState Soldier::GetState() {
    return state;
}

void Soldier::SetOccupiedTile(PlacementTile* tile) {
    occupiedTile = tile;
}

PlacementTile* Soldier::GetOccupiedTile() const {
    return occupiedTile;
}