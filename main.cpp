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

    while (!engine.HasGameEnded()) {
        engine.Update();
    }

    engine.Stop();

    delete menuState;
    delete tutorialState;
    delete mainMapState;
    delete battleMapState;

    return 0;
}