#include "Mode0.h"
#include <cmath>

Mode0::Mode0(Vector2 center, float rX, float rY)
    : radiusX(rX), tutorialMap(center, rX, rY, 3, 1, false)
{
    float root3 = sqrtf(3.0f);
    float xOffset = rX * 1.5f;
    float yOffset = rY * root3;
    float squash = 0.5f;

    float totalWidth = (3 - 1) * xOffset;

    std::vector<HexTile>& tiles = const_cast<std::vector<HexTile>&>(tutorialMap.GetTiles());
    tiles.clear();

    for (int x = 0; x < 3; x++) {
        int y = 0;
        float hexX = center.x - totalWidth / 2 + x * xOffset;
        float hexY = center.y + ((x % 2) * (yOffset / 2.0f));
        float squashedHexY = hexY * squash;

        HexTile tile = { x, y, { hexX, squashedHexY } };
        tiles.push_back(tile);
    }

    tutorialGeneral = new General(tiles[0].center, "Assets/General.png");
    staticGeneral = new General(tiles[2].center, "Assets/General1.png");

    chatWindowTexture = LoadTexture("Assets/chat_window.png");  // ✅ 설명창 이미지 로드
}

Mode0::~Mode0() {
    delete tutorialGeneral;
    delete staticGeneral;
    UnloadTexture(chatWindowTexture);  // ✅ 텍스처 해제
}

void Mode0::Update() {
    tutorialTimer += GetFrameTime();

    // ✅ 설명창 등장 타이밍 이후 밝기 증가
    if (tutorialTimer >= 2.0f && chatAlpha < 1.0f) {
        chatAlpha += GetFrameTime();  // 초당 1씩 증가 → 약 1초 fade-in
        if (chatAlpha > 1.0f) chatAlpha = 1.0f;
    }

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !tutorialGeneral->IsMoving()) {
        Vector2 mouse = GetMousePosition();

        if (CheckCollisionPointCircle(mouse, tutorialGeneral->GetPosition(), 50)) {
            if (!generalSelected) {
                generalSelected = true;
                HexTile* from = tutorialMap.GetTileAtPosition(tutorialGeneral->GetFootPosition());
                if (from) {
                    int x = from->x;
                    int y = from->y;
                    movableTiles.clear();

                    for (int dx = -1; dx <= 1; dx++) {
                        int nx = x + dx;
                        if (nx >= 0 && nx <= 2) {
                            if (nx == x) continue;
                            if (HexTile* t = tutorialMap.GetTileAt(nx, y)) {
                                movableTiles.push_back(*t);
                            }
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
    staticGeneral->Draw();

    // ✅ 설명창 페이드인 출력
    if (tutorialTimer >= 2.0f) {
        float scale = 1.0f;
        float width = chatWindowTexture.width * scale;
        float height = chatWindowTexture.height * scale;

        Vector2 pos = {
            (float)GetScreenWidth() / 2.0f - width / 2.0f,
            (float)GetScreenHeight() - height - 30.0f
        };

        DrawTexture(chatWindowTexture, (int)pos.x, (int)pos.y, Fade(WHITE, chatAlpha));
    }
}

bool Mode0::IsTutorialDone() const {
    return tutorialDone;
}
