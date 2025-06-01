#include "raylib.h"
#include "Game/map.h"
#include "Game/Battlemap.h"
#include "Game/General.h"
#include "Game/Mode0.h"
#include "Game/menu.h"
#include "Game/States.h"
#include "Engine/Engine.h"
#include "Game/Menu.h"
#include "Game/Mode0.h"
#include "Game/map.h"
#include "Game/BattleMap.h"

#include "Engine/TurnManager.h"
#include <cmath>

#define screenWidth 1500
#define screenHeight 1000

int main() {

    float radiusX = 200.0f;
    float radiusY = 200.0f;
    float squashFactor = 0.3f;

    Engine& engine = Engine::Instance();
    engine.Start("A COOOOOOORN? REALLY?");

    Menu* menu = new Menu(screenWidth, screenHeight);
    engine.GetGameStateManager().AddGameState(*menu);

    Mode0* mode0 = new Mode0({ screenWidth / 2.0f, screenHeight / 2.0f }, radiusX, radiusY);
    engine.GetGameStateManager().AddGameState(*mode0);

    Vector2 Fcenter = {
        screenWidth / 2.0f - ((5 - 1) * radiusX * 1.5f) / 2.0f,
        screenHeight / 2.0f - ((5 - 1) * radiusY * sqrtf(3.0f) * squashFactor) / 2.0f
    };

    Map map(Fcenter, radiusX, radiusY, 5, 5, true);
    map.SetPoint();
    engine.GetGameStateManager().AddGameState(map);

    BattleMap battleMap(screenWidth, screenHeight);
    engine.GetGameStateManager().AddGameState(battleMap);


    HexTile* tile33 = map.GetTileAt(3, 3);
    if (!tile33) {
        TraceLog(LOG_ERROR, "tile33 not found. Exiting.");
        CloseWindow();
        return -1;
    }

    General* player1 = new General(map.GetTiles()[0].center, "Assets/General.png");
    General* player2 = new General(tile33->center, "Assets/General1.png");
    std::vector<General*> player = { player1, player2 };

    
    TurnManager turnmanager;
    
   

    bool generalSelected = false;
    std::vector<HexTile> movableTiles;

    //GameState currentState = States::STATE_MENU; gamestart: MENU

    while (engine.HasGameEnded() == false) {
        engine.Update();

        if (currentState == STATE_MENU) {
            menu->Update();
            menu->Draw();

            if (menu->StartTutorialClicked()) {
                currentState = States::STATE_TUTORIAL;
            }
            else if (menu->StartMainMapClicked()) {
                currentState = STATE_MAIN_MAP;
            }
        }

        else if (currentState == STATE_TUTORIAL) {
            mode0->Update();
            mode0->Draw();
            if (mode0->IsTutorialDone()) {
                currentState = STATE_MAIN_MAP;
                delete mode0;
            }
        }

        else if (currentState == STATE_MAIN_MAP) {
            Vector2 mouse = GetMousePosition();
            Turn turn = turnmanager.GetCurrentTurn();

            if (!turnmanager.IsTransitioning()) {
                int playerIndex = (turn == Turn::P1) ? 0 : 1;
                General* currentGeneral = player[playerIndex];
                int enemyIndex = (turn == Turn::P1) ? 1 : 0;
                General* enemyGeneral = player[enemyIndex];

                map.Update();
                map.Draw(currentGeneral);


                HexTile* currentTile = map.GetTileAtPosition(currentGeneral->GetFootPosition());
                HexTile* enemyTile = map.GetTileAtPosition(enemyGeneral->GetFootPosition());

                if (turnmanager.CanMove()) {
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !currentGeneral->IsMoving()) {
                        if (CheckCollisionPointCircle(mouse, currentGeneral->GetPosition(), 50)) {
                            generalSelected = !generalSelected;
                            if (generalSelected) {
                                HexTile* from = map.GetTileAtPosition(currentGeneral->GetFootPosition());
                                movableTiles = map.GetMovableTiles(map, from);
                            }
                            else {
                                movableTiles.clear();
                            }
                        }
                        else if (generalSelected) {
                            for (auto& tile : movableTiles) {
                                if (CheckCollisionPointCircle(mouse, tile.center, radiusX * 0.8f)) {
                                    currentGeneral->SetPosition(tile.center);
                                    generalSelected = false;
                                    movableTiles.clear();
                                    turnmanager.Move();
                                    break;
                                }
                            }
                        }
                    }
                }

                for (const auto& tile : movableTiles) {
                    DrawCircleV(tile.center, 30, Fade(BLUE, 0.4f));
                }

                currentGeneral->Update();
                currentGeneral->Draw();
                
                if (map.GetTileAtPosition(currentGeneral->GetFootPosition()) &&
                    map.GetTileAtPosition(enemyGeneral->GetFootPosition())) {
                    HexTile* cTile = map.GetTileAtPosition(currentGeneral->GetFootPosition());
                    HexTile* eTile = map.GetTileAtPosition(enemyGeneral->GetFootPosition());
                    if (map.IsNeighborTile(cTile->x, cTile->y, eTile->x, eTile->y)) {
                        enemyGeneral->Update();
                        enemyGeneral->Draw();
                    }
                }

                if (!player[0]->IsMoving() && !player[1]->IsMoving()) {
                    Vector2 pos1 = player[0]->GetFootPosition();
                    Vector2 pos2 = player[1]->GetFootPosition();
                    float dx = pos1.x - pos2.x;
                    float dy = pos1.y - pos2.y;
                    float dist = sqrtf(dx * dx + dy * dy);

                    if (dist < 1.0f) {
                        currentState = STATE_BATTLE_MAP;
                        battleMap.LoadSoldiersForTurn(turnmanager.GetCurrentTurn());
                    }
                }

                Rectangle moveRect = { 1200, 750, 40, 40 };
                DrawRectangleRec(moveRect, turnmanager.CanMove() ? GREEN : RED);
                DrawText("Move", 1200, 795, 20, DARKGRAY);

                Rectangle endTurnButton = { 1260, 750, 110, 40 };
                DrawRectangleRec(endTurnButton, LIGHTGRAY);
                DrawText("TurnEnd", endTurnButton.x + 10, endTurnButton.y + 10, 20, BLACK);
                if (CheckCollisionPointRec(mouse, endTurnButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    turnmanager.EndTurn();
                }
            }
            else {
                DrawRectangle(0, 0, screenWidth, screenHeight, GRAY);
                const char* turnText = (turnmanager.GetCurrentTurn() == Turn::P1) ? "P1 TURN" : "P2 TURN";
                int textWidth = MeasureText(turnText, 60);
                DrawText(turnText, screenWidth / 2 - textWidth / 2, screenHeight / 2 - 100, 60, DARKBLUE);

                Rectangle startButton = { screenWidth / 2 - 100, screenHeight / 2 + 50, 200, 60 };
                bool hovering = CheckCollisionPointRec(mouse, startButton);
                DrawRectangleRec(startButton, hovering ? DARKGRAY : LIGHTGRAY);
                DrawText("START", startButton.x + 60, startButton.y + 15, 30, BLACK);
                if (hovering && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                    turnmanager.StartTurn();
                }
            }
        }
        engine.Stop();
        return 0;
        else if (currentState == STATE_BATTLE_MAP) {
            battleMap.Update();
            battleMap.Draw();
        }
    }

    delete player1;
    delete player2;
    delete menu;
}
