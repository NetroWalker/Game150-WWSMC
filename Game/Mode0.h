// Game/mode0.h
#pragma once
#include "raylib.h"
#include "map.h"
#include "../Engine/GameState.h"
#include <vector>
#include <string>

// 클래스 전방 선언: 헤더 파일 간의 불필요한 포함 관계를 줄입니다.
class SquirrelGen;
class SnakeGen;
struct HexTile;

class Mode0 : public CS230::GameState {
public:
    Mode0(Vector2 center, float radiusX, float radiusY);
    ~Mode0(); // 가상 소멸자 권장

    void Load() override;
    void Update(double dt) override;
    void Draw() override;
    void Unload() override;
    std::string GetName() override { return "Mode0"; }

private:
    void SetDialogueStep(int step);

    Map tutorialMap; // 포인터 대신 객체로 변경
    SquirrelGen* tutorialGeneral = nullptr; // 로딩 전까지 nullptr로 초기화
    SnakeGen* staticGeneral = nullptr;   // 로딩 전까지 nullptr로 초기화

    // 나머지 멤버 변수들은 대부분 그대로 사용합니다.
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