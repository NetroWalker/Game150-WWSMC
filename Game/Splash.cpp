//Splash.cpp
#include "../Engine/Engine.h"
#include "States.h"
#include "Splash.h"

Splash::Splash() {}

void Splash::Load() {
    counter = 0;
    texture = Engine::GetTextureManager().Load("Assets/DigiPen.png");
    transition_triggered = false;
}

void Splash::Draw() {
    Engine::GetWindow().Clear(UINT_MAX);
    texture->Draw(Math::TranslationMatrix({ (Engine::GetWindow().GetSize() - texture->GetSize()) / 2.0 }));

}

void Splash::Update([[maybe_unused]] double dt) {
    if (!transition_triggered) {
        Engine::GetLogger().LogDebug(std::to_string(counter));
        if ((counter >= 2)) {
            Engine::GetGameStateManager().ClearNextGameState();
            Engine::GetGameStateManager().SetNextGameState(static_cast<int>(STATE_MENU));
            Unload();
        }
        counter += dt;
    }
}

void Splash::Unload() {
}
