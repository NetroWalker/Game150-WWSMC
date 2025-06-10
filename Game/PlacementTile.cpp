#include "PlacementTile.h"      

PlacementTile::PlacementTile(Math::vec2 pos) : GameObject(pos), occupied(false) {
    sprite = new CS230::Sprite("Assets/PlacementTile.spt", this);
    AddGOComponent(sprite);
}

void PlacementTile::Update([[maybe_unsused]]double dt) {}

void PlacementTile::Draw(Math::TransformationMatrix camera_matrix) {
    CS230::GameObject::Draw(camera_matrix);
}

bool PlacementTile::IsPointInside(Math::vec2 point) const {
    Math::vec2 pos = GetPosition();
    Math::vec2 size = static_cast<Math::vec2>(sprite->GetFrameSize());
    Math::vec2 scale = GetScale();

    Math::vec2 halfSize = Math::vec2{ size.x * scale.x * 0.5, size.y * scale.y * 0.5 };
    Math::vec2 topLeft = pos - halfSize;
    Math::vec2 bottomRight = pos + halfSize;

    return (point.x >= topLeft.x && point.x <= bottomRight.x &&
        point.y >= topLeft.y && point.y <= bottomRight.y);
}

void PlacementTile::SetOccupied(bool occupiedStatus) {
    occupied = occupiedStatus;
}

bool PlacementTile::IsOccupied() const {
    return occupied;
}
