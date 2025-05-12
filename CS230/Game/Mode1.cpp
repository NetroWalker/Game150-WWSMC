#include "States.h"
#include "Mode1.h"
#include "Asteroid.h"
#include "Crates.h"
#include "Robot.h"
#include "Fonts.h"
#include "../Engine/Vec2.h"

Mode1::Mode1() :
    camera({ { 0.15 * Engine::GetWindow().GetSize().x, 0 },
             { 0.35 * Engine::GetWindow().GetSize().x, 0 } }),
    hexMap(nullptr) {
}

void Mode1::Load()
{
    Vector2 mapCenter = { Engine::GetWindow().GetSize().x / 2.0f,
                          Engine::GetWindow().GetSize().y / 2.0f };
    hexMap = new Map(mapCenter, 200.0f, 200.0f); 

    camera.SetPosition({ 0, 0 });
}

void Mode1::Update(double dt)
{
    hexMap->Update(); 
    gameobjectmanager.UpdateAll(dt);

    
    camera.Update(mapCenter);


    if (Engine::GetInput().KeyJustReleased(CS230::Input::Keys::Escape)) {
        Engine::GetGameStateManager().SetNextGameState(static_cast<int>(States::MainMenu));
    }
}

void Mode1::Draw()
{
    Engine::GetWindow().Clear(0x000000FF);
    hexMap->Draw(); 

    Math::TransformationMatrix camera_matrix = camera.GetMatrix();
    gameobjectmanager.DrawAll(camera_matrix);
}
//error test1
void Mode1::Unload()
{
    delete hexMap;
    hexMap = nullptr;

    gameobjectmanager.Unload();
}
