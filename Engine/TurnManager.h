
#pragma once

enum class Turn {
    P1,
    P2
};

class TurnManager {
public:
    TurnManager();
    void EndTurn();
    void StartTurn();
    Turn GetCurrentTurn() const;
    bool CanMove() const;
    void Move();
    bool IsTransitioning() const;
    void ShowTransition();
    void HideTransition();

private:
    Turn currentTurn;
    int move_count;
    bool intrasition;
};