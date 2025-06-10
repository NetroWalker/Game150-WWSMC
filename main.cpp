// main.cpp
#include "raylib.h"

// CS230 Engine Core
#include "Engine/Engine.h"

// Game State Classes (이제 모두 CS230::GameState를 상속한다고 가정)
#include "Game/Menu.h"
#include "Game/Mode0.h"
#include "Game/MainMapState.h" // 새로 만들 MainMapState.h (Map, General, TurnManager 포함)
#include "Game/BattleMap.h"   // BattleMap.h (CS230::GameState 상속)

// Game-specific enums/constants (필요하다면)
// #include "Game/States.h" // 기존 GameState enum은 상태 클래스 내부에서 다음 상태 인덱스 결정용으로 사용 가능

#define screenWidth 1500
#define screenHeight 1000

// GameStateManager에 추가되는 순서에 따른 상태 인덱스 정의
// 이 값들은 각 GameState 클래스 내부에서 다음 상태로 전환할 때 사용됩니다.
extern const int MENU_STATE_IDX;
extern const int TUTORIAL_STATE_IDX;
extern const int MAIN_MAP_STATE_IDX;
extern const int BATTLE_MAP_STATE_IDX;

int main() {
    Engine& engine = Engine::Instance();
    engine.Start("A COOOOOOORN? REALLY?"); // 창 제목 설정

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

    // 5. 게임 종료 및 리소스 해제
    engine.Stop();

    // 힙에 할당된 GameState 객체들 해제
    delete menuState;
    delete tutorialState;
    delete mainMapState;
    //delete battleMapState;

    // Raylib 창 닫기는 engine.Stop() 또는 Window 소멸자에서 처리될 것으로 예상됩니다.
    // 만약 Raylib의 CloseWindow()를 직접 호출해야 한다면 engine.Stop() 이후에 호출합니다.

    return 0;
}