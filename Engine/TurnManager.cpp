// TurnManger.cpp

#include "TurnManager.h"

TurnManager::TurnManager()
    : currentTurn(Turn::P1), move_count(1), intrasition(false) {
}

// �� ���� ��ư �Է� ��
void TurnManager::EndTurn() {
    intrasition = true;
}

// �� ��ȯ ȭ�鿡�� START ��ư �Է� ��
void TurnManager::StartTurn() {
    currentTurn = (currentTurn == Turn::P1) ? Turn::P2 : Turn::P1;
    move_count = 1;
    intrasition = false;
}

// ���� ��
Turn TurnManager::GetCurrentTurn() const {
    return currentTurn;
}

// �̵� Ƚ��, �� ��ȯ �ƴ� �� �̵� ����
bool TurnManager::CanMove() const {
    return move_count > 0 && !intrasition;
}

// �̵��� �̵� Ƚ�� ����
void TurnManager::Move() {
    if (move_count > 0) {
        move_count--;
    }
}

// �� ��ȯ ȭ������ Ȯ��
bool TurnManager::IsTransitioning() const {
    return intrasition;
}

// �� ��ȯ ȭ�� on
void TurnManager::ShowTransition() {
    intrasition = true;
}

// �� ��ȯ ȭ�� off
void TurnManager::HideTransition() {
    intrasition = false;
}