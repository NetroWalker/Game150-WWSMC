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
#include "GameSession.h" 
#include <cmath>
#include "UnitProduction.h"

MainMapState::MainMapState(int sw, int sh) :
    screenWidth(sw), screenHeight(sh),
    radiusX(200.0f), radiusY(200.0f),
    gameMap(Vector2{ screenWidth / 2.0f - ((8 - 1) * 200.0f * 1.5f) / 2.0f,
                      screenHeight / 2.0f - ((10 - 1) * 200.0f * sqrtf(3.0f) * 0.5f) / 2.0f },
        200.0f, 200.0f, 8, 10, false), 
    generalSelected(false), godMode(false), battle_ended(false), victory(false), notification_timer(0.0)
{
}

MainMapState::~MainMapState() {
}

void MainMapState::Load() {
    Engine::GetLogger().LogEvent(GetName() + " Load");
    AddGSComponent(new CS230::Camera({ {0,0}, {0,0} }));

    auto& session = GameSession::GetInstance();

    if (session.player1 == nullptr) {
        gameMap.SetPoint();
        HexTile* tile_end = gameMap.GetTileAt(7, 9);
        if (!tile_end) { return; }

        Math::vec2 p1_start_pos = gameMap.GetTiles()[0].center;
        Math::vec2 p2_start_pos = tile_end->center;

        session.player1 = new SquirrelGen(p1_start_pos);
        session.player2 = new SnakeGen(p2_start_pos);

        float y_offset = gameMap.radiusY * sqrtf(3.0f);
        Math::vec2 p1_castle_pos = { p1_start_pos.x, p1_start_pos.y + y_offset/2 };
        Math::vec2 p2_castle_pos = { p2_start_pos.x, p2_start_pos.y + y_offset/2 };

        Castle* initial_castle1 = new Castle(p1_castle_pos, false, "Assets/castle_me.spt");
        Castle* initial_castle2 = new Castle(p2_castle_pos, true, "Assets/castle_enemy.spt");
        initial_castle1->SetScale({ 0.5, 0.5 });
        initial_castle2->SetScale({ 0.5, 0.5 });

        session.player1_castles.push_back(initial_castle1);
        session.player2_castles.push_back(initial_castle2);

        session.gom.Add(initial_castle1);
        session.gom.Add(initial_castle2);
        session.gom.Add(session.player1);
        session.gom.Add(session.player2);

        session.player1_resources->AddResources(2);
        session.player1->SetScale({ 0.3, 0.3 });
        session.player2->SetScale({ 0.3, 0.3 });
        
    }

    this->victory = false;
    generalSelected = false;
    movableTiles.clear();
    startingTile = nullptr;

    unit_production_ui.SetMainMap(this);
}

void MainMapState::SetBattleOutcome(BattleOutcome outcome) {
    this->battle_outcome = outcome;
    this->battle_ended = true;
}

void MainMapState::HandleBattleAftermath() {
    Engine::GetLogger().LogEvent("Handling battle aftermath...");
    auto& session = GameSession::GetInstance();
    if (session.current_battle_type == GameSession::BattleType::Siege && session.castle_under_siege != nullptr) {
        Castle* sieged_castle = session.castle_under_siege;
        bool is_p1_castle_owner = (std::find(session.player1_castles.begin(), session.player1_castles.end(), sieged_castle) != session.player1_castles.end());

        bool defender_lost = (is_p1_castle_owner && battle_outcome == BattleOutcome::P2_WINS) ||
            (!is_p1_castle_owner && battle_outcome == BattleOutcome::P1_WINS);
            
        if (defender_lost) {
            Engine::GetLogger().LogEvent("Castle has fallen and is destroyed!");
            notification_message = "The castle has fallen!";
            notification_timer = 3.0;
            session.gom.Remove(sieged_castle);
            if (is_p1_castle_owner) {
                session.player1_castles.erase(std::remove(session.player1_castles.begin(), session.player1_castles.end(), sieged_castle), session.player1_castles.end());
            }
            else {
                session.player2_castles.erase(std::remove(session.player2_castles.begin(), session.player2_castles.end(), sieged_castle), session.player2_castles.end());
            }
        }
        session.current_battle_type = GameSession::BattleType::Field;
        session.castle_under_siege = nullptr;
    }
    auto respawn_player = [&](CS230::GameObject* player, std::vector<Castle*>& castles) {
        if (castles.empty()) {
            
        }
        else {
            player->SetPosition(castles[0]->GetPosition());
        }
        };

    switch (battle_outcome) {
    case BattleOutcome::P1_WINS:
        notification_message = "Player 1 Won! Player 2's castle destroyed.";
        notification_timer = 3.0;
        session.player1_resources->AddResources(session.player2_resources->GetStoneCount());
        session.player2_resources->SpendResources(session.player2_resources->GetStoneCount());

        if (!session.player2_castles.empty()) {
            Castle* destroyed_castle = session.player2_castles.back();
            session.gom.Remove(destroyed_castle);
            session.player2_castles.pop_back();
        }
        respawn_player(session.player2, session.player2_castles);
        break;

    case BattleOutcome::P2_WINS:
        notification_message = "Player 2 Won! Player 1's castle destroyed.";
        notification_timer = 3.0;
        session.player2_resources->AddResources(session.player1_resources->GetStoneCount());
        session.player1_resources->SpendResources(session.player1_resources->GetStoneCount());

        if (!session.player1_castles.empty()) {
            Castle* destroyed_castle = session.player1_castles.back();
            session.gom.Remove(destroyed_castle);
            session.player1_castles.pop_back();
        }
        respawn_player(session.player1, session.player1_castles);
        break;

    case BattleOutcome::DRAW:
        notification_message = "Draw! Both generals retreat.";
        notification_timer = 3.0;
        respawn_player(session.player1, session.player1_castles);
        respawn_player(session.player2, session.player2_castles);
        break;
    }
    battle_ended = false;
}

void MainMapState::Update(double dt) {
    auto& session = GameSession::GetInstance();

    if (session.player1_castles.empty()) {
        session.last_game_result = GameSession::GameResult::P2_Victory;
        Engine::GetGameStateManager().SetNextGameState(STATE_ENDING);
        return;
    }
    if (session.player2_castles.empty()) {
        session.last_game_result = GameSession::GameResult::P1_Victory;
        Engine::GetGameStateManager().SetNextGameState(STATE_ENDING);
        return;
    }

    if (battle_ended) {
        HandleBattleAftermath();
    }


    if (!isProducingUnit && unit_production_ui_initialized) {
        unit_production_ui.Unload();
        unit_production_ui_initialized = false;

    }
    if (isProducingUnit) {
        if (!unit_production_ui_initialized) {
            unit_production_ui.Init();
            unit_production_ui_initialized = true;
        }
        unit_production_ui.Update(dt);
        return;
    }


    if (notification_timer > 0) {
        notification_timer -= dt;
        if (notification_timer <= 0) notification_message.clear();
    }
    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::O)) {
        godMode = !godMode;
        notification_message = "God Mode " + std::string(godMode ? "ON" : "OFF");
        notification_timer = 2.0;
    }

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
    Vector2 mouse = GetMousePosition();
    if (!turnManager.IsTransitioning()) {
        Turn turn = turnManager.GetCurrentTurn();
        CS230::GameObject* currentGeneral = (turn == Turn::P1) ? session.player1 : session.player2;
        CS230::GameObject* enemyGeneral = (turn == Turn::P1) ? session.player2 : session.player1;

        if (godMode && Engine::GetInput().KeyJustPressed(CS230::Input::Keys::G)) {
            Stone* current_player_resources = (turn == Turn::P1) ? session.player1_resources : session.player2_resources;
            int currentAmount = current_player_resources->GetStoneCount();
            current_player_resources->AddResources(currentAmount > 0 ? currentAmount : 1);
            notification_message = "Resources Doubled!";
            notification_timer = 1.5;
        }
        std::vector<Castle*>& currentCastles = (turn == Turn::P1) ? session.player1_castles : session.player2_castles;
        Stone* currentResources = (turn == Turn::P1) ? session.player1_resources : session.player2_resources;

        
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
                            unit_production_ui.SetTarget(session.player1, &dynamic_cast<SquirrelGen*>(session.player1)->GetSoldierRoster(), currentResources);
                        }
                        else {
                            unit_production_ui.SetTarget(session.player2, &dynamic_cast<SnakeGen*>(session.player2)->GetSoldierRoster(), currentResources);
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
            std::vector<Castle*>& friendly_castles = (turn == Turn::P1) ? session.player1_castles : session.player2_castles;
            Stone* current_player_resources = (turn == Turn::P1) ? session.player1_resources : session.player2_resources;

            if (friendly_castles.size() >= MAX_CASTLES) {
                notification_message = "Cannot build: Maximum castles reached!";
                notification_timer = 2.0;
            }
            else {
                HexTile* build_tile = gameMap.GetTileAtPosition(currentGeneral->GetGOComponent<LinearMovement>()->GetFootPosition());
                bool is_build_location_valid = (build_tile != nullptr);

                if (is_build_location_valid) {
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

                if (is_build_location_valid) {
                    int next_castle_cost = friendly_castles.size() * 10;
                    if (current_player_resources->SpendResources(next_castle_cost)) {
                        Engine::GetLogger().LogEvent("Player built a castle! Cost: " + std::to_string(next_castle_cost));
                        bool isSnakeCastle = (turn != Turn::P1);
                        const char* spt_path = isSnakeCastle ? "Assets/castle_enemy.spt" : "Assets/castle_me.spt";
                        float y_offset = gameMap.radiusY * sqrtf(3.0f);
                        Math::vec2 castle_pos = { build_tile->center.x, build_tile->center.y + y_offset };

                        Castle* new_castle = new Castle(castle_pos, isSnakeCastle, spt_path);
                        new_castle->SetScale({ 0.5, 0.5 });
                        friendly_castles.push_back(new_castle);
                        session.gom.Add(new_castle);
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
            if (auto* collisionComp = currentGeneral->GetGOComponent<CS230::RectCollision>(); collisionComp && camera) {
                if (CheckCollisionPointRec(mouse, collisionComp->ToRaylibScreenRect(camera->GetMatrix()))) {
                    generalSelected = !generalSelected;
                    clickedOnGeneral = true;
                    if (generalSelected) {
                        startingTile = gameMap.GetTileAtPosition(currentGeneral->GetGOComponent<LinearMovement>()->GetFootPosition());
                        movableTiles.clear();
                        movableTiles = godMode ? gameMap.GetTiles() : gameMap.GetMovableTiles(startingTile);
                    }
                    else {
                        movableTiles.clear();
                        startingTile = nullptr;
                    }
                }
            }
            if (generalSelected && !clickedOnGeneral) {
                const int screen_height = GetScreenHeight();
                for (auto& tile : movableTiles) {
                    Math::vec2 transformed_pos = camera->GetMatrix() * tile.center;
                    Vector2 screen_pos = { (float)transformed_pos.x, screen_height - (float)transformed_pos.y };
                    if (CheckCollisionPointCircle(mouse, screen_pos, 35.0f)) {
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
            std::vector<Castle*>& castles_to_update = (turnManager.GetCurrentTurn() == Turn::P1) ? session.player1_castles : session.player2_castles;
            if (turnManager.GetCurrentTurn() == Turn::P1) {
                session.player1_resources->AddResources(castles_to_update.size() * 2);
            }
            else {
                session.player2_resources->AddResources(castles_to_update.size() * 2);
            }
            turnManager.EndTurn();
            generalSelected = false;
            movableTiles.clear();
            startingTile = nullptr;
        }

        auto* movement = currentGeneral->GetGOComponent<LinearMovement>();
        if (movement && !movement->IsMoving()) {
            HexTile* generalTile = gameMap.GetTileAtPosition(movement->GetFootPosition());
            if (generalTile) {
                std::vector<Castle*>& enemyCastles = (turn == Turn::P1) ? session.player2_castles : session.player1_castles;
                for (Castle* castle : enemyCastles) {
                    HexTile* castleTile = gameMap.GetTileAtPosition(castle->GetPosition());
                    if (castleTile && generalTile->x == castleTile->x && generalTile->y == castleTile->y) {

                        Engine::GetLogger().LogEvent("Siege battle initiated against castle!");
                        session.current_battle_type = GameSession::BattleType::Siege;
                        session.castle_under_siege = castle;
                        Engine::GetGameStateManager().SetNextGameState(STATE_BATTLE_MAP);
                        return;
                    }
                }
                HexTile* enemyGeneralTile = gameMap.GetTileAtPosition(enemyGeneral->GetGOComponent<LinearMovement>()->GetFootPosition());
                if (enemyGeneralTile && generalTile->x == enemyGeneralTile->x && generalTile->y == enemyGeneralTile->y) {
                    Engine::GetLogger().LogEvent("Generals have met! Entering battle...");
                    Engine::GetGameStateManager().SetNextGameState(STATE_BATTLE_MAP);
                    return;
                }
            }
        }
    }
    else {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            Rectangle startButton = { screenWidth / 2.0f - 100, screenHeight / 2.0f + 50, 200, 60 };
            if (CheckCollisionPointRec(mouse, startButton)) {
                std::vector<Castle*>& castles_to_update = (turnManager.GetCurrentTurn() == Turn::P1) ? session.player1_castles : session.player2_castles;
                Stone* resources_to_update = (turnManager.GetCurrentTurn() == Turn::P1) ? session.player1_resources : session.player2_resources;
                resources_to_update->AddResources(castles_to_update.size() * 2);
                turnManager.StartTurn();
            }
        }
    }

    session.gom.Update(dt);
}

void MainMapState::Draw() {
    auto& session = GameSession::GetInstance();
    ClearBackground(Color{ 158, 178, 81, 255 });
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
        CS230::GameObject* currentGeneral = (current_turn == Turn::P1) ? session.player1 : session.player2;
        CS230::GameObject* enemyGeneral = (current_turn == Turn::P1) ? session.player2 : session.player1;
        std::vector<Castle*>& friendlyCastles = (current_turn == Turn::P1) ? session.player1_castles : session.player2_castles;
        std::vector<Castle*>& enemyCastles = (current_turn == Turn::P1) ? session.player2_castles : session.player1_castles;
        
        std::map<HexTile*, TileType> visionMap;
        auto addVisionToMap = [&](HexTile* source_tile) {
            if (source_tile == nullptr) return;
            if (visionMap.find(source_tile) == visionMap.end()) {
                visionMap[source_tile] = source_tile->type;
            }
            auto neighbors = gameMap.GetAllNeighbors(source_tile);
            for (const auto& neighbor : neighbors) {
                HexTile* neighbor_ptr = gameMap.GetTileAt(neighbor.x, neighbor.y);
                if (neighbor_ptr != nullptr && visionMap.find(neighbor_ptr) == visionMap.end()) {
                    visionMap[neighbor_ptr] = neighbor_ptr->type;
                }
            }
            };

        for (Castle* castle : friendlyCastles) {
            addVisionToMap(gameMap.GetTileAtPosition(castle->GetPosition()));
        }
        addVisionToMap(gameMap.GetTileAtPosition(currentGeneral->GetGOComponent<LinearMovement>()->GetFootPosition()));

        gameMap.Draw(visionMap, camera_matrix);

        if (generalSelected) {
            const int screen_height = GetScreenHeight();
            if (startingTile != nullptr) {
                Math::vec2 transformed_pos = camera_matrix * Math::vec2(startingTile->center);
                Vector2 screen_pos = { (float)transformed_pos.x, screen_height - (float)transformed_pos.y };
                DrawCircleV(screen_pos, 30, Fade(GREEN, 0.5f));
            }
            for (const auto& tile : movableTiles) {
                Math::vec2 transformed_pos = camera_matrix * Math::vec2(tile.center);
                Vector2 screen_pos = { (float)transformed_pos.x, screen_height - (float)transformed_pos.y };
                DrawCircleV(screen_pos, 30, Fade(BLUE, 0.4f));
            }
        }

        currentGeneral->Draw(camera_matrix);
        for (Castle* castle : friendlyCastles) castle->Draw(camera_matrix);

        HexTile* enemyGeneralTile = gameMap.GetTileAtPosition(enemyGeneral->GetGOComponent<LinearMovement>()->GetFootPosition());
        if (enemyGeneralTile != nullptr && visionMap.count(enemyGeneralTile) > 0) {
            enemyGeneral->Draw(camera_matrix);
        }
        for (Castle* castle : enemyCastles) {
            HexTile* enemyCastleTile = gameMap.GetTileAtPosition(castle->GetPosition());
            if (enemyCastleTile != nullptr && visionMap.count(enemyCastleTile) > 0) {
                castle->Draw(camera_matrix);
            }
        }

        std::string p1_text = "P1 Stone: " + std::to_string(session.player1_resources->GetStoneCount());
        DrawText(p1_text.c_str(), 10, 10, 20, BLACK);
        std::string p2_text = "P2 Stone: " + std::to_string(session.player2_resources->GetStoneCount());
        int p2_text_width = MeasureText(p2_text.c_str(), 20);
        DrawText(p2_text.c_str(), screenWidth - p2_text_width - 10, 10, 20, BLACK);

        std::string p1_castle_text = "Castles: " + std::to_string(session.player1_castles.size()) + " / " + std::to_string(MAX_CASTLES);
        DrawText(p1_castle_text.c_str(), 10, 35, 20, BLACK);
        std::string p2_castle_text = "Castles: " + std::to_string(session.player2_castles.size()) + " / " + std::to_string(MAX_CASTLES);
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
}