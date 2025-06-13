#pragma once
#include "raylib.h"

class AudioManager {
public:
    static void Initialize();
    static void Update();
    static void Unload();
    static void PlayCastle();
    static void PlayPassingTurn();
    static void PlaySquirrelGen();
    static void PlaySnakeGen();
    static void PlayStartBattle();

private:
    static Music backgroundMusic;
    static bool   isEffectPlaying;
    static Sound  currentEffect;

    static Sound  castleSound;
    static Sound  passingTurnSound;
    static Sound  squirrelSound;
    static Sound  snakeSound;
    static Sound  battleStartSound;

    static void PlayEffect(Sound& effect);
};
