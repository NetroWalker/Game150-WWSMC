// Game/MainMapState.cpp
#include "MainMapState.h"
#include "BattleMap.h"
#include "States.h"
#include "SquirrelGen.h"
#include "SnakeGen.h"
#include "LinearMovement.h"
#include "../Engine/Camera.h"
#include <cmath>

MainMapState::MainMapState(int sw, int sh) :
    screenWidth(sw), screenHeight(sh),
    radiusX(200.0f), radiusY(200.0f),
    gameMap(Vector2{ screenWidth / 2.0f - ((5 - 1) * 200.0f * 1.5f) / 2.0f,
                     screenHeight / 2.0f - ((5 - 1) * 200.0f * sqrtf(3.0f) * 0.5f) / 2.0f },
        200.0f, 200.0f, 5, 5, true),
    generalSelected(false)
{
}

void MainMapState::Load() {
    AddGSComponent(new CS230::Camera({ {0,0}, {0,0} }));
    Engine::GetLogger().LogEvent(GetName() + " Load");
    AddGSComponent(new CS230::GameObjectManager());

    gameMap.SetPoint();
    HexTile* tile33 = gameMap.GetTileAt(3, 3);
    if (!tile33) {
        Engine::GetLogger().LogError("MainMapState: tile33 not found during Load.");
        return;
    }

    // Vec2.h 수정 덕분에 타입 변환 코드가 필요 없습니다.
    player1 = new SquirrelGen(gameMap.GetTiles()[0].center);
    player2 = new SnakeGen(tile33->center);
    GetGSComponent<CS230::GameObjectManager>()->Add(player1);
    GetGSComponent<CS230::GameObjectManager>()->Add(player2);
    player1->SetScale({ 0.5, 0.5 });
    player2->SetScale({ 0.5, 0.5 });
    generalSelected = false;
    movableTiles.clear();
}

void MainMapState::Update(double dt) {
    CS230::GameObjectManager* GOM = GetGSComponent<CS230::GameObjectManager>();
    Vector2 mouse = GetMousePosition();
    Turn turn = turnManager.GetCurrentTurn();
    CS230::Camera* camera = GetGSComponent<CS230::Camera>();
    if (camera != nullptr) {
        auto& input = Engine::GetInput(); // 입력 시스템에 대한 참조
        Math::vec2 camera_offset;         // 이번 프레임에 이동할 거리

        if (input.KeyDown(CS230::Input::Keys::Up)) {
            camera_offset.y += camera_speed * dt;
        }
        if (input.KeyDown(CS230::Input::Keys::Down)) {
            camera_offset.y -= camera_speed * dt;
        }
        if (input.KeyDown(CS230::Input::Keys::Left)) {
            camera_offset.x -= camera_speed * dt;
        }
        if (input.KeyDown(CS230::Input::Keys::Right)) {
            camera_offset.x += camera_speed * dt;
        }

        // 현재 카메라 위치에 이동할 거리를 더해 새로운 위치로 설정
        Math::vec2 camera_pos = camera->GetPosition();
        camera->SetPosition(camera_pos + camera_offset);
    }
    if (!turnManager.IsTransitioning()) {
        CS230::GameObject* currentGeneral = (turn == Turn::P1) ? player1 : player2;
        LinearMovement* currentMovement = currentGeneral->GetGOComponent<LinearMovement>();

        if (turnManager.CanMove()) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !currentMovement->IsMoving()) {
                // Vec2.h 수정 덕분에 GetPosition()의 결과를 바로 사용할 수 있습니다.
                if (CheckCollisionPointCircle(mouse, currentGeneral->GetPosition(), 50)) {
                    generalSelected = !generalSelected;
                    if (generalSelected) {
                        // GetFootPosition()의 결과도 바로 사용 가능합니다.
                        HexTile* from = gameMap.GetTileAtPosition(currentMovement->GetFootPosition());
                        movableTiles = gameMap.GetMovableTiles(from);
                    }
                    else {
                        movableTiles.clear();
                    }
                }
                else if (generalSelected) {
                    for (auto& tile : movableTiles) {
                        if (CheckCollisionPointCircle(mouse, tile.center, radiusX * 0.8f)) {
                            // tile.center (Vector2)도 MoveTo(Math::vec2)에 바로 전달 가능합니다.
                            currentMovement->MoveTo(tile.center);
                            generalSelected = false;
                            movableTiles.clear();
                            turnManager.Move();
                            break;
                        }
                    }
                }
            }
        }

        // 턴 종료 버튼 로직
        Rectangle endTurnButton = { 1260, 750, 110, 40 };
        if (CheckCollisionPointRec(mouse, endTurnButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            turnManager.EndTurn();
            generalSelected = false;
            movableTiles.clear();
        }

        GOM->CollisionTest();
    }
    else {
        // 턴 전환 화면에서 START 버튼 클릭 로직
        Rectangle startButton = { screenWidth / 2.0f - 100, screenHeight / 2.0f + 50, 200, 60 };
        if (CheckCollisionPointRec(mouse, startButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            turnManager.StartTurn();
        }
    }

    GOM->UpdateAll(dt);
}

void MainMapState::Draw() {
    ClearBackground(RAYWHITE);
    CS230::Camera* camera = GetGSComponent<CS230::Camera>();
    Math::TransformationMatrix camera_matrix; // 기본 행렬 (카메라가 없을 경우 대비)
    if (camera != nullptr) {
        camera_matrix = camera->GetMatrix();
    }
    if (!turnManager.IsTransitioning()) {
        // ========== 게임 플레이 화면 그리기 ==========

        // 1. 현재 턴인 장군의 위치 타일 찾기
        CS230::GameObject* currentGeneral = (turnManager.GetCurrentTurn() == Turn::P1) ? player1 : player2;
        HexTile* currentTile = nullptr;
        if (currentGeneral != nullptr) {
            LinearMovement* currentMovement = currentGeneral->GetGOComponent<LinearMovement>();
            currentTile = gameMap.GetTileAtPosition(currentMovement->GetFootPosition());
        }

        // 2. 맵 그리기 (찾아낸 타일을 시야의 중심으로 전달)
        gameMap.Draw(currentTile, camera_matrix);

        // 3. 이동 가능 범위 그리기 (선택 사항)
        if (generalSelected) {
            for (const auto& tile : movableTiles) {
                DrawCircleV(tile.center, 30, Fade(BLUE, 0.4f));
            }
        }

        // 4. 모든 유닛(장군) 그리기
        GetGSComponent<CS230::GameObjectManager>()->DrawAll(camera_matrix);

        // 5. UI 그리기
        Rectangle endTurnButton = { 1260, 750, 110, 40 };
        DrawRectangleRec(endTurnButton, LIGHTGRAY);
        DrawText("TurnEnd", endTurnButton.x + 10, endTurnButton.y + 10, 20, BLACK);

    }
    else {
        // 턴 전환 화면 그리기
        DrawRectangle(0, 0, screenWidth, screenHeight, Fade(GRAY, 0.8f));

        Turn turn = turnManager.GetCurrentTurn();
        const char* turnText = (turn == Turn::P1) ? "PLAYER 1 TURN" : "PLAYER 2 TURN";
        int textWidth = MeasureText(turnText, 60);
        DrawText(turnText, screenWidth / 2 - textWidth / 2, screenHeight / 2 - 100, 60, DARKBLUE);

        Rectangle startButton = { screenWidth / 2.0f - 100, screenHeight / 2.0f + 50, 200, 60 };
        Vector2 mouse = GetMousePosition();
        bool hovering = CheckCollisionPointRec(mouse, startButton);
        DrawRectangleRec(startButton, hovering ? DARKGRAY : LIGHTGRAY);
        DrawText("START", startButton.x + 60, startButton.y + 15, 30, BLACK);
    }
}

void MainMapState::Unload() {
    Engine::GetLogger().LogEvent(GetName() + " Unload");
}