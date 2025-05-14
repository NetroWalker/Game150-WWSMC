#include "BattleMap.h"

BattleMap::BattleMap(int screenWidth, int screenHeight) {
    background = LoadTexture("Assets/Battlemap (2).png");
    placementTileTexture = LoadTexture("Assets/Placement_Tile.png");
    waitingZoneTexture = LoadTexture("Assets/soldier_waiting_zone.png");

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
    UnloadTexture(waitingZoneTexture);
    for (auto& icon : unitIcons) UnloadTexture(icon.texture);
    for (auto& icon : enemyIcons) UnloadTexture(icon.texture);
}

void BattleMap::LoadSoldiersForTurn(Turn turn) {
    for (auto& icon : unitIcons) UnloadTexture(icon.texture);
    for (auto& icon : enemyIcons) UnloadTexture(icon.texture);
    unitIcons.clear();
    enemyIcons.clear();

    std::string suffix = (turn == Turn::P1) ? "" : "-s";
    std::string enemySuffix = (turn == Turn::P1) ? "-s" : "";

    Texture2D meleeTex = LoadTexture(("Assets/melee" + suffix + ".png").c_str());
    Texture2D adcTex = LoadTexture(("Assets/adc" + suffix + ".png").c_str());
    Texture2D tankerTex = LoadTexture(("Assets/tanker" + suffix + ".png").c_str());

    Texture2D emeleeTex = LoadTexture(("Assets/melee" + enemySuffix + ".png").c_str());
    Texture2D eadcTex = LoadTexture(("Assets/adc" + enemySuffix + ".png").c_str());
    Texture2D etankerTex = LoadTexture(("Assets/tanker" + enemySuffix + ".png").c_str());

    float scale = 0.3f;
    float startY = 820.0f;

    unitIcons.push_back({ meleeTex,  {  40 + 0 * 200, startY}, {  40 + 0 * 200, startY} });
    unitIcons.push_back({ meleeTex,  {  40 + 1 * 200, startY}, {  40 + 1 * 200, startY} });
    unitIcons.push_back({ adcTex,    {  40 + 2 * 200, startY}, {  40 + 2 * 200, startY} });
    unitIcons.push_back({ tankerTex, {  40 + 3 * 200, startY}, {  40 + 3 * 200, startY} });
    unitIcons.push_back({ meleeTex,  {  40 + 4 * 200, startY}, {  40 + 4 * 200, startY} });

    for (int i = 0; i < 5; i++) {
        float ex = slotPositions[i].x + 1800.0f;
        float ey = slotPositions[i].y;

        Texture2D tex;
        if (i == 0 || i == 3) tex = emeleeTex;
        else if (i == 1)      tex = eadcTex;
        else                  tex = etankerTex;

        enemyIcons.push_back({
            tex,
            { ex - (225 * scale), ey - (650 * scale) },
            { ex - (225 * scale), ey - (650 * scale) }
            });
    }
}

void BattleMap::Update() {
    Vector2 mouse = GetMousePosition();
    float scale = 0.3f;

    if (transitioning) {
        if (offsetX > -1500) offsetX -= 8.0f;
        return;
    }

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

        if (!placed) icon.position = icon.originalPosition;
        draggingSomething = false;
        draggingIndex = -1;
    }

    if (draggingSomething && draggingIndex != -1) {
        unitIcons[draggingIndex].position = {
            mouse.x - (225 * scale),
            mouse.y - (650 * scale)
        };
    }

    bool allPlaced = true;
    for (const auto& icon : unitIcons) {
        if (!icon.placed) {
            allPlaced = false;
            break;
        }
    }

    Rectangle clickableStart = {
        startButton.x + offsetX,
        startButton.y,
        startButton.width,
        startButton.height
    };

    if (allPlaced && CheckCollisionPointRec(mouse, clickableStart) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
        transitioning = true;
    }
}

void BattleMap::Draw() {
    float scale = 0.3f;

    DrawTexture(background, (int)offsetX, 0, WHITE);
    DrawTexture(waitingZoneTexture, (int)offsetX, 780, WHITE);

    for (const auto& pos : slotPositions) {
        DrawTexture(placementTileTexture,
            (int)(pos.x - placementTileTexture.width / 2 + offsetX),
            (int)(pos.y - placementTileTexture.height / 2),
            WHITE);
    }

    for (const auto& icon : unitIcons) {
        DrawTextureEx(icon.texture, icon.position, 0.0f, scale, WHITE);
    }

    for (const auto& icon : enemyIcons) {
        Rectangle src = { 0, 0, (float)icon.texture.width, (float)icon.texture.height };
        Rectangle dest = {
            icon.position.x + offsetX + (icon.texture.width * scale / 2),
            icon.position.y + (icon.texture.height * scale / 2),
            icon.texture.width * scale,
            icon.texture.height * scale
        };
        Vector2 origin = { dest.width / 2, dest.height / 2 };
        src.width = -src.width;  // 좌우 반전
        DrawTexturePro(icon.texture, src, dest, origin, 0.0f, WHITE);
    }

    Rectangle drawStart = { startButton.x + offsetX, startButton.y, startButton.width, startButton.height };
    DrawRectangleRec(drawStart, LIGHTGRAY);
    DrawText("START", (int)(drawStart.x + 15), (int)(drawStart.y + 15), 20, BLACK);
}
