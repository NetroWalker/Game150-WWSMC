//UnitProduction.cpp
#include "UnitProduction.h"
#include "Background.h"
#include "Fonts.h"
#include "States.h"
#include "UnitButton.h"
#include "../Engine/Engine.h"
#include "MainMapState.h"

UnitProduction::UnitProduction() {
	float window_width = static_cast<float>(Engine::GetWindow().GetSize().x);
	float window_height = static_cast<float>(Engine::GetWindow().GetSize().y);
	float unit_button_size_f = static_cast<float>(unit_button_size);
	float back_button_size_f = static_cast<float>(back_button_size);
	float button_offset_f = static_cast<float>(button_offset);

	back_button = { button_offset_f, button_offset_f, back_button_size_f, back_button_size_f };
	melee_button = { button_offset_f, back_button_size_f + button_offset_f * 2, unit_button_size_f, unit_button_size_f };
	tank_button = { button_offset_f, back_button_size_f + unit_button_size_f + button_offset_f * 3, unit_button_size_f, unit_button_size_f };
	ranged_button = { button_offset_f, back_button_size_f + unit_button_size_f * 2 + button_offset_f * 4, unit_button_size_f, unit_button_size_f };
}

void UnitProduction::Init() {
	Unload();

	background = new Background();
	camera = new CS230::Camera({ {0, 0}, {0, 0} });
	object = new CS230::GameObjectManager();

	background->Add("Assets/Battlemap.png", 1);

	Math::vec2 window = static_cast<Math::vec2>(Engine::GetWindow().GetSize());
	auto back_b = new Button({ button_offset, window.y - back_button_size - button_offset }, 0);
	back_b->SetScale({ 0.5, 0.5 });
	object->Add(back_b);
	object->Add(new Button({ button_offset, window.y - back_button_size - unit_button_size - button_offset }, 1));
	object->Add(new Button({ button_offset, window.y - back_button_size - unit_button_size * 2 - button_offset * 2 }, 2));
	object->Add(new Button({ button_offset, window.y - back_button_size - unit_button_size * 3 - button_offset * 3 }, 3));

	Vector2 centers[5] = {
		{400, 450}, {630, 350}, {860, 450}, {1090, 350}, {1320, 450}
	};
	for (int i = 0; i < 5; ++i) {
		auto* tile1 = new PlacementTile({ centers[i].x, centers[i].y });
		auto* tile2 = new PlacementTile({ background_width - centers[i].x, centers[i].y });
		tiles.push_back(tile1);
		tiles.push_back(tile2);
		object->Add(tile1);
		object->Add(tile2);
	}
	auto* zone1 = new WaitingZone({ 0, 0 });
	auto* zone2 = new WaitingZone({ 1500, 0 });
	zones.push_back(zone1);
	zones.push_back(zone2);
	object->Add(zone1);
	object->Add(zone2);

	LoadRoster();
}

void UnitProduction::Update(double dt) {
	if (notification_timer > 0) {
		notification_timer -= dt;
		if (notification_timer <= 0) {
			notification_message.clear();
		}
	}

	object->UpdateAll(dt);
	ProductUnit(GetMousePosition());
}

void UnitProduction::Draw() {
	Engine::GetWindow().Clear(0x000000FF);
	DrawButton();

	if (background && camera) {
		background->Draw(*camera);
	}

	if (object && camera) {
		object->DrawAll(camera->GetMatrix());
	}

	if (notification_timer > 0) {
		int text_width = MeasureText(notification_message.c_str(), 30);
		DrawRectangle(GetScreenWidth() / 2 - text_width / 2 - 10,
			GetScreenHeight() / 2 - 25, text_width + 20, 50, Fade(BLACK, 0.7f));
		DrawText(notification_message.c_str(),
			GetScreenWidth() / 2 - text_width / 2, GetScreenHeight() / 2 - 15, 30, YELLOW);
	}
}

void UnitProduction::ProductUnit(const Vector2& mouse_pos) {
	if (!IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
		return;
	}

	if (turn == Turn::P1) {
		animal = Animals::Squirrel;
	}
	else {
		animal = Animals::Snake;
	}

	bool button_clicked = true;
	if (CheckCollisionPointRec(mouse_pos, melee_button)) {
		type = SoldierTypes::Melee;
	}
	else if (CheckCollisionPointRec(mouse_pos, tank_button)) {
		type = SoldierTypes::Tank;
	}
	else if (CheckCollisionPointRec(mouse_pos, ranged_button)) {
		type = SoldierTypes::Ranged;
	}
	else if (CheckCollisionPointRec(mouse_pos, back_button)) {
		SaveRoster();
		Unload();

		if (main_map_state) {
			main_map_state->isProducingUnit = false;
			main_map_state->unit_production_ui_initialized = false;
		}
		return;
	}
	else {
		button_clicked = false;
	}

	if (button_clicked) {
		if (soldiers.size() >= soldier_max) {
			notification_message = "Cannot produce: Maximum units reached!";
			notification_timer = 2.0;
		}
		else {
			Soldier* s = new Soldier({ 0, 0 }, animal, type);
			soldiers.push_back(s);
			object->Add(s);

			WaitingUnit();
		}
	}
}

void UnitProduction::Unload() {
	for (auto* soldier : soldiers) {
		delete soldier;
	}
	soldiers.clear();

	for (auto* tile : tiles) {
		delete tile;
	}
	tiles.clear();

	for (auto* zone : zones) {
		delete zone;
	}
	zones.clear();

	delete object;
	delete camera;
	delete background;
	object = nullptr;
	camera = nullptr;
	background = nullptr;
}

void UnitProduction::WaitingUnit() {
	int index = 0;

	for (auto* soldier : soldiers) {
		if (soldier->GetState() == SoldierState::InQueue) {
			Math::vec2 pos = { Waiting_pos1.x + (index * 200), Waiting_pos1.y };
			soldier->SetPosition(pos);
			soldier->SetOriginalPosition(pos);
			index++;
		}
	}
}

void UnitProduction::SaveRoster() {
	if (!target_roster) return;

	target_roster->clear();
	for (auto* soldier : soldiers) {
		Soldier* clone = new Soldier({ 0,0 }, soldier->GetAnimal(), soldier->GetType());
		target_roster->push_back(clone);
	}
	soldiers.clear();
}

void UnitProduction::LoadRoster() {
	if (!target_roster) return;

	soldiers.clear();

	for (auto* old : *target_roster) {
		Soldier* new_soldier = new Soldier({ 0, 0 }, old->GetAnimal(), old->GetType());
		soldiers.push_back(new_soldier);
		object->Add(new_soldier);
		new_soldier->SetTileList(&tiles);
	}
	WaitingUnit();
}

void UnitProduction::DrawButton() {
	DrawRectangleRec(back_button, BLACK);
	DrawRectangleRec(melee_button, BLACK);
	DrawRectangleRec(tank_button, BLACK);
	DrawRectangleRec(ranged_button, BLACK);
}

void UnitProduction::SetTarget(CS230::GameObject* target, std::vector<Soldier*>* roster, Stone* resource_ptr, Turn current_turn) {
	selected_target = target;
	target_roster = roster;
	current_resource = resource_ptr;
	turn = current_turn;
}
