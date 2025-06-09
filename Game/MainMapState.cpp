// Game/MainMapState.cpp
#include "MainMapState.h"
#include "BattleMap.h"
#include "States.h"
#include "SquirrelGen.h"
#include "SnakeGen.h"
#include "LinearMovement.h"
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
    Engine::GetLogger().LogEvent(GetName() + " Load");
    AddGSComponent(new CS230::GameObjectManager());

    gameMap.SetPoint();
    HexTile* tile33 = gameMap.GetTileAt(3, 3);
    if (!tile33) {
        Engine::GetLogger().LogError("MainMapState: tile33 not found during Load.");
        return;
    }

    // 플레이어 생성 및 GameObjectManager에 추가
    // Raylib의 Vector2를 엔진의 Math::vec2로 변환하여 전달합니다.
    player1 = new SquirrelGen({ (double)gameMap.GetTiles()[0].center.x, (double)gameMap.GetTiles()[0].center.y });
    player2 = new SnakeGen({ (double)tile33->center.x, (double)tile33->center.y });
    GetGSComponent<CS230::GameObjectManager>()->Add(player1);
    GetGSComponent<CS230::GameObjectManager>()->Add(player2);

    generalSelected = false;
    movableTiles.clear();
    // turnManager는 자동으로 초기화됩니다.
}

void MainMapState::Update(double dt) {
    CS230::GameObjectManager* GOM = GetGSComponent<CS230::GameObjectManager>();
    Vector2 mouse = GetMousePosition(); // Raylib의 Vector2
    Turn turn = turnManager.GetCurrentTurn();

    if (!turnManager.IsTransitioning()) {
        CS230::GameObject* currentGeneral = (turn == Turn::P1) ? player1 : player2;
        LinearMovement* currentMovement = currentGeneral->GetGOComponent<LinearMovement>();

        if (turnManager.CanMove()) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !currentMovement->IsMoving()) {

                // ===== 수정된 부분 1 =====
                // GetPosition()이 반환하는 Math::vec2를 Raylib의 Vector2로 수동 변환합니다.
                Math::vec2 currentPos_math = currentGeneral->GetPosition();
                Vector2 currentPos_raylib = { (float)currentPos_math.x, (float)currentPos_math.y };

                if (CheckCollisionPointCircle(mouse, currentPos_raylib, 50)) {
                    generalSelected = !generalSelected;
                    if (generalSelected) {

                        // ===== 수정된 부분 2 =====
                        // GetFootPosition()이 반환하는 Math::vec2도 Raylib의 Vector2로 수동 변환합니다.
                        Math::vec2 footPos_math = currentMovement->GetFootPosition();
                        Vector2 footPos_raylib = { (float)footPos_math.x, (float)footPos_math.y };

                        HexTile* from = gameMap.GetTileAtPosition(footPos_raylib);
                        movableTiles = gameMap.GetMovableTiles(from);
                    }
                    else {
                        movableTiles.clear();
                    }
                }
                else if (generalSelected) {
                    for (auto& tile : movableTiles) {
                        if (CheckCollisionPointCircle(mouse, tile.center, radiusX * 0.8f)) {
                            // MoveTo에는 Raylib의 tile.center를 다시 Math::vec2로 변환하여 전달합니다.
                            currentMovement->MoveTo({ (double)tile.center.x, (double)tile.center.y });
                            generalSelected = false;
                            movableTiles.clear();
                            turnManager.Move();
                            break;
                        }
                    }
                }
            }
        }
        GOM->CollisionTest();
    }
    else {
        // 턴 전환 로직
        Rectangle startButton = { screenWidth / 2.0f - 100, screenHeight / 2.0f + 50, 200, 60 };
        if (CheckCollisionPointRec(mouse, startButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            turnManager.StartTurn();
        }
    }

    GOM->UpdateAll(dt);
}
void MainMapState::Draw() {
    ClearBackground(RAYWHITE);
    Math::TransformationMatrix camera_matrix; // 기본 카메라 행렬

    if (!turnManager.IsTransitioning()) {
        // ========== 게임 플레이 화면 그리기 ==========
        gameMap.Draw();

        // 선택 가능한 타일 그리기
        if (generalSelected) {
            for (const auto& tile : movableTiles) {
                DrawCircleV(tile.center, 30, Fade(BLUE, 0.4f));
            }
        }

        // GameObjectManager가 모든 게임 오브젝트(플레이어)를 그림
        GetGSComponent<CS230::GameObjectManager>()->DrawAll(camera_matrix);

        // 여기에 게임 플레이 UI (예: 턴 종료 버튼)를 그릴 수 있습니다.
        Rectangle endTurnButton = { 1260, 750, 110, 40 };
        DrawRectangleRec(endTurnButton, LIGHTGRAY);
        DrawText("TurnEnd", endTurnButton.x + 10, endTurnButton.y + 10, 20, BLACK);
    }
    else {
        // ========== 채워진 턴 전환 화면 그리기 ==========
        // 게임 화면 위에 반투명한 회색 배경을 그려 전환 중임을 표시
        DrawRectangle(0, 0, screenWidth, screenHeight, Fade(GRAY, 0.8f));

        Turn turn = turnManager.GetCurrentTurn();
        const char* turnText = (turn == Turn::P1) ? "PLAYER 1 TURN" : "PLAYER 2 TURN";
        int textWidth = MeasureText(turnText, 60);
        DrawText(turnText, screenWidth / 2 - textWidth / 2, screenHeight / 2 - 100, 60, DARKBLUE);

        // START 버튼 그리기
        Rectangle startButton = { screenWidth / 2.0f - 100, screenHeight / 2.0f + 50, 200, 60 };
        Vector2 mouse = GetMousePosition();
        bool hovering = CheckCollisionPointRec(mouse, startButton);
        DrawRectangleRec(startButton, hovering ? DARKGRAY : LIGHTGRAY);
        DrawText("START", startButton.x + 60, startButton.y + 15, 30, BLACK);
    }
}

void MainMapState::Unload() {
    Engine::GetLogger().LogEvent(GetName() + " Unload");
    // GameState가 소멸될 때 컴포넌트(GameObjectManager)와 그 안의 GameObject들이 자동으로 해제됩니다.
}