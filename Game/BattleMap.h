// BattleMap.h
#pragma once
#include "../Engine/GameState.h"
#include "../Engine/TurnManager.h"
#include "../Engine/Camera.h"
#include "Soldier.h"
#include "PlacementTile.h"
#include "WaitingZone.h"

class BattleMap : public CS230::GameState {
public:
	BattleMap();
    void Load() override;
    void Update(double dt) override;
    void Unload() override;
    void Draw() override;

    std::string GetName() override {
        return "BattleMap";
    }

    void LoadSoldiers();
    void StartBattle();

private:
    double background_width = 3000;
    Math::vec2 Waiting_pos1 = { 150, 50 };
    Math::vec2 Waiting_pos2 = { 2400, 50 };
    Math::vec2 Place_pos = { 100, 50 };
    Math::vec2 Battle_pos = { 100, 50 };

    bool p1_ready = false;
    bool p2_ready = false;
    bool isReady() const;
    int p1_score = 0;
    int p2_score = 0;
    double move_speed = 1500;
    int placed_count = 0;


    std::vector<Soldier*> p1_soldiers;
    std::vector<Soldier*> p2_soldiers;
    int battle_index = 0;
    int attack_count = 0;
    double battle_timer = 0.0;
    double end_timer = 0.0;
    bool is_battling = false;
    Soldier* current_p1 = nullptr;
    Soldier* current_p2 = nullptr;
    bool score_updated = false;
    bool battle_finished = false;


    CS230::Camera* camera = nullptr;
    CS230::Texture* button_text = nullptr;
    CS230::Texture* score_text_p1 = nullptr;
    CS230::Texture* score_text_p2 = nullptr;
    CS230::Texture* title_text = nullptr;
    CS230::GameObjectManager* object = nullptr;
    std::vector<PlacementTile*> tiles;
    std::vector<WaitingZone*> zones;
    //Turn turn;

    float offsetX = 0.0f;
    bool transitioning = false;
    TurnManager turnmanager;
    std::vector<Soldier*> soldiers;

    void update_title_text(const std::string& text);
    void update_button_text(const std::string& text);
    void update_score_text(int p1_score, int p2_score);
};