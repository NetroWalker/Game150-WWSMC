// main.cpp
#include "raylib.h"
#include "Engine/Engine.h"
#include "Game/Splash.h"
#include "Game/Menu.h"
#include "Game/Mode0.h"
#include "Game/MainMapState.h"
#include "Game/BattleMap.h"   
#include "Engine/Window.h"

int main() {
    Engine& engine = Engine::Instance();
    engine.Start("A COOOOOOORN? REALLY?"); 
    Menu* menuState = new Menu(CS230::Window::default_width, CS230::Window::default_height);
    Mode0* tutorialState = new Mode0({ CS230::Window::default_width / 2.0f, CS230::Window::default_height / 2.0f }, 200.0f, 200.0f);
    MainMapState* mainMapState = new MainMapState(CS230::Window::default_width, CS230::Window::default_height);
    BattleMap* battleMapState = new BattleMap(CS230::Window::default_width, CS230::Window::default_height);
    Splash splash;
    engine.GetGameStateManager().AddGameState(splash);
    engine.GetGameStateManager().AddGameState(*menuState);      
    engine.GetGameStateManager().AddGameState(*tutorialState);  
    engine.GetGameStateManager().AddGameState(*mainMapState);   
    engine.GetGameStateManager().AddGameState(*battleMapState); 

    engine.AddFont("Assets/Font_Simple.png");
    engine.AddFont("Assets/Font_Outlined.png");

    // 1. GameState 객체 생성 (힙에 할당하여 GameStateManager가 포인터를 안전하게 관리하도록 함)
    Menu* menuState = new Menu(screenWidth, screenHeight);
    Mode0* tutorialState = new Mode0({ screenWidth / 2.0f, screenHeight / 2.0f }, 200.0f, 200.0f); // Mode0 생성자 사용
    MainMapState* mainMapState = new MainMapState(screenWidth, screenHeight); // MainMapState 생성자 사용
    BattleMap battleMapState;     // BattleMap 생성자 사용 (GameState로 가정)

    // 2. GameStateManager에 GameState 추가
    // 여기서 추가하는 순서가 위의 *_STATE_IDX와 일치해야 합니다.
    engine.GetGameStateManager().AddGameState(*menuState);         // 인덱스 0
    engine.GetGameStateManager().AddGameState(*tutorialState);     // 인덱스 1
    engine.GetGameStateManager().AddGameState(*mainMapState);      // 인덱스 2
    engine.GetGameStateManager().AddGameState(battleMapState);     // 인덱스 3

    // 3. 초기 GameState 설정
    engine.GetGameStateManager().SetNextGameState(MENU_STATE_IDX); // 메뉴 화면으로 시작

    // 4. 메인 게임 루프 (단순화됨)
    // engine.Update()가 GameStateManager를 통해 현재 활성화된 GameState의 Update 및 Draw를 호출합니다.
    while (!engine.HasGameEnded()) {
        engine.Update();
    }

    engine.Stop();

    delete menuState;
    delete tutorialState;
    delete mainMapState;
    //delete battleMapState;

    return 0;
}