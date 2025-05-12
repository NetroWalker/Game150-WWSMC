#include "raylib.h"
#include "Game/map.h"
#include "Game/Battlemap.h"
#include <cmath>
#include "Game/General.h"

#define screenWidth 1500
#define screenHeight 1000

enum GameState {
    STATE_MAIN_MAP,
    STATE_BATTLE_MAP
};

std::vector<HexTile> GetMovableTiles(Map& map, HexTile* from) {
    std::vector<HexTile> result;
    if (!from) return result;

    int x = from->x;
    int y = from->y;

    if (x == 1 || x == 3) {
        int dx[] = { 0, +1, -1, -1,  0, +1 };
        int dy[] = { -1,  0,  0, +1, +1, +1 };
        for (int i = 0; i < 6; i++) {
            int nx = x + dx[i];
            int ny = y + dy[i];
            if (HexTile* t = map.GetTileAt(nx, ny)) {
                result.push_back(*t);
            }
        }
    }
    else {
        int dx[] = { 0,  0, +1, -1, -1, +1 };
        int dy[] = { -1, +1,  0,  0, -1, -1 };
        for (int i = 0; i < 6; i++) {
            int nx = x + dx[i];
            int ny = y + dy[i];
            if (HexTile* t = map.GetTileAt(nx, ny)) {
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
        screenWidth / 2.0f - ((mapW - 1) * radiusX * 1.5f) / 2.0f,
        screenHeight / 2.0f - ((mapH - 1) * radiusY * sqrtf(3.0f) * squashFactor) / 2.0f
    };

    Map map(Fcenter, radiusX, radiusY);
    map.SetPoint();

    General* general = new General(map.GetTiles()[0].center);
    General* general2 = nullptr;
    Texture2D general2Texture;

    HexTile* tile33 = map.GetTileAt(3, 3);
    if (tile33) {
        general2 = new General(tile33->center);
        general2Texture = LoadTexture("assets/General1.png");
    }

    BattleMap battleMap(screenWidth, screenHeight, 4);

    bool generalSelected = false;
    std::vector<HexTile> movableTiles;

    GameState currentState = STATE_MAIN_MAP;

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (currentState == STATE_MAIN_MAP) {
            map.Update();
            map.SetPoint();

            Vector2 mouse = GetMousePosition();

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !general->IsMoving()) {
                if (CheckCollisionPointCircle(mouse, general->GetPosition(), 50)) {
                    if (!generalSelected) {
                        generalSelected = true;
                        HexTile* from = map.GetTileAtPosition(general->GetFootPosition());
                        movableTiles = GetMovableTiles(map, from);
                    }
                    else {
                        generalSelected = false;
                        movableTiles.clear();
                    }
                }
                else if (generalSelected) {
                    for (auto& tile : movableTiles) {
                        if (CheckCollisionPointCircle(mouse, tile.center, radiusX * 0.8f)) {
                            general->SetPosition(tile.center);
                            generalSelected = false;
                            movableTiles.clear();
                            break;
                        }
                    }
                }
            }

            for (const auto& tile : movableTiles) {
                DrawCircleV(tile.center, 30, Fade(BLUE, 0.4f));
            }

            general->Update();
            general->Draw();

            if (general2) {
                Vector2 pos = general2->GetPosition();
                float scale = 0.3f;
                float width = general2Texture.width * scale;
                float height = general2Texture.height * scale;
                float positionOffset = 20.0f;

                DrawTexturePro(
                    general2Texture,
                    { 0, 0, (float)general2Texture.width, (float)general2Texture.height },
                    { pos.x, pos.y + positionOffset, width, height },
                    { width / 2.0f, height },
                    0.0f,
                    WHITE
                );
            }

            if (!general->IsMoving() && general2 && !general2->IsMoving()) {
                Vector2 pos1 = general->GetFootPosition();
                Vector2 pos2 = general2->GetFootPosition();
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

    delete general;
    if (general2) delete general2;
    UnloadTexture(general2Texture);
    CloseWindow();
    return 0;
}
