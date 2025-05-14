#pragma once
#include "raylib.h"
#include "map.h"
#include "General.h"
#include <vector>

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

    // 설명창 관련
    float tutorialTimer = 0.0f;
    float chatAlpha = 0.0f;               // ✅ fade-in 투명도
    Texture2D chatWindowTexture;
};
