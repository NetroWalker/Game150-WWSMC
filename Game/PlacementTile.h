#pragma once
#include "../Engine/Vec2.h"
#include "../Engine/Matrix.h"
#include "../Engine/GameObject.h"
#include "GameObjectTypes.h"

class PlacementTile : public CS230::GameObject {
public:
    PlacementTile(Math::vec2 pos);

    void Update(double dt) override;
    void Draw(Math::TransformationMatrix camera_matrix);

    GameObjectTypes Type() override { return GameObjectTypes::PlacementTile; }
    std::string TypeName() override { return "PlacementTile"; }

    void SetOccupied(bool occupiedStatus);
    bool IsOccupied() const;

    bool IsPointInside(Math::vec2 point) const;
    bool occupied;
private:
    CS230::Sprite* sprite = nullptr;
};