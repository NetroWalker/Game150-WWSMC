//States.h
#pragma once

enum GameState {
    SPLASH,          //0
    STATE_MENU,      //1
    STATE_TUTORIAL,  //2
    STATE_MAIN_MAP,  //3
    STATE_BATTLE_MAP,//4
    STATE_ENDING     //5
};

enum class BattleOutcome {
    P1_WINS,
    P2_WINS,
    DRAW
};