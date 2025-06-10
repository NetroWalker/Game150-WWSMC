#include "MainMapState.h"
#include "BattleMap.h"
#include "States.h"
#include "SquirrelGen.h"
#include "SnakeGen.h"
#include "LinearMovement.h"
#include "../Engine/Collision.h"
#include "../Engine/ShowCollision.h"
#include "Castle.h"
#include "../Engine/Camera.h"
#include <cmath>

MainMapState::MainMapState(int sw, int sh) :
    screenWidth(sw), screenHeight(sh),
    radiusX(200.0f), radiusY(200.0f),
    gameMap(Vector2{ screenWidth / 2.0f - ((5 - 1) * 200.0f * 1.5f) / 2.0f,
                     screenHeight / 2.0f - ((5 - 1) * 200.0f * sqrtf(3.0f) * 0.5f) / 2.0f },
        200.0f, 200.0f, 8, 10, true),
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
    player1_resources = new Stone();
    player2_resources = new Stone();
    player1 = new SquirrelGen({ (double)gameMap.GetTiles()[0].center.x, (double)gameMap.GetTiles()[0].center.y });
    player2 = new SnakeGen({ (double)tile33->center.x, (double)tile33->center.y });
    castle1 = new Castle({ (double)gameMap.GetTiles()[0].center.x, (double)gameMap.GetTiles()[0].center.y
        }, "Assets/castle_me.spt");
    castle2 = new Castle({ (double)tile33->center.x, (double)tile33->center.y }, "Assets/castle_eneme.spt");

    GetGSComponent<CS230::GameObjectManager>()->Add(castle1);
    GetGSComponent<CS230::GameObjectManager>()->Add(castle2);
    GetGSComponent<CS230::GameObjectManager>()->Add(player1);
    GetGSComponent<CS230::GameObjectManager>()->Add(player2);
    player1->SetScale({ 0.3, 0.3 });
    player2->SetScale({ 0.3, 0.3 });
    castle1->SetScale({ 0.7, 0.7 });
    castle2->SetScale({ 0.7, 0.7 });
    generalSelected = false;
    movableTiles.clear();
    startingTile = nullptr;
}

void MainMapState::Update(double dt) {
    player1_resources->Update(dt, player1_castles.size());
    player2_resources->Update(dt, player2_castles.size());
    Vector2 mouse = GetMousePosition();
    Turn turn = turnManager.GetCurrentTurn();
    CS230::Camera* camera = GetGSComponent<CS230::Camera>();
    Turn current_turn = turnManager.GetCurrentTurn();
    CS230::GameObject* currentGeneral = (current_turn == Turn::P1) ? player1 : player2;
    CS230::GameObject* enemyGeneral = (current_turn == Turn::P1) ? player2 : player1;
    friendlyCastle = (current_turn == Turn::P1) ? castle1 : castle2;
    enemyCastle = (current_turn == Turn::P1) ? castle2 : castle1;
    LinearMovement* currentMovement = currentGeneral->GetGOComponent<LinearMovement>();

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


        if (turnManager.CanMove()) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !currentMovement->IsMoving()) {
                CS230::RectCollision* collisionComp = currentGeneral->GetGOComponent<CS230::RectCollision>();

                if (collisionComp != nullptr && camera != nullptr) {
                    Rectangle click_box = collisionComp->ToRaylibScreenRect(camera->GetMatrix());
                    if (CheckCollisionPointRec(mouse, click_box)) {
                        generalSelected = !generalSelected;
                        if (generalSelected) {
                            startingTile = gameMap.GetTileAtPosition(currentMovement->GetFootPosition());
                            movableTiles = gameMap.GetMovableTiles(startingTile);
                        }
                        else {
                            movableTiles.clear();
                            startingTile = nullptr;
                        }
                    }
                }

                if (generalSelected && (collisionComp == nullptr || !CheckCollisionPointRec(mouse, collisionComp->ToRaylibScreenRect(camera->GetMatrix())))) {
                    const int screen_height = GetScreenHeight();
                    const Math::TransformationMatrix& camera_matrix = camera->GetMatrix();
                    for (auto& tile : movableTiles) {
                        Math::vec2 transformed_pos = camera_matrix * Math::vec2(tile.center);
                        Vector2 screen_pos = { (float)transformed_pos.x, screen_height - (float)transformed_pos.y };
                        if (CheckCollisionPointCircle(mouse, screen_pos, 35.0f)) {
                            currentMovement->MoveTo({ (double)tile.center.x, (double)tile.center.y });
                            generalSelected = false;
                            movableTiles.clear();
                            startingTile = nullptr;
                            turnManager.Move();
                            break;
                        }
                    }
                }
            }
        }


        Rectangle endTurnButton = { 1260, 750, 110, 40 };
        if (CheckCollisionPointRec(mouse, endTurnButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            turnManager.EndTurn();
            generalSelected = false;
            movableTiles.clear();
            startingTile = nullptr;
        }
    }
    else {
            Rectangle startButton = { screenWidth / 2.0f - 100, screenHeight / 2.0f + 50, 200, 60 };
            if (CheckCollisionPointRec(mouse, startButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                turnManager.StartTurn();
            }
        }
    GetGSComponent<CS230::GameObjectManager>()->UpdateAll(dt);
    }
void MainMapState::Draw() {
    ClearBackground(RAYWHITE);
    CS230::Camera* camera = GetGSComponent<CS230::Camera>();
    Math::TransformationMatrix camera_matrix;
    if (camera != nullptr) {
        camera_matrix = camera->GetMatrix();
    }
    const int screen_height = GetScreenHeight();

    if (!turnManager.IsTransitioning()) {
        CS230::GameObject* currentGeneral = (turnManager.GetCurrentTurn() == Turn::P1) ? player1 : player2;
        CS230::GameObject* enemyGeneral = (turnManager.GetCurrentTurn() == Turn::P1) ? player2 : player1;
        Turn current_turn = turnManager.GetCurrentTurn();
        HexTile* currentTile = nullptr;
        Castle* friendlyCastle = (current_turn == Turn::P1) ? castle1 : castle2;
        Castle* enemyCastle = (current_turn == Turn::P1) ? castle2 : castle1;

        if (currentGeneral != nullptr) {
            currentTile = gameMap.GetTileAtPosition(currentGeneral->GetGOComponent<LinearMovement>()->GetFootPosition());
        }

        gameMap.Draw(currentTile, camera_matrix);

        if (generalSelected) {
            if (startingTile != nullptr) {
                Math::vec2 transformed_pos = camera_matrix * Math::vec2(startingTile->center);
                Vector2 screen_pos = { (float)transformed_pos.x, screen_height - (float)transformed_pos.y };
            }
            for (const auto& tile : movableTiles) {
                Math::vec2 transformed_pos = camera_matrix * Math::vec2(tile.center);
                Vector2 screen_pos = { (float)transformed_pos.x, screen_height - (float)transformed_pos.y };
                DrawCircleV(screen_pos, 30, Fade(BLUE, 0.4f));
            }
        }

        if (currentGeneral != nullptr) {
            currentGeneral->Draw(camera_matrix);
        }

        if (currentGeneral != nullptr && enemyGeneral != nullptr) {
            HexTile* generalTile = currentTile;
            HexTile* enemyTile = gameMap.GetTileAtPosition(enemyGeneral->GetGOComponent<LinearMovement>()->GetFootPosition());

            if (generalTile != nullptr && enemyTile != nullptr &&
                gameMap.IsNeighborTile(generalTile->x, generalTile->y, enemyTile->x, enemyTile->y)) {
                enemyGeneral->Draw(camera_matrix);
            }
        }
        //GetGSComponent<CS230::GameObjectManager>()->DrawAll(camera_matrix);
        
        if (currentGeneral != nullptr) {
            currentGeneral->Draw(camera_matrix);
        }

        if (currentTile != nullptr) {
            HexTile* enemyGeneralTile = gameMap.GetTileAtPosition(enemyGeneral->GetGOComponent<LinearMovement>()->GetFootPosition());
            if (enemyGeneralTile != nullptr && gameMap.IsNeighborTile(currentTile->x, currentTile->y, enemyGeneralTile->x, enemyGeneralTile->y)) {
                enemyGeneral->Draw(camera_matrix);
            }

            HexTile* enemyCastleTile = gameMap.GetTileAtPosition(enemyCastle->GetPosition());
            if (enemyCastleTile != nullptr && gameMap.IsNeighborTile(currentTile->x, currentTile->y, enemyCastleTile->x, enemyCastleTile->y)) {
                enemyCastle->Draw(camera_matrix);
            }
        }
        if (friendlyCastle != nullptr) {
            friendlyCastle->Draw(camera_matrix);
        }
        Rectangle endTurnButton = { 1260, 750, 110, 40 };
        DrawRectangleRec(endTurnButton, LIGHTGRAY);
        DrawText("TurnEnd", endTurnButton.x + 10, endTurnButton.y + 10, 20, BLACK);
        std::string p1_text = "P1 Stone: " + std::to_string(player1_resources->GetStoneCount());
        DrawText(p1_text.c_str(), 10, 10, 20, BLACK);

        std::string p2_text = "P2 Stone: " + std::to_string(player2_resources->GetStoneCount());
        int p2_text_width = MeasureText(p2_text.c_str(), 20);
        DrawText(p2_text.c_str(), screenWidth - p2_text_width - 10, 10, 20, BLACK);
    }
    else {
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