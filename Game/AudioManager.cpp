#include "AudioManager.h"

Music AudioManager::backgroundMusic;
bool  AudioManager::isEffectPlaying = false;
Sound AudioManager::currentEffect;
Sound AudioManager::castleSound;
Sound AudioManager::passingTurnSound;
Sound AudioManager::squirrelSound;
Sound AudioManager::snakeSound;
Sound AudioManager::battleStartSound;

void AudioManager::Initialize() {
    InitAudioDevice();
    backgroundMusic = LoadMusicStream("assets/audio/maintheme.mp3");
    castleSound = LoadSound("assets/audio/castle.mp3");
    passingTurnSound = LoadSound("assets/audio/PassingTurn.mp3");
    squirrelSound = LoadSound("assets/audio/SquralGen.mp3");
    snakeSound = LoadSound("assets/audio/SnakeGen.mp3");
    battleStartSound = LoadSound("assets/audio/StartBattle.mp3");
    PlayMusicStream(backgroundMusic);
}

void AudioManager::Update() {
    UpdateMusicStream(backgroundMusic);
    if (isEffectPlaying && !IsSoundPlaying(currentEffect)) {
        ResumeMusicStream(backgroundMusic);
        isEffectPlaying = false;
    }
}

void AudioManager::Unload() {
    StopMusicStream(backgroundMusic);
    UnloadMusicStream(backgroundMusic);
    UnloadSound(castleSound);
    UnloadSound(passingTurnSound);
    UnloadSound(squirrelSound);
    UnloadSound(snakeSound);
    UnloadSound(battleStartSound);
    CloseAudioDevice();
}

void AudioManager::PlayEffect(Sound& effect) {
    if (isEffectPlaying) return;
    PauseMusicStream(backgroundMusic);
    PlaySound(effect);
    currentEffect = effect;
    isEffectPlaying = true;
}

void AudioManager::PlayCastle() { PlayEffect(castleSound); }
void AudioManager::PlayPassingTurn() { PlayEffect(passingTurnSound); }
void AudioManager::PlaySquirrelGen() { PlayEffect(squirrelSound); }
void AudioManager::PlaySnakeGen() { PlayEffect(snakeSound); }
void AudioManager::PlayStartBattle() { PlayEffect(battleStartSound); }
