// Game/MainMapState.cpp
#include "MainMapState.h"
#include"BattleMap.h"
#include <cmath> // For sqrtf

MainMapState::MainMapState(int sw, int sh) :
    screenWidth(sw), screenHeight(sh),
    radiusX(200.0f), radiusY(200.0f), // Example values, pass or configure as needed
    // Calculate Fcenter for the map
    gameMap(Vector2{ screenWidth / 2.0f - ((5 - 1) * 200.0f * 1.5f) / 2.0f,
                     screenHeight / 2.0f - ((5 - 1) * 200.0f * sqrtf(3.0f) * 0.5f) / 2.0f },
        200.0f, 200.0f, 5, 5, true),
    generalSelected(false) {
}

MainMapState::~MainMapState() {
    for (General* p : players) {
        delete p;
    }
    players.clear();
}

void MainMapState::Load() {
    Engine::GetLogger().LogEvent("Loading MainMapState");
    gameMap.SetPoint(); // Initialize map tiles

    HexTile* tile33 = gameMap.GetTileAt(3, 3);
    if (!tile33) {
        Engine::GetLogger().LogError("MainMapState: tile33 not found during Load.");
        // Potentially set a flag to transition to an error state or menu
        return;
    }

    players.push_back(new General(gameMap.GetTiles()[0].center, "Assets/General.png"));
    players.push_back(new General(tile33->center, "Assets/General1.png"));
    // turnManager is already default constructed
}

void MainMapState::Update([[maybe_unused]]double dt) {
    // This logic is moved from main.cpp's STATE_MAIN_MAP block
    Vector2 mouse = GetMousePosition();
    Turn turn = turnManager.GetCurrentTurn();

    if (!turnManager.IsTransitioning()) {
        int playerIndex = (turn == Turn::P1) ? 0 : 1;
        General* currentGeneral = players[playerIndex];
        int enemyIndex = (turn == Turn::P1) ? 1 : 0;
        General* enemyGeneral = players[enemyIndex];

        gameMap.Update(); // Map's own update (e.g., for dragging, zooming if re-enabled)

        // Handle selecting and moving general
        if (turnManager.CanMove()) {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !currentGeneral->IsMoving()) {
                if (CheckCollisionPointCircle(mouse, currentGeneral->GetPosition(), 50)) {
                    generalSelected = !generalSelected;
                    if (generalSelected) {
                        HexTile* from = gameMap.GetTileAtPosition(currentGeneral->GetFootPosition());
                        movableTiles = gameMap.GetMovableTiles(gameMap, from); // Pass gameMap by ref
                    }
                    else {
                        movableTiles.clear();
                    }
                }
                else if (generalSelected) {
                    for (auto& tile : movableTiles) {
                        if (CheckCollisionPointCircle(mouse, tile.center, radiusX * 0.8f)) {
                            currentGeneral->SetPosition(tile.center);
                            generalSelected = false;
                            movableTiles.clear();
                            turnManager.Move();
                            break;
                        }
                    }
                }
            }
        }
        currentGeneral->Update(); // Update general's movement animation

        // Battle transition logic
        if (!players[0]->IsMoving() && !players[1]->IsMoving()) {
            HexTile* p1Tile = gameMap.GetTileAtPosition(players[0]->GetFootPosition());
            HexTile* p2Tile = gameMap.GetTileAtPosition(players[1]->GetFootPosition());

            if (p1Tile && p2Tile && p1Tile->x == p2Tile->x && p1Tile->y == p2Tile->y) { //Simplified check: are they on the same tile?
                Engine::GetLogger().LogEvent("Transitioning to Battle Map");
                // Access BattleMap state directly to load soldiers - this is a bit of a hack.
                // A better way would be an event system or passing data through a shared context.
                // For now, assuming direct access or a way to pass the current turn.
                dynamic_cast<BattleMap*>(Engine::Instance().GetGameStateManager().GetGameState(BATTLE_MAP_STATE_INDEX))->LoadSoldiersForTurn(turnManager.GetCurrentTurn());
                Engine::Instance().GetGameStateManager().SetNextGameState(BATTLE_MAP_STATE_INDEX);
                return; // Return to avoid further processing this frame
            }
        }

        // End turn button
        Rectangle endTurnButton = { 1260, 750, 110, 40 };
        if (CheckCollisionPointRec(mouse, endTurnButton) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            turnManager.EndTurn();
            generalSelected = false; // Deselect general on turn end
            movableTiles.clear();
        }

    }
    else { // Turn is transitioning
        Rectangle startButton = { screenWidth / 2.0f - 100, screenHeight / 2.0f + 50, 200, 60 };
        bool hovering = CheckCollisionPointRec(mouse, startButton);
        if (hovering && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
            turnManager.StartTurn();
        }
    }
}

void MainMapState::Draw() {
    ClearBackground(RAYWHITE); // Or your desired background color
    Turn turn = turnManager.GetCurrentTurn();

    if (!turnManager.IsTransitioning()) {
        int playerIndex = (turn == Turn::P1) ? 0 : 1;
        General* currentGeneral = players[playerIndex];
        int enemyIndex = (turn == Turn::P1) ? 1 : 0;
        General* enemyGeneral = players[enemyIndex];

        gameMap.Draw(currentGeneral); // Draw map, highlighting current general's area

        // Draw movable tiles if general is selected
        for (const auto& tile : movableTiles) {
            DrawCircleV(tile.center, 30, Fade(BLUE, 0.4f));
        }

        currentGeneral->Draw(); // Draw current player

        // Draw enemy if they are neighbors (or visible)
        HexTile* cTile = gameMap.GetTileAtPosition(currentGeneral->GetFootPosition());
        //HexT<ctrl63>
    }
}

void MainMapState::Unload()
{
    Engine::GetLogger().LogEvent("Unloading BattleMap");
}
