// Game/main.cpp

#include "raylib.h"
#include "Engine/Engine.h"
#include "Game/Splash.h"
#include "Game/Menu.h"
#include "Game/States.h"
#include "Game/Mode0.h"
#include "Game/MainMapState.h"
#include "Game/BattleMap.h"
#include "Engine/Window.h"
#include "Game/GameSession.h"

int main() {
    Engine& engine = Engine::Instance();
    engine.Start("A COOOOOOORN? REALLY?");

    // 1. 모든 상태 객체 생성
    Splash* splashState = new Splash();
    Menu* menuState = new Menu(CS230::Window::default_width, CS230::Window::default_height);
    Mode0* tutorialState = new Mode0({ (double)CS230::Window::default_width / 2.0, (double)CS230::Window::default_height / 2.0 }, 200.0f, 200.0f);
    MainMapState* mainMapState = new MainMapState(CS230::Window::default_width, CS230::Window::default_height);
    BattleMap* battleMapState = new BattleMap();

    engine.GetGameStateManager().AddGameState(*splashState);      // 인덱스 0
    engine.GetGameStateManager().AddGameState(*menuState);         // 인덱스 1
    engine.GetGameStateManager().AddGameState(*tutorialState);     // 인덱스 2
    engine.GetGameStateManager().AddGameState(*mainMapState);      // 인덱스 3
    engine.GetGameStateManager().AddGameState(*battleMapState);    // 인덱스 4

    // 폰트 추가
    engine.AddFont("Assets/Font_Simple.png");
    engine.AddFont("Assets/Font_Outlined.png");

    // 초기 상태를 Splash로 명시적으로 설정 (이 코드는 좋은 습관입니다)
    engine.GetGameStateManager().SetNextGameState(SPLASH);
    GameSession::GetInstance().Load();
    // 메인 루프
    while (!engine.HasGameEnded()) {
        engine.Update();
    }
    GameSession::GetInstance().Unload();
    // 게임 종료 및 메모리 해제
    engine.Stop();
    delete splashState;
    delete menuState;
    delete tutorialState;
    delete mainMapState;
    delete battleMapState;

    return 0;
}