//mode0.cpp
#include"../Engine/Engine.h"
#include "Mode0.h"
#include "States.h"
#include "SquirrelGen.h"
#include "SnakeGen.h"
#include "LinearMovement.h"
#include <cmath>

Mode0::Mode0(Vector2 center, float rX, float rY)
    : tutorialMap(center, rX, rY, 3, 1, false), // 멤버 객체 초기화
    radiusX(rX)
{
    //float root3 = sqrtf(3.0f);
    //float xOffset = rX * 1.5f;
    //float yOffset = rY * root3;
    //float squash = 0.5f;
    //float totalWidth = (3 - 1) * xOffset;
    //
    //std::vector<HexTile>& tiles = const_cast<std::vector<HexTile>&>(tutorialMap->GetTiles());
    //tiles.clear();
    //
    //for (int x = 0; x < 3; x++) {
    //    int y = 0;
    //    float hexX = center.x - totalWidth / 2 + x * xOffset;
    //    float hexY = center.y + ((x % 2) * (yOffset / 2.0f));
    //    float squashedHexY = hexY * squash;
    //
    //    HexTile tile = { x, y, { hexX, squashedHexY } };
    //    tiles.push_back(tile);
    //}
    //
    //tutorialGeneral = new SquirrelGen(tiles[0].center, "Assets/General.png");
    //staticGeneral = new SnakeGen(tiles[2].center, "Assets/General1.png");
    //
    //chatWindowTexture = LoadTexture("Assets/chat_window.png");
}

Mode0::~Mode0() {
    //delete tutorialGeneral;
    //delete staticGeneral;
    //UnloadTexture(chatWindowTexture);
}

void Mode0::SetDialogueStep(int step) {
    dialogueStep = step;
    waitingForSpace = false;
    delayTimer = 0.0f;
    canMove = false;

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
        canMove = true;
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
        canMove = true;
        break;
    case 6:
        fullDialogue = "When your general encounters an enemy general, a battle will begin!";
        waitingForSpace = true; \
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

void Mode0::Load() {
    Engine::GetLogger().LogEvent(GetName() + " Load");

    AddGSComponent(new CS230::GameObjectManager());

    tutorialMap.SetPoint();
    const std::vector<HexTile>& tiles = tutorialMap.GetTiles();

    // ===== 수정된 부분 =====
    // Raylib의 Vector2를 엔진의 Math::vec2 타입으로 변환하여 생성자에 전달합니다.
    tutorialGeneral = new SquirrelGen({ (double)tiles[0].center.x, (double)tiles[0].center.y });
    staticGeneral = new SnakeGen({ (double)tiles[2].center.x, (double)tiles[2].center.y });
    // ======================

    GetGSComponent<CS230::GameObjectManager>()->Add(tutorialGeneral);
    GetGSComponent<CS230::GameObjectManager>()->Add(staticGeneral);

    chatWindowTexture = LoadTexture("Assets/chat_window.png");
    tutorialTimer = 0.0f;
    chatAlpha = 0.0f;
    dialogueShown = false;
    tutorialDone = false;
    generalSelected = false;
    movableTiles.clear();
}

void Mode0::Update(double dt) {
    tutorialTimer += dt;

    if (tutorialTimer >= 2.0f && !dialogueShown) {
        chatAlpha += dt;
        if (chatAlpha >= 1.0f) {
            chatAlpha = 1.0f;
            dialogueShown = true;
            SetDialogueStep(0);
        }
    }

    if (dialogueShown && dialogueCharIndex < (int)fullDialogue.length()) {
        dialogueCharTimer += dt;
        if (dialogueCharTimer >= 0.03f) {
            currentDialogue += fullDialogue[dialogueCharIndex++];
            dialogueCharTimer = 0.0f;
        }
    }

    // ===== 수정: 닫는 중괄호 추가 =====
    if (dialogueShown && waitingForSpace && IsKeyPressed(KEY_SPACE) && dialogueCharIndex == (int)fullDialogue.length()) {
        SetDialogueStep(dialogueStep + 1);
    }

    // ===== 수정: LinearMovement 컴포넌트를 통해 GetFootPosition 호출 =====
    LinearMovement* tg_movement = tutorialGeneral->GetGOComponent<LinearMovement>();
    Math::vec2 tg_foot_pos = tg_movement->GetFootPosition();
    if (dialogueStep == 2 && tg_foot_pos.x == tutorialMap.GetTiles()[1].center.x) {
        SetDialogueStep(3);
    }

    if (dialogueStep == 3 && dialogueCharIndex == (int)fullDialogue.length()) {
        delayTimer += dt;
        if (delayTimer >= 2.0f) {
            SetDialogueStep(4);
        }
    }

    // ===== 수정: 'movement' 변수 선언 및 올바른 함수 호출 =====
    if (dialogueShown && canMove && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        LinearMovement* movement = tutorialGeneral->GetGOComponent<LinearMovement>();
        if (!movement->IsMoving()) {
            Vector2 mouse = GetMousePosition();
            Math::vec2 currentPos_math = tutorialGeneral->GetPosition();
            Vector2 currentPos_raylib = { (float)currentPos_math.x, (float)currentPos_math.y };

            if (CheckCollisionPointCircle(mouse, currentPos_raylib, 50)) {
                generalSelected = !generalSelected;
                if (generalSelected) {
                    Math::vec2 footPos_math = movement->GetFootPosition();
                    Vector2 footPos_raylib = { (float)footPos_math.x, (float)footPos_math.y };
                    HexTile* from = tutorialMap.GetTileAtPosition(footPos_raylib);
                    movableTiles = tutorialMap.GetMovableTiles(from);
                }
                else {
                    movableTiles.clear();
                }
            }
            else if (generalSelected) {
                for (auto& tile : movableTiles) {
                    if (CheckCollisionPointCircle(mouse, tile.center, radiusX * 0.8f)) {
                        movement->MoveTo({ (double)tile.center.x, (double)tile.center.y });
                        generalSelected = false;
                        movableTiles.clear();
                        break;
                    }
                }
            }
        }
    }

    if (tutorialDone) {
        Engine::Instance().GetGameStateManager().SetNextGameState(STATE_MAIN_MAP);
        return;
    }

    GetGSComponent<CS230::GameObjectManager>()->UpdateAll(dt);
}

void Mode0::Draw() {
    ClearBackground(BLACK);
    Math::TransformationMatrix camera_matrix;

    // 1. 튜토리얼 장군의 현재 타일 위치 찾기
    HexTile* vision_center_tile = nullptr;
    if (tutorialGeneral != nullptr) {
        LinearMovement* movement = tutorialGeneral->GetGOComponent<LinearMovement>();
        vision_center_tile = tutorialMap.GetTileAtPosition(movement->GetFootPosition());
    }

    // 2. 맵 그리기 (시야의 중심 타일을 전달하여 Fog of War 효과 적용)
    tutorialMap.Draw(vision_center_tile, camera_matrix);

    // 3. 이동 가능 범위 그리기
    if (generalSelected) {
        for (const auto& tile : movableTiles) {
            DrawCircleV(tile.center, 30, Fade(BLUE, 0.4f));
        }
    }

    // 4. 모든 GameObject 그리기 (장군들)
    GetGSComponent<CS230::GameObjectManager>()->DrawAll(camera_matrix);

    // 5. 다이얼로그 UI 그리기
    if (dialogueShown) {
        float width = chatWindowTexture.width;
        float height = chatWindowTexture.height;
        Vector2 pos = { GetScreenWidth() / 2.0f - width / 2.0f, GetScreenHeight() - height - 30.0f };
        DrawTexture(chatWindowTexture, (int)pos.x, (int)pos.y, Fade(WHITE, chatAlpha));

        if (chatAlpha >= 1.0f) {
            Font font = GetFontDefault();
            Vector2 textPos = { 120, (float)GetScreenHeight() - 165 };
            DrawTextEx(font, currentDialogue.c_str(), textPos, 30, 2.0f, BLACK);
        }
    }
}

void Mode0::Unload() {
    Engine::GetLogger().LogEvent(GetName() + " Unload");
    UnloadTexture(chatWindowTexture);
    // AddGSComponent로 추가된 GameObjectManager는 자동으로 해제됩니다.
    // GOM에 추가된 tutorialGeneral, staticGeneral도 자동으로 해제됩니다.
    tutorialGeneral = nullptr;
    staticGeneral = nullptr;
}