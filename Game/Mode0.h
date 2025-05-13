
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
    bool generalSelected = false;
    std::vector<HexTile> movableTiles;
    float radiusX;
    bool tutorialDone = false;

    std::vector<HexTile> GenerateTutorialTiles(Vector2 center, float radiusX, float radiusY);
};
