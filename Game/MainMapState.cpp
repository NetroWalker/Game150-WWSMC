#include "MainMapState.h"
#include "BattleMap.h"
#include "States.h"
#include "SquirrelGen.h"
#include "SnakeGen.h"
#include "LinearMovement.h"
#include "../Engine/Collision.h"
#include "../Engine/ShowCollision.h"
#include "Castle.h"
#include "Material.h"
#include "../Engine/Camera.h"
#include <cmath>
#include "UnitProduction.h"

MainMapState::MainMapState(int sw, int sh) :
    screenWidth(sw), screenHeight(sh),
    radiusX(200.0f), radiusY(200.0f),
    gameMap(Vector2{ screenWidth / 2.0f - ((8 - 1) * 200.0f * 1.5f) / 2.0f,
                     screenHeight / 2.0f - ((10 - 1) * 200.0f * sqrtf(3.0f) * 0.5f) / 2.0f },
        200.0f, 200.0f, 8, 10, true),
    generalSelected(false)
{
}

MainMapState::~MainMapState() {
    delete player1_resources;
    delete player2_resources;
}

void MainMapState::Load() {
    Engine::GetLogger().LogEvent(GetName() + " Load");
    AddGSComponent(new CS230::Camera({ {0,0}, {0,0} }));
    AddGSComponent(new CS230::GameObjectManager());
    CS230::GameObjectManager* GOM = GetGSComponent<CS230::GameObjectManager>();

    player1_resources = new Stone();
    player2_resources = new Stone();

    gameMap.SetPoint();
    HexTile* tile_end = gameMap.GetTileAt(7, 9);
    if (!tile_end) { return; }

    Math::vec2 p1_start_pos = gameMap.GetTiles()[0].center;
    Math::vec2 p2_start_pos = tile_end->center;

    this->player1 = new SquirrelGen(p1_start_pos);
    this->player2 = new SnakeGen(p2_start_pos);
    Castle* initial_castle1 = new Castle(p1_start_pos, "Assets/castle_me.spt");
    Castle* initial_castle2 = new Castle(p2_start_pos, "Assets/castle_enemy.spt");

    player1_castles.push_back(initial_castle1);
    player2_castles.push_back(initial_castle2);

    GOM->Add(initial_castle1);
    GOM->Add(initial_castle2);
    GOM->Add(player1);
    GOM->Add(player2);

    player1->SetScale({ 0.3, 0.3 });
    player2->SetScale({ 0.3, 0.3 });
    initial_castle1->SetScale({ 0.7, 0.7 });
    initial_castle2->SetScale({ 0.7, 0.7 });

    generalSelected = false;
    movableTiles.clear();
    startingTile = nullptr;
}

void MainMapState::Update(double dt) {
    if (!isProducingUnit && unit_production_ui_initialized) {
        unit_production_ui.Unload();
        unit_production_ui_initialized = false;

    }
    if (isProducingUnit) {
        unit_production_ui.Update(dt);
        return;
    }

    if (notification_timer > 0) {
        notification_timer -= dt;
        if (notification_timer <= 0) notification_message.clear();
    }

    CS230::GameObjectManager* GOM = GetGSComponent<CS230::GameObjectManager>();
    Vector2 mouse = GetMousePosition();
    CS230::Camera* camera = GetGSComponent<CS230::Camera>();

    if (camera != nullptr) {
        auto& input = Engine::GetInput();
        Math::vec2 camera_offset;
        if (input.KeyDown(CS230::Input::Keys::Up)) { camera_offset.y += camera_speed * dt; }
        if (input.KeyDown(CS230::Input::Keys::Down)) { camera_offset.y -= camera_speed * dt; }
        if (input.KeyDown(CS230::Input::Keys::Left)) { camera_offset.x -= camera_speed * dt; }
        if (input.KeyDown(CS230::Input::Keys::Right)) { camera_offset.x += camera_speed * dt; }
        Math::vec2 camera_pos = camera->GetPosition();
        camera->SetPosition(camera_pos + camera_offset);
    }

    if (!turnManager.IsTransitioning()) {
        Turn turn = turnManager.GetCurrentTurn();
        CS230::GameObject* currentGeneral = (turn == Turn::P1) ? player1 : player2;

        std::vector<Castle*>& currentCastles = (turn == Turn::P1) ? player1_castles : player2_castles;
        Stone* currentResources = (turn == Turn::P1) ? player1_resources : player2_resources;

        Vector2 mouse = GetMousePosition();
        CS230::Camera* camera = GetGSComponent<CS230::Camera>();
        const Math::TransformationMatrix& camMatrix = camera->GetMatrix();

        for (Castle* castle : currentCastles) {
            if (auto* col = castle->GetGOComponent<CS230::RectCollision>()) {
                if (CheckCollisionPointRec(mouse, col->ToRaylibScreenRect(camMatrix))) {
                    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::P)) {
                        if (currentResources->GetStoneCount() < 10) {
                            notification_message = "Not enough resources!";
                            notification_timer = 2.0;
                        }
                        else {
                            unit_production_ui.SetTarget(castle, &castle->GetSoldierRoster(), currentResources);
                            unit_production_ui.Init();
                            unit_production_ui.LoadRoster();
                            isProducingUnit = true;
                            unit_production_ui_initialized = true;
                            return;
                        }
                    }
                }
            }
        }

        if (auto* col = currentGeneral->GetGOComponent<CS230::RectCollision>()) {
            if (CheckCollisionPointRec(mouse, col->ToRaylibScreenRect(camMatrix))) {
                if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::P)) {
                    if (currentResources->GetStoneCount() < 0) {
                        notification_message = "Not enough resources!";
                        notification_timer = 2.0;
                    }
                    else {
                        if (turn == Turn::P1) {
                            unit_production_ui.SetTarget(player1, &dynamic_cast<SquirrelGen*>(player1)->GetSoldierRoster(), currentResources);
                        }
                        else {
                            unit_production_ui.SetTarget(player2, &dynamic_cast<SnakeGen*>(player2)->GetSoldierRoster(), currentResources);
                        }
                        unit_production_ui.Init();
                        unit_production_ui.LoadRoster();
                        isProducingUnit = true;
                        return;
                    }
                }
            }
        }

        if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::B)) {
            std::vector<Castle*>& friendly_castles = (turn == Turn::P1) ? player1_castles : player2_castles;
            Stone* current_player_resources = (turn == Turn::P1) ? player1_resources : player2_resources;

            if (friendly_castles.size() >= MAX_CASTLES) {
                notification_message = "Cannot build: Maximum castles reached!";
                notification_timer = 2.0;
            }
            else {
                HexTile* build_tile = gameMap.GetTileAtPosition(currentGeneral->GetGOComponent<LinearMovement>()->GetFootPosition());
                bool is_build_location_valid = true;
                if (build_tile) {
                    for (Castle* existing_castle : friendly_castles) {
                        HexTile* existing_castle_tile = gameMap.GetTileAtPosition(existing_castle->GetPosition());
                        if (existing_castle_tile && gameMap.IsNeighborTile(existing_castle_tile->x, existing_castle_tile->y, build_tile->x, build_tile->y)) {
                            is_build_location_valid = false;
                            notification_message = "Cannot build: Too close to an existing castle!";
                            notification_timer = 2.0;
                            break;
                        }
                    }
                }
                else { is_build_location_valid = false; }

                if (is_build_location_valid) {
                    int next_castle_cost = friendly_castles.size() * 10;
                    if (current_player_resources->SpendResources(next_castle_cost)) {
                        Engine::GetLogger().LogEvent("Player built a castle! Cost: " + std::to_string(next_castle_cost));
                        const char* spt_path = (turn == Turn::P1) ? "Assets/castle_me.spt" : "Assets/castle_enemy.spt";
                        Castle* new_castle = new Castle(build_tile->center, spt_path);
                        new_castle->SetScale({ 0.7, 0.7 });
                        friendly_castles.push_back(new_castle);
                        GOM->Add(new_castle);
                    }
                    else {
                        notification_message = "Not enough resources! (Needed: " + std::to_string(next_castle_cost) + ")";
                        notification_timer = 2.0;
                    }
                }
            }
        }

        if (turnManager.CanMove() && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !currentGeneral->GetGOComponent<LinearMovement>()->IsMoving()) {
            bool clickedOnGeneral = false;
            if (auto collisionComp = currentGeneral->GetGOComponent<CS230::RectCollision>(); collisionComp && camera) {
                if (CheckCollisionPointRec(mouse, collisionComp->ToRaylibScreenRect(camera->GetMatrix()))) {
                    generalSelected = !generalSelected;
                    clickedOnGeneral = true;
                    if (generalSelected) {
                        startingTile = gameMap.GetTileAtPosition(currentGeneral->GetGOComponent<LinearMovement>()->GetFootPosition());
                        movableTiles = gameMap.GetMovableTiles(startingTile);
                    }
                    else {
                        movableTiles.clear();
                        startingTile = nullptr;
                    }
                }
            }
            if (generalSelected && !clickedOnGeneral) {
                // ===== 수정된 부분 시작 =====
                const int screen_height = GetScreenHeight();
                const Math::TransformationMatrix& camera_matrix = camera->GetMatrix();

                for (auto& tile : movableTiles) {
                    // 1. 타일의 월드 좌표를 가져옵니다.
                    Math::vec2 world_pos = tile.center;

                    // 2. 카메라의 영향을 받은 화면 좌표로 변환합니다. (Draw 함수와 동일한 로직)
                    Math::vec2 transformed_pos = camera_matrix * world_pos;
                    Vector2 screen_pos = {
                        (float)transformed_pos.x,
                        screen_height - (float)transformed_pos.y
                    };

                    // 3. 변환된 '화면 좌표'를 기준으로 클릭을 확인합니다.
                    if (CheckCollisionPointCircle(mouse, screen_pos, 35.0f)) {

                        // 이동 명령은 '월드 좌표'로 해야 합니다.
                        currentGeneral->GetGOComponent<LinearMovement>()->MoveTo(tile.center);

                        generalSelected = false;
                        movableTiles.clear();
                        startingTile = nullptr;
                        turnManager.Move();
                        break;
                    }
                }
            }
        }

        Rectangle endTurnButton = { (float)screenWidth - 120, (float)screenHeight - 50, 110, 40 };
        if (CheckCollisionPointRec(mouse, endTurnButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            std::vector<Castle*>& castles_to_update = (turnManager.GetCurrentTurn() == Turn::P1) ? player1_castles : player2_castles;
            if (turnManager.GetCurrentTurn() == Turn::P1) {
                player1_resources->AddResources(castles_to_update.size() * 2);
            }
            else {
                player2_resources->AddResources(castles_to_update.size() * 2);
            }
            turnManager.EndTurn();
            generalSelected = false;
            movableTiles.clear();
            startingTile = nullptr;
        }

    }
    else {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Rectangle startButton = { screenWidth / 2.0f - 100, screenHeight / 2.0f + 50, 200, 60 };
            if (CheckCollisionPointRec(mouse, startButton)) {
                turnManager.StartTurn();
            }
        }
    }
    GOM->UpdateAll(dt);
}

void MainMapState::Draw() {
    if (isProducingUnit) {
        unit_production_ui.Draw();
        return;
    }

    ClearBackground(RAYWHITE);
    CS230::Camera* camera = GetGSComponent<CS230::Camera>();
    Math::TransformationMatrix camera_matrix;
    if (camera != nullptr) camera_matrix = camera->GetMatrix();

    if (!turnManager.IsTransitioning()) {
        Turn current_turn = turnManager.GetCurrentTurn();
        CS230::GameObject* currentGeneral = (current_turn == Turn::P1) ? player1 : player2;
        std::vector<Castle*>& friendlyCastles = (current_turn == Turn::P1) ? player1_castles : player2_castles;

        std::set<HexTile*> visibleTiles;
        auto addVisionFrom = [&](const Math::vec2& pos) {
            HexTile* source_tile = gameMap.GetTileAtPosition(pos);
            if (source_tile == nullptr) return;
            visibleTiles.insert(source_tile);
            auto neighbors = gameMap.GetMovableTiles(source_tile);
            for (const auto& neighbor : neighbors) {
                visibleTiles.insert(gameMap.GetTileAt(neighbor.x, neighbor.y));
            }
            };

        addVisionFrom(currentGeneral->GetGOComponent<LinearMovement>()->GetFootPosition());
        for (Castle* castle : friendlyCastles) {
            addVisionFrom(castle->GetPosition());
        }

        gameMap.Draw(visibleTiles, camera_matrix);

        if (generalSelected) {
            const int screen_height = GetScreenHeight(); // Y축 변환을 위해 화면 높이 가져오기

            // 1. '이동 전' 타일 (녹색 원) 그리기
            if (startingTile != nullptr) {
                // 월드 좌표를 카메라 행렬로 변환
                Math::vec2 transformed_pos = camera_matrix * Math::vec2(startingTile->center);
                // 최종 화면 좌표 계산 (Y축 뒤집기 포함)
                Vector2 screen_pos = {
                    (float)transformed_pos.x,
                    screen_height - (float)transformed_pos.y
                };
                DrawCircleV(screen_pos, 30, Fade(GREEN, 0.5f));
            }

            // 2. '이동 후' 타일 (파란 원) 그리기
            for (const auto& tile : movableTiles) {
                // 월드 좌표를 카메라 행렬로 변환
                Math::vec2 transformed_pos = camera_matrix * Math::vec2(tile.center);
                // 최종 화면 좌표 계산 (Y축 뒤집기 포함)
                Vector2 screen_pos = {
                    (float)transformed_pos.x,
                    screen_height - (float)transformed_pos.y
                };
                DrawCircleV(screen_pos, 30, Fade(BLUE, 0.4f));
            }
        }

        GetGSComponent<CS230::GameObjectManager>()->DrawAll(camera_matrix);

        std::string p1_text = "P1 Stone: " + std::to_string(player1_resources->GetStoneCount());
        DrawText(p1_text.c_str(), 10, 10, 20, BLACK);
        std::string p2_text = "P2 Stone: " + std::to_string(player2_resources->GetStoneCount());
        int p2_text_width = MeasureText(p2_text.c_str(), 20);
        DrawText(p2_text.c_str(), screenWidth - p2_text_width - 10, 10, 20, BLACK);

        std::string p1_castle_text = "Castles: " + std::to_string(player1_castles.size()) + " / " + std::to_string(MAX_CASTLES);
        DrawText(p1_castle_text.c_str(), 10, 35, 20, BLACK);
        std::string p2_castle_text = "Castles: " + std::to_string(player2_castles.size()) + " / " + std::to_string(MAX_CASTLES);
        int p2_castle_width = MeasureText(p2_castle_text.c_str(), 20);
        DrawText(p2_castle_text.c_str(), screenWidth - p2_castle_width - 10, 35, 20, BLACK);

        Rectangle endTurnButton = { (float)screenWidth - 120, (float)screenHeight - 50, 110, 40 };
        DrawRectangleRec(endTurnButton, LIGHTGRAY);
        DrawText("TurnEnd", endTurnButton.x + 10, endTurnButton.y + 10, 20, BLACK);

        if (notification_timer > 0) {
            int text_width = MeasureText(notification_message.c_str(), 30);
            DrawRectangle(screenWidth / 2 - text_width / 2 - 10, screenHeight / 2 - 25, text_width + 20, 50, Fade(BLACK, 0.7f));
            DrawText(notification_message.c_str(), screenWidth / 2 - text_width / 2, screenHeight / 2 - 15, 30, YELLOW);
        }

    }
    else {
        Vector2 mouse_pos = GetMousePosition();
        DrawRectangle(0, 0, screenWidth, screenHeight, Fade(GRAY, 0.8f));
        const char* turnText = (turnManager.GetCurrentTurn() == Turn::P1) ? "PLAYER 1 TURN" : "PLAYER 2 TURN";
        int textWidth = MeasureText(turnText, 60);
        DrawText(turnText, screenWidth / 2 - textWidth / 2, screenHeight / 2 - 100, 60, DARKBLUE);
        Rectangle startButton = { screenWidth / 2.0f - 100, screenHeight / 2.0f + 50, 200, 60 };
        bool hovering = CheckCollisionPointRec(mouse_pos, startButton);
        DrawRectangleRec(startButton, hovering ? DARKGRAY : LIGHTGRAY);
        DrawText("START", startButton.x + 60, startButton.y + 15, 30, BLACK);
    }
}

void MainMapState::Unload() {
    Engine::GetLogger().LogEvent(GetName() + " Unload");
    player1 = nullptr;
    player2 = nullptr;
    player1_castles.clear();
    player2_castles.clear();
}