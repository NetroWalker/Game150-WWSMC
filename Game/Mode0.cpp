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

    chatWindowTexture = LoadTexture("Assets/chat_window.png");
}

Mode0::~Mode0() {
    delete tutorialGeneral;
    delete staticGeneral;
    UnloadTexture(chatWindowTexture);
}

void Mode0::SetDialogueStep(int step) {
    dialogueStep = step;
    waitingForSpace = false;
    delayTimer = 0.0f;
    canMove = false;  // ✅ 기본은 이동 불가

    switch (step) {
    case 0:
        fullDialogue = "Hello, you're the new general, aren't you?";
        waitingForSpace = true;
        break;
    case 1:
        fullDialogue = "Your mission is to help our tribe thrive by driving out the other tribes from this vast land!";
        waitingForSpace = true;
        break;
    case 2:
        fullDialogue = "You can move the general to any tile you want using the mouse!";
        canMove = true;  // ✅ 여기서 이동 가능
        break;
    case 3:
        fullDialogue = "Good!!!";
        break;
    case 4:
        fullDialogue = "Normally, you can only move once per turn.";
        waitingForSpace = true;
        break;
    case 5:
        fullDialogue = "But just this time, I'll let you move once more.";
        waitingForSpace = true;
        canMove = true;  // ✅ 다시 이동 허용
        break;
    case 6:
        fullDialogue = "When your general encounters an enemy general, a battle will begin!";
        waitingForSpace = true;\
        canMove = true;
        break;
    case 7:
        tutorialDone = true;
        return;
    }

    currentDialogue.clear();
    dialogueCharIndex = 0;
    dialogueCharTimer = 0.0f;
}

void Mode0::Update() {
    tutorialTimer += GetFrameTime();

    if (tutorialTimer >= 2.0f && chatAlpha < 1.0f) {
        chatAlpha += GetFrameTime();
        if (chatAlpha >= 1.0f) {
            chatAlpha = 1.0f;
            dialogueShown = true;
            SetDialogueStep(0);
        }
    }

    if (dialogueShown && dialogueCharIndex < (int)fullDialogue.length()) {
        dialogueCharTimer += GetFrameTime();
        if (dialogueCharTimer >= 0.03f) {
            currentDialogue += fullDialogue[dialogueCharIndex++];
            dialogueCharTimer = 0.0f;
        }
    }

    if (dialogueShown && waitingForSpace && IsKeyPressed(KEY_SPACE) && dialogueCharIndex == (int)fullDialogue.length()) {
        SetDialogueStep(dialogueStep + 1);
    }

    if (dialogueStep == 2 && tutorialGeneral->GetFootPosition().x == tutorialMap.GetTiles()[1].center.x) {
        SetDialogueStep(3);
    }

    if (dialogueStep == 3 && dialogueCharIndex == (int)fullDialogue.length()) {
        delayTimer += GetFrameTime();
        if (delayTimer >= 2.0f) {
            SetDialogueStep(4);
        }
    }

    // ✅ General 이동 가능 여부를 canMove로 제어
    if (dialogueShown && canMove && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !tutorialGeneral->IsMoving()) {
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
                    break;
                }
            }
        }
    }

    tutorialGeneral->Update();
}

void Mode0::Draw() {
    ClearBackground(BLACK);
    tutorialMap.Draw(tutorialGeneral);

    for (const auto& tile : movableTiles) {
        DrawCircleV(tile.center, 30, Fade(BLUE, 0.4f));
    }

    tutorialGeneral->Draw();
    staticGeneral->Draw();

    if (tutorialTimer >= 2.0f) {
        float scale = 1.0f;
        float width = chatWindowTexture.width * scale;
        float height = chatWindowTexture.height * scale;

        Vector2 pos = {
            (float)GetScreenWidth() / 2.0f - width / 2.0f,
            (float)GetScreenHeight() - height - 30.0f
        };

        DrawTexture(chatWindowTexture, (int)pos.x, (int)pos.y, Fade(WHITE, chatAlpha));

        if (dialogueShown) {
            Font font = GetFontDefault();
            Vector2 textPos = { 120, 635 };
            DrawTextEx(font, currentDialogue.c_str(), textPos, 30, 2.0f, Fade(BLACK, chatAlpha));
        }
    }
}

bool Mode0::IsTutorialDone() const {
    return tutorialDone;
}
