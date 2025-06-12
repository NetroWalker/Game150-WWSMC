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


    //victory = false;
    //generalSelected = false;
    //movableTiles.clear();
    //startingTile = nullptr;
    //// 턴 매니저 리셋: P1 START 모드
    //turnManager = TurnManager();
    //turnManager.SetTurn(Turn::P1);
    //turnManager.SetTransition(true);

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
    // 플레이어1 성
    Castle* initial_castle1 = new Castle(
        p1_start_pos,
        /*isSnake=*/false,
        "Assets/castle_me.spt"
    );
    // 플레이어2 성
    Castle* initial_castle2 = new Castle(
        p2_start_pos,
        /*isSnake=*/true,
        "Assets/castle_enemy.spt"
    );

    player1_castles.push_back(initial_castle1);
    player2_castles.push_back(initial_castle2);

    GOM->Add(initial_castle1);
    GOM->Add(initial_castle2);
    GOM->Add(player1);
    GOM->Add(player2);
    player1_resources->AddResources(2);
    player1->SetScale({ 0.3, 0.3 });
    player2->SetScale({ 0.3, 0.3 });
    initial_castle1->SetScale({ 0.7, 0.7 });
    initial_castle2->SetScale({ 0.7, 0.7 });

    generalSelected = false;
    movableTiles.clear();
    startingTile = nullptr;
}

void MainMapState::Update(double dt) {
//    if (victory) {
//    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::Escape)) {
//        victory = false;  // 놔두면 다시 돌아왔을 때 전투 플래그가 살아있음
//        // 즉시 메인 메뉴로 전환
//        Engine::GetGameStateManager().SetNextGameState(STATE_MENU);
//    }
//    return;  // victory 모드에선 그 외 로직 건너뛰기
//}


    if (notification_timer > 0) {
        notification_timer -= dt;
        if (notification_timer <= 0) notification_message.clear();
    }
    if (Engine::GetInput().KeyJustPressed(CS230::Input::Keys::O)) {
        godMode = !godMode;
        notification_message = "God Mode " + std::string(godMode ? "ON" : "OFF");
        notification_timer = 2.0;
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
        if (godMode && Engine::GetInput().KeyJustPressed(CS230::Input::Keys::G)) {
            Stone* current_player_resources = (turn == Turn::P1) ? player1_resources : player2_resources;
            int currentAmount = current_player_resources->GetStoneCount();
            if (currentAmount > 0) {
                current_player_resources->AddResources(currentAmount);
            }
            else {
                current_player_resources->AddResources(1); // 자원이 0일 경우 1을 추가
            }
            notification_message = "Resources Doubled!";
            notification_timer = 1.5;
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

                        bool isSnakeCastle = (turn != Turn::P1);
                        std::string spt_path = isSnakeCastle
                            ? "Assets/castle_enemy.spt"
                            : "Assets/castle_me.spt";
                        Castle* new_castle = new Castle(
                            build_tile->center,
                            isSnakeCastle,
                            spt_path
                        );

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
                        movableTiles.clear(); // 일단 비움

                        // ===== 수정: 갓 모드 이동 범위 계산 =====
                        if (godMode) {
                            // 갓 모드: 모든 타일로 이동 가능
                            const auto& allTiles = gameMap.GetTiles();
                            for (const auto& tile : allTiles) {
                                if (&tile != startingTile) {
                                    movableTiles.push_back(tile);
                                }
                            }
                        }
                        else {
                            // 일반 모드: 인접 타일만 이동 가능
                            movableTiles = gameMap.GetMovableTiles(startingTile);
                        }
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
                std::vector<Castle*>& castles_to_update = (turnManager.GetCurrentTurn() == Turn::P1) ? player1_castles : player2_castles;
                if (turnManager.GetCurrentTurn() == Turn::P1) {
                    player1_resources->AddResources(castles_to_update.size() * 2);
                }
                else {
                    player2_resources->AddResources(castles_to_update.size() * 2);
                }
                turnManager.StartTurn();
            }
        }
    }
    GOM->UpdateAll(dt);
    // --- 1) 이동 컴포넌트 가져오기 ---
    // ─── 여기에 넣기 (turn/currentGeneral 이 살아 있는 범위)
    if (!turnManager.IsTransitioning()) {
        // ① 턴과 장군 포인터 다시 가져오기
        Turn turn = turnManager.GetCurrentTurn();
        CS230::GameObject* currentGeneral = (turn == Turn::P1) ? player1 : player2;

        // ② 적 성 리스트
        auto& enemyCastles = (turn == Turn::P1) ? player2_castles : player1_castles;

        // ③ 이동이 끝난 시점에만 체크
        auto movement = currentGeneral->GetGOComponent<LinearMovement>();
        if (movement && !movement->IsMoving()) {
            HexTile* generalTile = gameMap.GetTileAtPosition(movement->GetFootPosition());
            if (generalTile) {
                for (Castle* castle : enemyCastles) {
                    HexTile* castleTile = gameMap.GetTileAtPosition(castle->GetPosition());
                    if (castleTile
                        && generalTile->x == castleTile->x
                        && generalTile->y == castleTile->y)
                    {
                        Engine::GetLogger().LogEvent(
                            "Tile-Collision: " +
                            currentGeneral->TypeName() +
                            " ↔ " +
                            castle->TypeName()
                        );
                        castle->ResolveCollision(currentGeneral);
                        currentGeneral->ResolveCollision(castle);

                        victory = true;                           // ← 여기를 추가
                        notification_message = "You Win!";        // 옵션: 간단한 메시지
                        notification_timer = 3.0;
                        break;
                    }
                }
            }
        }
    }
}

void MainMapState::Draw() {
    ClearBackground(RAYWHITE);
    CS230::Camera* camera = GetGSComponent<CS230::Camera>();
    Math::TransformationMatrix camera_matrix;
    if (camera != nullptr) camera_matrix = camera->GetMatrix();

    if (!turnManager.IsTransitioning()) {
        Turn current_turn = turnManager.GetCurrentTurn();
        CS230::GameObject* currentGeneral = (current_turn == Turn::P1) ? player1 : player2;
        CS230::GameObject* enemyGeneral = (current_turn == Turn::P1) ? player2 : player1;
        std::vector<Castle*>& friendlyCastles = (current_turn == Turn::P1) ? player1_castles : player2_castles;
        std::vector<Castle*>& enemyCastles = (current_turn == Turn::P1) ? player2_castles : player1_castles;

        // ===== 1. '시야 지도' 생성 =====
        std::map<HexTile*, TileType> visionMap;

        // 헬퍼 람다 함수: 특정 위치 주변의 시야를 지도에 추가
        auto addVisionToMap = [&](HexTile* source_tile, bool is_castle_vision) {
            if (source_tile == nullptr) return;

            // 시야 중심 타일 처리
            if (is_castle_vision) visionMap[source_tile] = TileType::Water;
            else if (visionMap.find(source_tile) == visionMap.end()) visionMap[source_tile] = source_tile->type;

            // 주변 타일 처리
            auto neighbors = gameMap.GetMovableTiles(source_tile);
            for (const auto& neighbor : neighbors) {
                HexTile* neighbor_ptr = gameMap.GetTileAt(neighbor.x, neighbor.y);
                if (is_castle_vision) {
                    visionMap[neighbor_ptr] = TileType::Water;
                }
                else {
                    if (visionMap.find(neighbor_ptr) == visionMap.end()) {
                        visionMap[neighbor_ptr] = neighbor_ptr->type;
                    }
                }
            }
            };

        // ===== 2. 규칙에 따라 시야 지도 채우기 =====
        // 우선순위가 높은 성의 시야부터 추가
        for (Castle* castle : friendlyCastles) {
            addVisionToMap(gameMap.GetTileAtPosition(castle->GetPosition()), true);
        }
        // 그 다음 장군의 시야 추가 (겹치는 부분은 덮어쓰지 않음)
        addVisionToMap(gameMap.GetTileAtPosition(currentGeneral->GetGOComponent<LinearMovement>()->GetFootPosition()), false);

        // ===== 3. 완성된 시야 지도로 맵 그리기 =====
        gameMap.Draw(visionMap, camera_matrix);

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

            for (const auto& tile : movableTiles) {
                Math::vec2 transformed_pos = camera_matrix * Math::vec2(tile.center);

                Vector2 screen_pos = {
                    (float)transformed_pos.x,
                    screen_height - (float)transformed_pos.y
                };
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