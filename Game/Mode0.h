#pragma once
#include "raylib.h"
#include "map.h"
#include "General.h"
#include <vector>
#include <string>

class Mode0 {
public:
    Mode0(Vector2 center, float radiusX, float radiusY);
    ~Mode0();

    void Update();
    void Draw();
    bool IsTutorialDone() const;

private:
    Map tutorialMap;
    General* tutorialGeneral;
    General* staticGeneral;
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

    bool canMove = false;  // General 움직임 허용 여부

    void SetDialogueStep(int step);
};
