/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Engine.cpp
Project:    CS230 Engine
Author:     Jonathan Holmes, Junwoo Lee
Created:    March 8, 2023
Updated:    March 23, 2025
*/

#include "Engine.h"

Engine::Engine() : last_tick(std::chrono::system_clock::now()), last_test(last_tick),
#ifdef _DEBUG
    logger(CS230::Logger::Severity::Debug, true, last_tick)
#else
    logger(CS230::Logger::Severity::Event, false, last_tick)
#endif
{
}

void Engine::AddFont(const std::filesystem::path& file_name)
{
    fonts.emplace_back(file_name);
}

void Engine::Start(std::string window_title) {
    logger.LogEvent("Engine Started");
    window.Start(window_title);
    //Start other services
    last_tick = std::chrono::system_clock::now();
    last_test = last_tick;
    unsigned int seed = static_cast<unsigned int>(time(NULL));
    srand(seed);
    GetLogger().LogEvent("Seed generated" + std::to_string(seed));
}


void Engine::Stop() {
    //Stop all services
    texturemanager.Unload();
    logger.LogEvent("Engine Stopped");
}

void Engine::Update() {
    std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
    double dt = std::chrono::duration<double>(now - last_tick).count();

    if (dt >= 1.0 / TargetFPS) {
        logger.LogVerbose("Engine Update");
        last_tick = now;

        frame_count++;
        if (frame_count >= FPSTargetFrames) {
            std::chrono::duration<double> actual_duration = now - last_test;
            double actual_time = actual_duration.count();
            if (actual_time > 0)
            {
                logger.LogDebug("FPS: " + std::to_string(frame_count / actual_time));
            }
            frame_count = 0;
            last_test = now;
        }

        gamestatemanager.Update(dt);
        input.Update();
        window.Update();
    }
    //Update other services
}




bool Engine::HasGameEnded()
{
    return ((gamestatemanager.HasGameEnded() == true) || (window.IsClosed() == true)) ? true : false;
}
