#include "raylib.h"
#include "Engine/Engine.h"
#include "Game/Splash.h"
#include "Game/Menu.h"
#include "Game/States.h"
#include "Game/Mode0.h"
#include "Game/MainMapState.h"
#include "Game/BattleMap.h"
#include "Engine/Window.h"

int main() {
    Engine& engine = Engine::Instance();
    engine.Start("A COOOOOOORN? REALLY?");

    // 모든 GameState를 new를 사용해 힙(heap)에 생성하도록 통일
    Splash* splashState = new Splash(); // ===== 수정 1 =====
    Menu* menuState = new Menu(CS230::Window::default_width, CS230::Window::default_height);
    Mode0* tutorialState = new Mode0({ (double)CS230::Window::default_width / 2.0, (double)CS230::Window::default_height / 2.0 }, 200.0f, 200.0f);
    MainMapState* mainMapState = new MainMapState(CS230::Window::default_width, CS230::Window::default_height);
    BattleMap* battleMapState = new BattleMap();

    // GameStateManager에 상태 추가
    engine.GetGameStateManager().AddGameState(*splashState);
    engine.GetGameStateManager().AddGameState(*menuState);
    engine.GetGameStateManager().AddGameState(*tutorialState);
    engine.GetGameStateManager().AddGameState(*mainMapState);
    engine.GetGameStateManager().AddGameState(*battleMapState);

    // 폰트 추가 및 초기 상태 설정
    engine.AddFont("Assets/Font_Simple.png");
    engine.AddFont("Assets/Font_Outlined.png");
    engine.GetGameStateManager().SetNextGameState(SPLASH);

    // 메인 루프
    while (!engine.HasGameEnded()) {
        engine.Update();
    }

    // 게임 종료
    engine.Stop();
    delete splashState; // ===== 수정 1 =====
    delete menuState;
    delete tutorialState;
    delete mainMapState;
    delete battleMapState; // ===== 수정 2 =====

    return 0;
}