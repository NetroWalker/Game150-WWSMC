#include "BattleMap.h"

BattleMap::BattleMap(int screenWidth, int screenHeight) {
    background = LoadTexture("Assets/Battlemap (2).png");
    placementTileTexture = LoadTexture("Assets/Placement_Tile.png");

    Texture2D meleeTex = LoadTexture("Assets/melee.png");
    Texture2D adcTex = LoadTexture("Assets/adc.png");
    Texture2D tankerTex = LoadTexture("Assets/tanker.png");

    float startX = 40.0f;
    float startY = 820.0f;

    unitIcons.push_back({ meleeTex,  {startX + 0 * 200, startY}, {startX + 0 * 200, startY} });
    unitIcons.push_back({ meleeTex,  {startX + 1 * 200, startY}, {startX + 1 * 200, startY} });
    unitIcons.push_back({ adcTex,    {startX + 2 * 200, startY}, {startX + 2 * 200, startY} });
    unitIcons.push_back({ tankerTex, {startX + 3 * 200, startY}, {startX + 3 * 200, startY} });
    unitIcons.push_back({ meleeTex,  {startX + 4 * 200, startY}, {startX + 4 * 200, startY} });

    Vector2 centers[5] = {
        {105, 583}, {290, 700}, {475, 583}, {660, 700}, {845, 583}
    };
    for (int i = 0; i < 5; i++) {
        slotPositions.push_back(centers[i]);
        slotOccupied.push_back(false);
    }

    startButton = { screenWidth - 160.0f, screenHeight - 100.0f, 120, 50 };
}

BattleMap::~BattleMap() {
    UnloadTexture(background);
    UnloadTexture(placementTileTexture);
    for (auto& icon : unitIcons) {
        UnloadTexture(icon.texture);
    }
}

void BattleMap::Update() {
    Vector2 mouse = GetMousePosition();
    float scale = 0.3f;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        for (int i = 0; i < unitIcons.size(); i++) {
            UnitIcon& icon = unitIcons[i];
            Rectangle bounds = {
                icon.position.x,
                icon.position.y,
                icon.texture.width * scale,
                icon.texture.height * scale
            };

            if (!icon.placed && CheckCollisionPointRec(mouse, bounds)) {
                icon.dragging = true;
                draggingSomething = true;
                draggingIndex = i;
                break;
            }
        }
    }

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON) && draggingSomething && draggingIndex != -1) {
        UnitIcon& icon = unitIcons[draggingIndex];
        icon.dragging = false;

        bool placed = false;
        for (int i = 0; i < slotPositions.size(); i++) {
            if (!slotOccupied[i] &&
                CheckCollisionPointCircle(mouse, slotPositions[i], placementTileTexture.width / 2.0f)) {
                icon.position = {
                    slotPositions[i].x - (225 * scale),
                    slotPositions[i].y - (650 * scale)
                };
                icon.placed = true;
                slotOccupied[i] = true;
                placed = true;
                break;
            }
        }

        if (!placed) {
            icon.position = icon.originalPosition;
        }

        draggingSomething = false;
        draggingIndex = -1;
    }

    if (draggingSomething && draggingIndex != -1) {
        unitIcons[draggingIndex].position = {
            mouse.x - (225 * scale),
            mouse.y - (650 * scale)
        };
    }
}

void BattleMap::Draw() {
    DrawTexture(background, 0, 0, WHITE);

    for (const auto& pos : slotPositions) {
        DrawTexture(placementTileTexture,
            (int)(pos.x - placementTileTexture.width / 2),
            (int)(pos.y - placementTileTexture.height / 2),
            WHITE);
    }

    float scale = 0.3f;
    for (const auto& icon : unitIcons) {
        DrawTextureEx(icon.texture, icon.position, 0.0f, scale, WHITE);
    }

    DrawRectangleRec(startButton, LIGHTGRAY);
    DrawText("START", (int)(startButton.x + 15), (int)(startButton.y + 15), 20, BLACK);
}
