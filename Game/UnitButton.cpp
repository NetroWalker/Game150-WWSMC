#include "UnitButton.h"

Button::Button(Math::vec2 position, int type) : CS230::GameObject(position) {
    switch (type) {
    case 0:
        AddGOComponent(new CS230::Sprite("Assets/Back.spt", this));
        break;
    case 1:
        AddGOComponent(new CS230::Sprite("Assets/Melee.spt", this));
        break;
    case 2:
        AddGOComponent(new CS230::Sprite("Assets/Tank.spt", this));
        break;
    case 3:
        AddGOComponent(new CS230::Sprite("Assets/Ranged.spt", this));
        break;
    }
}
