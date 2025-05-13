// TurnManger.cpp

#include "TurnManager.h"

TurnManager::TurnManager()
    : currentTurn(Turn::P1), move_count(1), intrasition(false) {
}

// 턴 종료 버튼 입력 시
void TurnManager::EndTurn() {
    intrasition = true;
}

// 턴 전환 화면에서 START 버튼 입력 시
void TurnManager::StartTurn() {
    currentTurn = (currentTurn == Turn::P1) ? Turn::P2 : Turn::P1;
    move_count = 1;
    intrasition = false;
}

// 현재 턴
Turn TurnManager::GetCurrentTurn() const {
    return currentTurn;
}

// 이동 횟수, 턴 전환 아닐 시 이동 가능
bool TurnManager::CanMove() const {
    return move_count > 0 && !intrasition;
}

// 이동시 이동 횟수 감소
void TurnManager::Move() {
    if (move_count > 0) {
        move_count--;
    }
}

// 턴 전환 화면인지 확인
bool TurnManager::IsTransitioning() const {
    return intrasition;
}

// 턴 전환 화면 on
void TurnManager::ShowTransition() {
    intrasition = true;
}

// 턴 전환 화면 off
void TurnManager::HideTransition() {
    intrasition = false;
}