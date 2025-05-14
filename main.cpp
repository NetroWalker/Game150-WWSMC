//main.cpp
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
    STATE_BATTLE_MAP,
    STATE_WIN_SCREEN
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
    //map.DistributeResources();

    int winningPlayerIndex = -1; // -1: 아직 승리자 없음, 0: player[0] 승리, 1: player[1] 승리

    HexTile* tile33 = map.GetTileAt(3, 3);
    if (!tile33) {
        TraceLog(LOG_ERROR, "tile33 not found. Exiting.");
        CloseWindow();
        return -1;
    }

    General* player1 = new General(map.GetTiles()[0].center, "Assets/General.png");
    General* player2 = new General(tile33->center, "Assets/General1.png");
    std::vector<General*> player = { player1, player2 };

    BattleMap battleMap(screenWidth, screenHeight);
    TurnManager turnmanager;

    bool generalSelected = false;
    std::vector<HexTile> movableTiles;

    Mode0* mode0 = new Mode0({ screenWidth / 2.0f, screenHeight / 2.0f }, radiusX, radiusY);
    GameState currentState = STATE_TUTORIAL;

    const char* onScreenMessage = nullptr;
    float onScreenMessageTimer = 0.0f;
    const float ON_SCREEN_MESSAGE_DURATION = 3.0f;

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

                // 인접 시에만 적 캐릭터 보임
                if (map.GetTileAtPosition(currentGeneral->GetFootPosition()) &&
                    map.GetTileAtPosition(enemyGeneral->GetFootPosition())) {
                    HexTile* cTile = map.GetTileAtPosition(currentGeneral->GetFootPosition());
                    HexTile* eTile = map.GetTileAtPosition(enemyGeneral->GetFootPosition());
                    if (abs(cTile->x - eTile->x) <= 1 && abs(cTile->y - eTile->y) <= 1) {
                        enemyGeneral->Update();
                        enemyGeneral->Draw();
                    }
                }

                if (!player[0]->IsMoving() && !player[1]->IsMoving()) 
                {
                    Vector2 pos1 = player[0]->GetFootPosition();
                    Vector2 pos2 = player[1]->GetFootPosition();
                    float dx = pos1.x - pos2.x;
                    float dy = pos1.y - pos2.y;
                    float dist = sqrtf(dx * dx + dy * dy);
                    if (dist < 1.0f) 
                    {
                        currentState = STATE_BATTLE_MAP;
                        battleMap.LoadSoldiersForTurn(turnmanager.GetCurrentTurn());  // ✅ 병사 에셋 결정
                    }
                }

                Rectangle moveRect = { 1200, 750, 40, 40 };
                Color moveColor = turnmanager.CanMove() ? GREEN : RED;
                DrawRectangleRec(moveRect, moveColor);
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
            for (const auto& tile : movableTiles) {
                DrawCircleV(tile.center, 30, Fade(BLUE, 0.4f));
            }

            for (auto& g : player) {
                bool wasMoving = g->IsMoving(); // Update 전 상태 저장
                g->Update(); // 장군 위치 업데이트
                bool isNowMoving = g->IsMoving(); // Update 후 상태 저장

                // 장군 이동이 방금 끝났다면
                if (wasMoving && !isNowMoving) {
                    // 장군이 도달한 타일을 가져옵니다.
                    HexTile* landedTile = map.GetTileAtPosition(g->GetFootPosition());
                    if (landedTile != nullptr) {
                        // 어떤 플레이어의 장군인지 확인합니다.
                        int movedPlayerIndex = -1;
                        if (g == player[0]) movedPlayerIndex = 0;
                        else if (g == player[1]) movedPlayerIndex = 1;

                        // 플레이어 장군이 맞다면 승리 조건 체크
                        if (movedPlayerIndex != -1) {
                            // --- 승리 조건 체크 ---
                            // player[0] (인덱스 0)이 상대방 성 타일 (맵의 우하단: mapW-1, mapH-1)에 도달했다면 승리
                            if (movedPlayerIndex == 0) {
                                if (landedTile->x == map.GetMapW() - 1 && landedTile->y == map.GetMapH() - 1) {
                                    winningPlayerIndex = 0; // player[0] 승리
                                    currentState = STATE_WIN_SCREEN; // 승리 화면 상태로 전환
                                    DrawText("Player 1 wins!!", GetScreenWidth() / 2, GetScreenHeight() / 2, 20, BLUE);
                                }
                            }
                            // player[1] (인덱스 1)이 상대방 성 타일 (맵의 좌상단: 0,0)에 도달했다면 승리
                            else if (movedPlayerIndex == 1) {
                                if (landedTile->x == 0 && landedTile->y == 0) {
                                    winningPlayerIndex = 1; // player[1] 승리
                                    currentState = STATE_WIN_SCREEN; // 승리 화면 상태로 전환
                                    DrawText("Player 2 wins!!", GetScreenWidth() / 2, GetScreenHeight() / 2, 20, RED);
                                }
                            }
                        }
                    }
                }
                g->Draw();
            }

            // Map 객체로부터 성 그리는 데 필요한 정보들을 가져옵니다.
            float castleWidth = map.GetRadiusX() * 2.0f * 0.8f;
            float castleHeight = map.GetRadiusY() * 2.0f * map.GetSquashFactor() * 1.1f;
            Texture2D meTexture = map.GetMeTexture();     // 아군 성 텍스처
            Texture2D enemeTexture = map.GetEnemeTexture(); // 적군 성 텍스처
            for (const auto& tile : map.GetTiles()) {
                if (tile.hasCastle) {
                    // 타일 소유자에 따라 사용할 텍스처를 결정
                    Texture2D castleTex = (tile.castleOwner == 0) ? meTexture : enemeTexture;
                    Vector2 pos = {
                        tile.center.x - castleTex.width / 2.0f,
                        tile.center.y - castleTex.height / 2.0f
                    };

                    // 성 텍스처를 그립니다.
                    DrawTextureV(castleTex, pos, WHITE);
                }
            }
            // main.cpp - STATE_MAIN_MAP 안에 HUD 그리기 추가
            DrawText(TextFormat("P1 - Wood: %d  Stone: %d", player1->GetWood(), player1->GetStone()), 10, 10, 20, DARKGREEN);
            DrawText(TextFormat("P2 - Wood: %d  Stone: %d", player2->GetWood(), player2->GetStone()), screenWidth - 250, screenHeight - 40, 20, DARKBLUE);

            if (IsKeyPressed(KEY_B)) {
                // 현재 턴인 플레이어와 해당 장군을 가져옵니다.
                int currentPlayerIndex = (turnmanager.GetCurrentTurn() == Turn::P1) ? 0 : 1;
                General* currentGeneral = player[currentPlayerIndex];

                // 장군이 서 있는 타일을 찾습니다.
                HexTile* currentTile = map.GetTileAtPosition(currentGeneral->GetFootPosition());
                if (currentTile != nullptr) {
                    if (currentGeneral->CanBuildCastle()) {
                        if (currentTile->hasCastle) {
                            if (currentTile->castleOwner == currentPlayerIndex) {
                                TraceLog(LOG_INFO, "Player %d: It's already ours!", currentPlayerIndex + 1);
                            } else {
                                // 적 성인 경우 - 점령 또는 대체 (자원 소모 후 소유권 변경)
                                TraceLog(LOG_INFO, "Player %d: Conquering other castle. tile (%d, %d)",
                                                    currentPlayerIndex + 1, currentTile->x, currentTile->y);
                                currentGeneral->UseResourcesForCastle(); // 자원 소모
                                currentTile->castleOwner = currentPlayerIndex; // 소유권 변경
                            }
                        } else {
                            currentGeneral->UseResourcesForCastle(); // 자원 소모
                            currentTile->hasCastle = true; // 타일에 성이 있다고 표시
                            currentTile->castleOwner = currentPlayerIndex; // 성 소유자 설정
                        }
                    } else {
                        // 자원이 부족한 경우
                        TraceLog(LOG_INFO, "Player %d: Lack of resources. (wood 10, stone 10 required)", currentPlayerIndex + 1);
                    }
                } else {
                    // 장군이 타일 위에 없는 경우
                    TraceLog(LOG_WARNING, "General not valid");
                }
            }

            if (!player[0]->IsMoving() && !player[1]->IsMoving()) {
                Vector2 pos1 = player[0]->GetFootPosition();
                Vector2 pos2 = player[1]->GetFootPosition();
                
                // main.cpp - 이동 완료 후 자원 수집 추가
                if (!player[0]->IsMoving() && !player[1]->IsMoving()) {
                    for (int i = 0; i < 2; ++i) {
                        HexTile* tile = map.GetTileAtPosition(player[i]->GetFootPosition());
                        player[i]->CollectResources(tile);
                    }

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
        }

        else if (currentState == STATE_BATTLE_MAP) {
            battleMap.Update();
            battleMap.Draw();
        }
        else if (currentState == STATE_WIN_SCREEN) {
            if (winningPlayerIndex == 0) {
                DrawText("Player 1 wins!!", GetScreenWidth() / 2, GetScreenHeight() / 2, 100, RED);
            }
            else {
                DrawText("Player 2 wins!!", GetScreenWidth() / 2, GetScreenHeight() / 2, 100, BLUE);
            }

        }

        EndDrawing();
    }

    delete player1;
    delete player2;
    CloseWindow();
    return 0;
}
}
