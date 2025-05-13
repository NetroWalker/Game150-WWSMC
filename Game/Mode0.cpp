
#include "Mode0.h"
#include <cmath>

Mode0::Mode0(Vector2 center, float rX, float rY)
    : radiusX(rX), tutorialMap(center, rX, rY)
{
    // 수동으로 (0,0), (1,0), (2,0) 타일만 생성
    tutorialMap.GetTiles().clear();
    float root3 = sqrtf(3.0f);
    float xOffset = rX * 1.5f;
    float yOffset = rY * root3;
    float squash = 0.5f;

    for (int x = 0; x < 3; x++) {
        int y = 0;
        float hexX = center.x + x * xOffset;
        float hexY = center.y + y * yOffset + ((x % 2) * (yOffset / 2.0f));
        float squashedHexY = hexY * squash;

        HexTile tile;
        tile.x = x;
        tile.y = y;
        tile.center = { hexX, squashedHexY };
        tutorialMap.GetTiles().push_back(tile);
    }

    tutorialGeneral = new General(tutorialMap.GetTiles()[0].center);
}

Mode0::~Mode0() {
    delete tutorialGeneral;
}

void Mode0::Update() {
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !tutorialGeneral->IsMoving()) {
        Vector2 mouse = GetMousePosition();

        if (CheckCollisionPointCircle(mouse, tutorialGeneral->GetPosition(), 50)) {
            if (!generalSelected) {
                generalSelected = true;
                HexTile* from = tutorialMap.GetTileAtPosition(tutorialGeneral->GetFootPosition());
                if (from) {
                    // 가능한 이동 타일: 좌우
                    int x = from->x;
                    int y = from->y;
                    movableTiles.clear();
                    for (int dx = -1; dx <= 1; dx++) {
                        int nx = x + dx;
                        if (nx >= 0 && nx <= 2) {
                            HexTile* t = tutorialMap.GetTileAt(nx, y);
                            if (t) movableTiles.push_back(*t);
                        }
                    }
                }
            }
            else {
                generalSelected = false;
                movableTiles.clear();
            }
        }
        else if (generalSelected) {
            for (auto& tile : movableTiles) {
                if (CheckCollisionPointCircle(mouse, tile.center, radiusX * 0.8f)) {
                    tutorialGeneral->SetPosition(tile.center);
                    generalSelected = false;
                    movableTiles.clear();

                    if (tile.x == 2) {
                        tutorialDone = true;
                    }
                    break;
                }
            }
        }
    }

    tutorialGeneral->Update();
}

void Mode0::Draw() {
    ClearBackground(BLACK);
    tutorialMap.Draw();
    for (const auto& tile : movableTiles) {
        DrawCircleV(tile.center, 30, Fade(BLUE, 0.4f));
    }
    tutorialGeneral->Draw();
}

bool Mode0::IsTutorialDone() const {
    return tutorialDone;
}
