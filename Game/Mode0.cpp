//mode0.cpp
#include"../Engine/Engine.h"
#include "Mode0.h"
#include "States.h"
#include "SquirrelGen.h"
#include "../Engine/Camera.h"
#include "SnakeGen.h"
#include "LinearMovement.h"
#include <cmath>

Mode0::Mode0(Vector2 center, float rX, float rY)
    : tutorialMap(center, rX, rY, 3, 1, false),
    radiusX(rX)
{
}

Mode0::~Mode0() {
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
    AddGSComponent(new CS230::Camera({ {0,0}, {0,0} }));

    tutorialMap.SetPoint();
    const std::vector<HexTile>& tiles = tutorialMap.GetTiles();

    tutorialGeneral = new SquirrelGen({ (double)tiles[0].center.x, (double)tiles[0].center.y });
    staticGeneral = new SnakeGen({ (double)tiles[2].center.x, (double)tiles[2].center.y });
    tutorialGeneral->SetScale({ 0.3, 0.3 });
    staticGeneral->SetScale({ 0.3,0.3 });

    CS230::GameObjectManager* GOM = GetGSComponent<CS230::GameObjectManager>();
    GOM->Add(tutorialGeneral);
    GOM->Add(staticGeneral);

    chatWindowTexture = LoadTexture("Assets/chat_window.png");
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
    if (dialogueShown && waitingForSpace && IsKeyPressed(KEY_SPACE) && dialogueCharIndex == (int)fullDialogue.length()) {
        SetDialogueStep(dialogueStep + 1);
    }

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
    CS230::Camera* camera = GetGSComponent<CS230::Camera>();
    Math::TransformationMatrix camera_matrix;
    if (camera != nullptr) {
        camera_matrix = camera->GetMatrix();
    }

    std::set<HexTile*> visibleTiles;
    if (tutorialGeneral != nullptr) {
        HexTile* source_tile = tutorialMap.GetTileAtPosition(tutorialGeneral->GetGOComponent<LinearMovement>()->GetFootPosition());
        if (source_tile != nullptr) {
            visibleTiles.insert(source_tile);
            auto neighbors = tutorialMap.GetMovableTiles(source_tile);
            for (const auto& neighbor : neighbors) {
                visibleTiles.insert(tutorialMap.GetTileAt(neighbor.x, neighbor.y));
            }
        }
    }

    tutorialMap.Draw(visibleTiles, camera_matrix);

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
            Font font = GetFontDefault();
            Vector2 textPos = { pos.x + 100, pos.y + 40 };
            DrawTextEx(font, currentDialogue.c_str(), textPos, 30, 2.0f, BLACK);
        }
    }
}

void Mode0::Unload() {
    Engine::GetLogger().LogEvent(GetName() + " Unload");
    UnloadTexture(chatWindowTexture);
    tutorialGeneral = nullptr;
    staticGeneral = nullptr;
}