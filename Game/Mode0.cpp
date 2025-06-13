// Game/Mode0.cpp
#include "Mode0.h"
#include "../Engine/Engine.h"
#include "../Engine/GameObjectManager.h"
#include "../Engine/Camera.h"
#include "States.h"
#include "SquirrelGen.h"
#include "SnakeGen.h"
#include "LinearMovement.h"
#include <cmath>

Mode0::Mode0(Vector2 center, float rX, float rY)
    : tutorialMap(center, rX, rY, 3, 1, false), radiusX(rX) 
{
}

Mode0::~Mode0() {}

void Mode0::Load() {
    Engine::GetLogger().LogEvent(GetName() + " Load");
    AddGSComponent(new CS230::GameObjectManager());
    AddGSComponent(new CS230::Camera({ {0,0}, {0,0} }));

    tutorialMap.SetPoint();
    const std::vector<HexTile>& tiles = tutorialMap.GetTiles();

    tutorialGeneral = new SquirrelGen({ (double)tiles[0].center.x, (double)tiles[0].center.y });
    staticGeneral = new SnakeGen({ (double)tiles[2].center.x, (double)tiles[2].center.y });
    tutorialGeneral->SetScale({ 0.3, 0.3 });
    staticGeneral->SetScale({ 0.3, 0.3 });

    CS230::GameObjectManager* GOM = GetGSComponent<CS230::GameObjectManager>();
    GOM->Add(tutorialGeneral);
    GOM->Add(staticGeneral);

    chatWindowTexture = LoadTexture("Assets/chat_window.png");
    dialogueFont = LoadFont("Assets/Font_Simple.png"); // 폰트 로드

    tutorialTimer = 0.0f;
    chatAlpha = 0.0f;
    dialogueShown = false;
    tutorialDone = false;
    generalSelected = false;
    movableTiles.clear();
    SetDialogueStep(-1);
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
    if (dialogueShown && waitingForSpace && IsKeyPressed(KEY_SPACE) && dialogueCharIndex >= (int)fullDialogue.length()) {
        SetDialogueStep(dialogueStep + 1);
    }

    LinearMovement* movement = tutorialGeneral->GetGOComponent<LinearMovement>();

    HexTile* current_tile = tutorialMap.GetTileAtPosition(movement->GetFootPosition());
    if (dialogueStep == 2 && !movement->IsMoving() && current_tile == tutorialMap.GetTileAt(1, 0)) {
        SetDialogueStep(3);
    }
    if (dialogueStep == 3 && dialogueCharIndex >= (int)fullDialogue.length()) {
        delayTimer += dt;
        if (delayTimer >= 2.0f) SetDialogueStep(4);
    }
    if (dialogueStep == 6 && !movement->IsMoving() && current_tile == tutorialMap.GetTileAt(2, 0)) {
        SetDialogueStep(7);
    }

    if (dialogueShown && canMove && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !movement->IsMoving()) {
        Vector2 mouse = GetMousePosition();
        CS230::Camera* camera = GetGSComponent<CS230::Camera>();
        Math::TransformationMatrix camera_matrix;
        if (camera) camera_matrix = camera->GetMatrix();

        if (CheckCollisionPointCircle(mouse, tutorialGeneral->GetPosition(), 50)) {
            generalSelected = !generalSelected;
            if (generalSelected) {
                movableTiles = tutorialMap.GetMovableTiles(tutorialMap.GetTileAtPosition(movement->GetFootPosition()));
            }
            else {
                movableTiles.clear();
            }
        }
        else if (generalSelected) {
            for (auto& tile : movableTiles) {
                if (CheckCollisionPointCircle(mouse, camera_matrix * Math::vec2(tile.center), radiusX * 0.4f)) {
                    movement->MoveTo(tile.center);
                    generalSelected = false;
                    movableTiles.clear();
                    break;
                }
            }
        }
    }

    if (tutorialDone) {
        Engine::GetGameStateManager().SetNextGameState(STATE_MAIN_MAP);
        return;
    }

    GetGSComponent<CS230::GameObjectManager>()->UpdateAll(dt);
}

void Mode0::Draw() {
    ClearBackground(BLACK);
    CS230::Camera* camera = GetGSComponent<CS230::Camera>();
    Math::TransformationMatrix camera_matrix;
    if (camera != nullptr) camera_matrix = camera->GetMatrix();

    std::map<HexTile*, TileType> visionMap;
    for (const auto& tile : tutorialMap.GetTiles()) {
        visionMap[const_cast<HexTile*>(&tile)] = tile.type;
    }
    tutorialMap.Draw(visionMap, camera_matrix);

    if (generalSelected) {
        for (const auto& tile : movableTiles) {
            DrawCircleV(camera_matrix * Math::vec2(tile.center), 30, Fade(BLUE, 0.4f));
        }
    }

    GetGSComponent<CS230::GameObjectManager>()->DrawAll(camera_matrix);

    if (dialogueShown) {
        float width = chatWindowTexture.width;
        float height = chatWindowTexture.height;
        Vector2 pos = { GetScreenWidth() / 2.0f - width / 2.0f, GetScreenHeight() - height - 30.0f };
        DrawTexture(chatWindowTexture, (int)pos.x, (int)pos.y, Fade(WHITE, chatAlpha));

        if (chatAlpha >= 1.0f) {
            Vector2 textPos = { pos.x + 100, pos.y + 40 };
            DrawTextEx(dialogueFont, currentDialogue.c_str(), textPos, 30, 2.0f, BLACK);
        }
    }
}

void Mode0::Unload() {
    Engine::GetLogger().LogEvent(GetName() + " Unload");
    UnloadTexture(chatWindowTexture);
    UnloadFont(dialogueFont); // 폰트 해제
}

void Mode0::SetDialogueStep(int step) {
    dialogueStep = step;
    waitingForSpace = false;
    delayTimer = 0.0f;
    canMove = false;
    currentDialogue.clear();
    dialogueCharIndex = 0;
    dialogueCharTimer = 0.0f;

    switch (step) {
    case 0: fullDialogue = "Hello, you're the new general, aren't you?"; waitingForSpace = true; break;
    case 1: fullDialogue = "Your mission is to help our tribe thrive..."; waitingForSpace = true; break;
    case 2: fullDialogue = "You can move the general using the mouse!"; canMove = true; break;
    case 3: fullDialogue = "Good!!!"; break;
    case 4: fullDialogue = "Normally, you can only move once per turn."; waitingForSpace = true; break;
    case 5: fullDialogue = "But just this time, I'll let you move once more."; waitingForSpace = true; canMove = true; break;
    case 6: fullDialogue = "When your general encounters an enemy, a battle will begin!"; waitingForSpace = true; canMove = true; break;
    case 7: tutorialDone = true; break;
    default: fullDialogue = ""; break;
    }
}