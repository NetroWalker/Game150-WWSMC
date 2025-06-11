#pragma once
#include "raylib.h"
#include "map.h"
#include "../Engine/GameState.h"
#include <vector>
#include <string>
#include <set>

// 전방 선언
class SquirrelGen;
class SnakeGen;
struct HexTile;

class Mode0 : public CS230::GameState {
public:
    Mode0(Vector2 center, float radiusX, float radiusY);
    ~Mode0() override;

    void Load() override;
    void Update(double dt) override;
    void Draw() override;
    void Unload() override;
    std::string GetName() override { return "Mode0"; }

private:
    Font dialogueFont;
    void SetDialogueStep(int step);

    Map tutorialMap;
    SquirrelGen* tutorialGeneral = nullptr;
    SnakeGen* staticGeneral = nullptr;

    bool generalSelected = false;
    std::vector<HexTile> movableTiles;
    float radiusX;
    bool tutorialDone = false;

    float tutorialTimer = 0.0f;
    float chatAlpha = 0.0f;
    bool dialogueShown = false;
    Texture2D chatWindowTexture;

    std::string fullDialogue = "";
    std::string currentDialogue = "";
    float dialogueCharTimer = 0.0f;
    int dialogueCharIndex = 0;

    int dialogueStep = -1;
    bool waitingForSpace = false;
    float delayTimer = 0.0f;
    bool canMove = false;
};