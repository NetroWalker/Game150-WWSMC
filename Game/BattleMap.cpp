//Battlemap.cpp
#include "../Engine/Engine.h"
#include "../Engine/ShowCollision.h"
#include "../Engine/TurnManager.h"
#include "BattleMap.h"
#include "Background.h"
#include "Fonts.h"
#include "States.h"
#include "MainMapState.h"
#include <iostream>

BattleMap::BattleMap() {}
void BattleMap::LoadSoldiersForTurn(Turn attacking_turn) {
	this->attacking_turn = attacking_turn;
}

void BattleMap::Load() {
	AddGSComponent(new Background());
	AddGSComponent(new CS230::GameObjectManager());
	AddGSComponent(new CS230::Camera({{ 0.0, 0.0 }, {static_cast<double>(Engine::GetWindow().GetSize().x), static_cast<double>(Engine::GetWindow().GetSize().y)} }));
	auto background = Engine::GetGameStateManager().GetGSComponent<Background>();
	background->Add("Assets/Battlemap.png", 1);

	camera = Engine::GetGameStateManager().GetGSComponent<CS230::Camera>();
	camera->SetPosition({ 0, 0 });
	camera->SetLimit({ { 0, 0 }, background->GetSize() - Engine::GetWindow().GetSize() });

	object = Engine::GetGameStateManager().GetGSComponent<CS230::GameObjectManager>();

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

	LoadSoldiers();
	update_title_text("P1 Placement");
	update_button_text("READY!");
	update_score_text(0, 0);
	turnmanager.SetTurn(this->attacking_turn);
	turnmanager.SetTransition(true);

	turnmanager.SetTurn(Turn::P1);
	turnmanager.SetTransition(true);
}

void BattleMap::Update(double dt) {
	UpdateGSComponents(dt);
	// 화면 흔들림 처리
	if (shake_timer < shake_duration) {
		shake_timer += dt;
		float offsetX = static_cast<float>((rand() % 200 - 100) / 100.0) * shake_magnitude;
		float offsetY = static_cast<float>((rand() % 200 - 100) / 100.0) * shake_magnitude;
		camera->SetPosition({ original_camera_pos.x + offsetX, original_camera_pos.y + offsetY });
	}
	else if (shake_duration > 0.0f) {
		camera->SetPosition(original_camera_pos);
		shake_duration = 0.0f;
	}

	const Turn currentTurn = turnmanager.GetCurrentTurn();
	const bool isTransition = turnmanager.IsTransitioning();
	const bool ready = isReady();

	const auto windowSize = Engine::GetWindow().GetSize();
	const Vector2 mouse = GetMousePosition();

	const Math::ivec2 buttonPos = isTransition ?
		Math::ivec2{ windowSize.x / 2 - button_text->GetSize().x / 2, (Engine::GetWindow().GetSize().y / 8) * 7 - 70 } :
		Math::ivec2{ windowSize.x - button_text->GetSize().x - 50, (Engine::GetWindow().GetSize().y / 8) * 7 - 70 };

	const Rectangle buttonRect = {
		static_cast<float>(buttonPos.x),
		static_cast<float>(buttonPos.y),
		static_cast<float>(button_text->GetSize().x),
		static_cast<float>(button_text->GetSize().y)
	};
	if (battle_finished) {
		end_timer += dt;

		if (end_timer > 3.0) { // 3초 후 맵으로 돌아감

			// 1. MainMapState의 인스턴스를 가져옵니다.
			MainMapState* main_map = dynamic_cast<MainMapState*>(Engine::GetGameStateManager().GetGameState(STATE_MAIN_MAP));

			if (main_map != nullptr) {
				// 2. 승/패/무승부를 결정합니다.
				BattleOutcome outcome;
				if (p1_score > p2_score) {
					outcome = BattleOutcome::P1_WINS;
				}
				else if (p2_score > p1_score) {
					outcome = BattleOutcome::P2_WINS;
				}
				else {
					outcome = BattleOutcome::DRAW; // 무승부
				}

				// 3. MainMapState에 전투 결과를 알립니다.
				main_map->SetBattleOutcome(outcome);
			}

			// 4. MainMapState로 전환합니다.
			Engine::GetGameStateManager().SetNextGameState(STATE_MAIN_MAP);
		}
	}
	// P1 Placement Turn
	if (currentTurn == Turn::P1 && isTransition && !isReady()) {
		update_title_text("P1 Placement");
		update_button_text("Start!");

		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, buttonRect)) {
			turnmanager.StartTurn();
		}
	}
	else if (currentTurn == Turn::P1 && !isTransition && !isReady()) {
		update_button_text("Ready!");

		placed_count = 0;
		for (auto soldier : p1_soldiers) {
			if (soldier->GetState() == SoldierState::Placed) {
				++placed_count;
			}
		}

		if (placed_count == 5) {
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, buttonRect)) {
				p1_ready = true;
				turnmanager.EndTurn();
			}
		}

		for (auto soldier : p1_soldiers) {
			if (soldier->GetState() == SoldierState::Placed && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
				Math::vec2 size = static_cast<Math::vec2>(soldier->GetGOComponent<CS230::Sprite>()->GetFrameSize());
				Math::vec2 halfSize = { size.x * soldier->GetScale().x * 0.5, size.y * soldier->GetScale().y * 0.5 };
				Rectangle soldierRect = {
					static_cast<float>(soldier->GetPosition().x - halfSize.x),
					static_cast<float>(soldier->GetPosition().y),
					static_cast<float>(size.x * soldier->GetScale().x),
					static_cast<float>(size.y * soldier->GetScale().y)
				};

				if (CheckCollisionPointRec(mouse, soldierRect)) {
					if (soldier->GetOccupiedTile() != nullptr) {
						soldier->GetOccupiedTile()->SetOccupied(false);
						soldier->SetOccupiedTile(nullptr);
					}
					soldier->SetState(SoldierState::InQueue);
					soldier->SetPosition(soldier->GetOriginalPosition());
					break;
				}
			}
		}
	}
	// P2 Placement Turn
	else if (currentTurn == Turn::P2 && isTransition && !isReady()) {
		update_title_text("P2 Placement");
		update_button_text("Start!");
		camera->SetPosition({ 1500, 0 });

		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, buttonRect)) {
			turnmanager.StartTurn();
		}

	}
	else if (currentTurn == Turn::P2 && !isTransition && !isReady()) {
		update_button_text("Ready!");

		placed_count = 0;
		for (auto soldier : p2_soldiers) {
			if (soldier->GetState() == SoldierState::Placed) {
				++placed_count;
			}
		}

		if (placed_count == 5) {
			if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, buttonRect)) {
				p2_ready = true;
				turnmanager.ShowTransition();
			}
		}

		for (auto soldier : p2_soldiers) {
			if (soldier->GetState() == SoldierState::Placed && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
				Math::vec2 size = static_cast<Math::vec2>(soldier->GetGOComponent<CS230::Sprite>()->GetFrameSize());
				Math::vec2 halfSize = { size.x * std::abs(soldier->GetScale().x) * 0.5, size.y * soldier->GetScale().y * 0.5 };
				Math::vec2 cam_offset = camera->GetPosition();
				Rectangle soldierRect = {
					static_cast<float>(soldier->GetPosition().x - halfSize.x - cam_offset.x),
					static_cast<float>(soldier->GetPosition().y),
					static_cast<float>(size.x * std::abs(soldier->GetScale().x)),
					static_cast<float>(size.y * soldier->GetScale().y)
				};

				if (CheckCollisionPointRec(mouse, soldierRect)) {
					if (soldier->GetOccupiedTile() != nullptr) {
						soldier->GetOccupiedTile()->SetOccupied(false);
						soldier->SetOccupiedTile(nullptr);
					}
					soldier->SetState(SoldierState::InQueue);
					soldier->SetPosition(soldier->GetOriginalPosition());
					break;
				}
			}
		}
	}
	// Battle Turn
	else if (isTransition && isReady()) {
		update_title_text("Ready");
		update_button_text("Start!");
		camera->SetPosition({ 750, 0 });

		for (auto* tile : tiles) {
			tile->SetPosition({ 3000, 0 });
		}
		for (auto* zone : zones) {
			zone->SetPosition({ 3000, 0 });
		}

		if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, buttonRect)) {
			turnmanager.StartTurn();
		}
		StartBattle();
	}
	else if (!isTransition && isReady()) {
		if (attack_count < 2) {
			if (current_p1->GetPosition().x < 1300 && current_p2->GetPosition().x > 1700) {
				auto pos1 = current_p1->GetPosition();
				auto pos2 = current_p2->GetPosition();
				current_p1->SetPosition({ pos1.x + move_speed * dt, pos1.y });
				current_p2->SetPosition({ pos2.x - move_speed * dt, pos2.y });
				if (current_p1->GetPosition().x > 1300 && current_p2->GetPosition().x < 1700) {
					current_p1->GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Soldier::Animations::Attack));
					current_p2->GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Soldier::Animations::Attack));
					++attack_count;
				}
			}
			else if (current_p1->GetPosition().x > 1300 && current_p2->GetPosition().x < 1700 &&
				current_p1->GetGOComponent<CS230::Sprite>()->AnimationEnded() && current_p1->GetGOComponent<CS230::Sprite>()->AnimationEnded())
			{
				current_p1->GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Soldier::Animations::Attack));
				current_p2->GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Soldier::Animations::Attack));
				StartShake(0.3f, 10.0f); // 흔들림 추가

				++attack_count;
			}
		}
		else {
			current_p1->GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Soldier::Animations::None));
			current_p2->GetGOComponent<CS230::Sprite>()->PlayAnimation(static_cast<int>(Soldier::Animations::None));
			battle_timer += dt;
			if (battle_timer < 1.0) {
				if (current_p1->win(current_p2->GetType())) {
					auto pos1 = current_p1->GetPosition();
					auto pos2 = current_p2->GetPosition();
					current_p1->SetPosition({ pos1.x - move_speed * dt, pos1.y });
					current_p2->SetPosition({ pos2.x + move_speed * dt, pos2.y + move_speed * dt });
				}
				else if (current_p2->win(current_p1->GetType())) {
					auto pos1 = current_p1->GetPosition();
					auto pos2 = current_p2->GetPosition();
					current_p1->SetPosition({ pos1.x - move_speed * dt, pos1.y + move_speed * dt });
					current_p2->SetPosition({ pos2.x + move_speed * dt, pos2.y });
				}
				else {
					auto pos1 = current_p1->GetPosition();
					auto pos2 = current_p2->GetPosition();
					current_p1->SetPosition({ pos1.x - move_speed * dt, pos1.y + move_speed * dt });
					current_p2->SetPosition({ pos2.x + move_speed * dt, pos2.y + move_speed * dt });
				}
			}
			else {
				if (!score_updated) {
					if (current_p1->win(current_p2->GetType())) {
						p1_score++;
					}
					else if (current_p2->win(current_p1->GetType())) {
						p2_score++;
					}
					update_score_text(p1_score, p2_score);
					score_updated = true;
				}

				if (battle_timer > 1.5) {
					battle_index++;

					if (battle_index >= 5) {
						is_battling = false;
						battle_finished = true;

						if (p1_score > p2_score) {
							update_title_text("P1 Win!");
						}
						else if (p1_score < p2_score) {
							update_title_text("P2 Win!");
						}
						else {
							update_title_text("Draw!");
						}

						end_timer += dt;

						if (end_timer > 3.0) {
							Engine::GetGameStateManager().SetNextGameState(STATE_MAIN_MAP);
						}
					}
					else {
						current_p1 = p1_soldiers[battle_index];
						current_p2 = p2_soldiers[battle_index];

						current_p1->SetPosition({ 500, 200 });
						current_p2->SetPosition({ 2500, 200 });

						battle_timer = 0.0;
						score_updated = false;
						attack_count = 0;
						end_timer = 0.0; 
					}
				}
			}
		}
	}
}

void BattleMap::Draw() {
	Engine::GetWindow().Clear(0x000000FF);

	//Math::TransformationMatrix camera_matrix = Engine::GetGameStateManager().GetGSComponent<CS230::Camera>()->GetMatrix();
	Engine::GetGameStateManager().GetGSComponent<Background>()->Draw(*Engine::GetGameStateManager().GetGSComponent<CS230::Camera>());
	Engine::GetGameStateManager().GetGSComponent<CS230::GameObjectManager>()->DrawAll(Engine::GetGameStateManager().GetGSComponent<CS230::Camera>()->GetMatrix());

	if (turnmanager.IsTransitioning()) {
		DrawRectangle(0, 0, Engine::GetWindow().GetSize().x, Engine::GetWindow().GetSize().y, Color{ 100, 100, 100, 255 });

		Math::ivec2 buttonPos = {
			Engine::GetWindow().GetSize().x / 2 - button_text->GetSize().x / 2,
			(Engine::GetWindow().GetSize().y / 8) * 7 - 70
		};

    
		Rectangle buttonRect = {
			static_cast<float>(buttonPos.x),
			static_cast<float>(buttonPos.y),
			static_cast<float>(button_text->GetSize().x),
			static_cast<float>(button_text->GetSize().y)
		};

		Color hoverColor = CheckCollisionPointRec(GetMousePosition(), buttonRect) ? DARKGRAY : LIGHTGRAY;
		DrawRectangleRec(buttonRect, hoverColor);

		button_text->Draw(Math::TranslationMatrix(Math::ivec2{
			Engine::GetWindow().GetSize().x / 2 - button_text->GetSize().x / 2,
			Engine::GetWindow().GetSize().y / 8 }));

		title_text->Draw(Math::TranslationMatrix(Math::ivec2{
			Engine::GetWindow().GetSize().x / 2 - title_text->GetSize().x / 2,
			Engine::GetWindow().GetSize().y / 2 })
			);
	}
	else if (!turnmanager.IsTransitioning()) {
		if (!isReady()) {
			Math::ivec2 buttonPos = {
				Engine::GetWindow().GetSize().x - button_text->GetSize().x - 50,
				(Engine::GetWindow().GetSize().y / 8) * 7 - 70
			};

			Rectangle buttonRect = {
				static_cast<float>(buttonPos.x),
				static_cast<float>(buttonPos.y),
				static_cast<float>(button_text->GetSize().x),
				static_cast<float>(button_text->GetSize().y)
			};

			if (placed_count == 5) {
				Color hoverColor = CheckCollisionPointRec(GetMousePosition(), buttonRect) ? DARKGRAY : LIGHTGRAY;
				DrawRectangleRec(buttonRect, hoverColor);
			}
			else {
				Color basicColor = { 100, 100, 100, 100 };
				DrawRectangleRec(buttonRect, basicColor);
			}

			button_text->Draw(Math::TranslationMatrix(Math::ivec2{
				Engine::GetWindow().GetSize().x - button_text->GetSize().x - 50,
				Engine::GetWindow().GetSize().y / 8 }));
		}

		if (battle_index >= 5) {
			title_text->Draw(Math::TranslationMatrix(Math::ivec2{
				Engine::GetWindow().GetSize().x / 2 - title_text->GetSize().x / 2,
				Engine::GetWindow().GetSize().y / 2 })
			);
		}
	}
	score_text_p1->Draw(Math::TranslationMatrix(Math::ivec2{ 10, Engine::GetWindow().GetSize().y - score_text_p1->GetSize().y - 5 }));
	score_text_p2->Draw(Math::TranslationMatrix(Math::ivec2{ Engine::GetWindow().GetSize().x - score_text_p2->GetSize().x - 10, Engine::GetWindow().GetSize().y - score_text_p1->GetSize().y - 5}));
}

void BattleMap::Unload() {
	ClearGSComponents();
}
void BattleMap::update_title_text(const std::string& text) {
	title_text = Engine::GetFont(static_cast<int>(Fonts::Simple)).PrintToTexture(text, 0xFFFFFFFF);
}

void BattleMap::update_button_text(const std::string& text) {
	button_text = Engine::GetFont(static_cast<int>(Fonts::Simple)).PrintToTexture(text, 0xFFFFFFFF);
}

void BattleMap::update_score_text(int p1_score, int p2_score) {
	score_text_p1 = Engine::GetFont(static_cast<int>(Fonts::Simple)).PrintToTexture("P1: " + std::to_string(p1_score), 0xFFFFFFFF);
	score_text_p2 = Engine::GetFont(static_cast<int>(Fonts::Simple)).PrintToTexture("P2: " + std::to_string(p2_score), 0xFFFFFFFF);
}

void BattleMap::LoadSoldiers() {
	std::vector<Math::vec2> squirrelPositions = {
		{ Waiting_pos1.x, Waiting_pos1.y },
		{ Waiting_pos1.x + 200, Waiting_pos1.y },
		{ Waiting_pos1.x + 400, Waiting_pos1.y },
		{ Waiting_pos1.x + 600, Waiting_pos1.y },
		{ Waiting_pos1.x + 800, Waiting_pos1.y },
	};

	std::vector<Math::vec2> snakePositions = {
		{ Waiting_pos2.x, Waiting_pos2.y },
		{ Waiting_pos2.x - 200, Waiting_pos2.y },
		{ Waiting_pos2.x - 400, Waiting_pos2.y },
		{ Waiting_pos2.x - 600, Waiting_pos2.y },
		{ Waiting_pos2.x - 800, Waiting_pos2.y },
	};

	for (int i = 0; i < 5; ++i) {
		auto* s = new Soldier(squirrelPositions[i], Animals::Squirrel, static_cast<SoldierTypes>(i % 3));
		s->SetTileList(&tiles);
		object->Add(s);
		p1_soldiers.push_back(s);
		soldiers.push_back(s);
	}

	for (int i = 0; i < 5; ++i) {
		auto* s = new Soldier(snakePositions[i], Animals::Snake, static_cast<SoldierTypes>(i % 3));
		s->SetTileList(&tiles);
		object->Add(s);
		p2_soldiers.push_back(s);
		soldiers.push_back(s);
	}
}

bool BattleMap::isReady() {
	return p1_ready && p2_ready;
}

void BattleMap::StartBattle() {

	for (Soldier* s : p1_soldiers) {
		if (s->GetState() == SoldierState::Placed) {
			s->SetState(SoldierState::InBattle);
		}
	}
	for (Soldier* s : p2_soldiers) {
		if (s->GetState() == SoldierState::Placed) {
			s->SetState(SoldierState::InBattle);
		}
	}

	if (p1_soldiers.size() < 5 || p2_soldiers.size() < 5) {
		is_battling = false;
		return;
	}

	std::sort(p1_soldiers.begin(), p1_soldiers.end(), [](Soldier* a, Soldier* b) {
		return a->GetPosition().x > b->GetPosition().x;
		});

	std::sort(p2_soldiers.begin(), p2_soldiers.end(), [](Soldier* a, Soldier* b) {
		return a->GetPosition().x < b->GetPosition().x;
		});

	for (int i = 0; i < 5; ++i) {
		p1_soldiers[i]->SetPosition({ -500, 0 });
		p2_soldiers[i]->SetPosition({ 3500, 0 });
	}

	update_score_text(p1_score, p2_score);

	battle_index = 0;
	battle_timer = 0.0;
	is_battling = true;
	score_updated = false;

	current_p1 = p1_soldiers[battle_index];
	current_p2 = p2_soldiers[battle_index];

	current_p1->SetPosition({ 500, 200 });
	current_p2->SetPosition({ 2500, 200 });
}

void BattleMap::StartShake(float duration, float magnitude) {
	shake_duration = duration;
	shake_timer = 0.0f;
	shake_magnitude = magnitude;
	original_camera_pos = camera->GetPosition();
}
