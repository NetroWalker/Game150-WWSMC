#include "TurnManager.h"

TurnManager::TurnManager()
    : currentTurn(Turn::P1), move_count(1), intrasition(false) {
}

void TurnManager::EndTurn() {
    currentTurn = (currentTurn == Turn::P1) ? Turn::P2 : Turn::P1;
    intrasition = true;
}

void TurnManager::StartTurn() {
    move_count = 1;
    intrasition = false;
}

Turn TurnManager::GetCurrentTurn() const {
    return currentTurn;
}

bool TurnManager::CanMove() const {
    return move_count > 0 && !intrasition;
}

void TurnManager::Move() {
    if (move_count > 0) {
        move_count--;
    }
}

bool TurnManager::IsTransitioning() const {
    return intrasition;
}