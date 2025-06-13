//UnitProduction.h
#pragma once
#include "../Engine/GameState.h"
#include "../Engine/TurnManager.h"
#include "../Engine/Camera.h"
#include "../Engine/GameObjectManager.h"
#include "Background.h"
#include "Soldier.h"
#include "PlacementTile.h"
#include "WaitingZone.h"
#include "Material.h"
#include "UnitButton.h"
#include <vector>
#include <string>

class MainMapState;

class UnitProduction {
public:
    UnitProduction();
    void Init();
    void Update(double dt);
    void Unload();
    void Draw();

    void ProductUnit(const Vector2& mouse_pos);
    void WaitingUnit();

    void SaveRoster();
    void LoadRoster();

    void DrawButton();
    void SetTarget(CS230::GameObject* target, std::vector<Soldier*>* roster, Stone* resource_ptr);
    void SetMainMap(MainMapState* ptr) { main_map_state = ptr; }

private:
    int soldier_max = 5;
    double background_width = 3000;
    Math::vec2 Waiting_pos1 = { 150, 50 };
    Math::vec2 Waiting_pos2 = { 2400, 50 };
    Math::vec2 Place_pos = { 100, 50 };

    Rectangle back_button{};
    Rectangle melee_button{};
    Rectangle ranged_button{};
    Rectangle tank_button{};

    double back_button_size = 100;
    double unit_button_size = 200;
    double button_offset = 5;

    Background* background = nullptr;
    CS230::Camera* camera = nullptr;
    CS230::GameObjectManager* object = nullptr;
    std::vector<PlacementTile*> tiles;
    std::vector<WaitingZone*> zones;
    //Turn turn;

    float offsetX = 0.0f;
    bool transitioning = false;
    TurnManager turnmanager;
    std::vector<Soldier*> soldiers;
    SoldierTypes type;
    Animals animal;
    MainMapState* main_map_state = nullptr;

    std::string notification_message;
    double notification_timer = 0.0;

    CS230::GameObject* selected_target = nullptr;
    std::vector<Soldier*>* target_roster = nullptr;
    Stone* current_resource = nullptr;
};