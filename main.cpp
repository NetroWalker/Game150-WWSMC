#include "raylib.h"
#include "Game/map.h"
#include "Game/Battlemap.h"
#include "Game/General.h"
#include "Game/Mode0.h"
#include "Engine/TurnManager.h"
#include <cmath>

#define screenWidth 1500
#define screenHeight 1000

enum GameState {
    STATE_TUTORIAL,
    STATE_MAIN_MAP,
    STATE_BATTLE_MAP
};

std::vector<HexTile> GetMovableTiles(Map& map, HexTile* from) {
    std::vector<HexTile> result;
    if (!from) return result;

    int x = from->x;
    int y = from->y;

    if (x % 2 == 1) {
        int dx[] = { 0, +1, -1, -1,  0, +1 };
        int dy[] = { -1,  0,  0, +1, +1, +1 };
        for (int i = 0; i < 6; i++) {
            if (HexTile* t = map.GetTileAt(x + dx[i], y + dy[i])) {
                result.push_back(*t);
            }
        }
    }
    else {
        int dx[] = { 0,  0, +1, -1, -1, +1 };
        int dy[] = { -1, +1,  0,  0, -1, -1 };
        for (int i = 0; i < 6; i++) {
            if (HexTile* t = map.GetTileAt(x + dx[i], y + dy[i])) {
                result.push_back(*t);
            }
        }
    }

    return result;
}

int main() {
    InitWindow(screenWidth, screenHeight, "NoName");
    SetTargetFPS(60);

    float radiusX = 200.0f;
    float radiusY = 200.0f;
    float squashFactor = 0.3f;

    Vector2 Fcenter = {
        screenWidth / 2.0f - ((5 - 1) * radiusX * 1.5f) / 2.0f,
        screenHeight / 2.0f - ((5 - 1) * radiusY * sqrtf(3.0f) * squashFactor) / 2.0f
    };

    Map map(Fcenter, radiusX, radiusY, 5, 5, true);  // ✅ 일반 맵
    map.SetPoint();  // ✅ 반드시 타일 초기화

    HexTile* tile33 = map.GetTileAt(3, 3);
    if (!tile33) {
        TraceLog(LOG_ERROR, "tile33 not found. Exiting.");
        CloseWindow();
        return -1;
    }

    General* player1 = new General(map.GetTiles()[0].center, "Assets/General.png");
    General* player2 = new General(tile33->center, "Assets/General1.png");
    std::vector<General*> player = { player1, player2 };

    BattleMap battleMap(screenWidth, screenHeight, 4);
    TurnManager turnmanager;

    bool generalSelected = false;
    std::vector<HexTile> movableTiles;

    Mode0* mode0 = new Mode0({ screenWidth / 2.0f, screenHeight / 2.0f }, radiusX, radiusY);  // ✅ 튜토리얼 맵
    GameState currentState = STATE_TUTORIAL;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (currentState == STATE_TUTORIAL) {
            mode0->Update();
            mode0->Draw();

            if (mode0->IsTutorialDone()) {
                currentState = STATE_MAIN_MAP;
                delete mode0;
            }
        }
        else if (currentState == STATE_MAIN_MAP) {
            map.Update();
            map.Draw();  // ✅ 일반 맵 타일 그리기

            Turn turn = turnmanager.GetCurrentTurn();
            Vector2 mouse = GetMousePosition();

            int playerIndex = (turn == Turn::P1) ? 0 : 1;
            General* currentGeneral = player[playerIndex];

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !currentGeneral->IsMoving()) {
                if (CheckCollisionPointCircle(mouse, currentGeneral->GetPosition(), 50)) {
                    generalSelected = !generalSelected;
                    if (generalSelected) {
                        HexTile* from = map.GetTileAtPosition(currentGeneral->GetFootPosition());
                        movableTiles = GetMovableTiles(map, from);
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
                            turnmanager.EndTurn();
                            turnmanager.StartTurn();
                            break;
                        }
                    }
                }
            }

            for (const auto& tile : movableTiles) {
                DrawCircleV(tile.center, 30, Fade(BLUE, 0.4f));
            }

            for (auto& g : player) {
                g->Update();
                g->Draw();
            }

            if (!player[0]->IsMoving() && !player[1]->IsMoving()) {
                Vector2 pos1 = player[0]->GetFootPosition();
                Vector2 pos2 = player[1]->GetFootPosition();
                float dx = pos1.x - pos2.x;
                float dy = pos1.y - pos2.y;
                float dist = sqrtf(dx * dx + dy * dy);

                if (dist < 1.0f) {
                    currentState = STATE_BATTLE_MAP;
                }
            }
        }
        else if (currentState == STATE_BATTLE_MAP) {
            battleMap.Update();
            battleMap.Draw();
        }

        EndDrawing();
    }

    delete player1;
    delete player2;
    CloseWindow();
    return 0;
}
