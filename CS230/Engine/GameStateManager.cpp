/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  GameStateManager.cpp
Project:    CS230 Engine
Author:     Jonathan Holmes, Junwoo Lee
Created:    March 8, 2023
Updated:    March 23, 2025
*/
#include "GameStateManager.h"
#include "Engine.h"

namespace CS230 {
	GameStateManager::GameStateManager() :
		current_gamestate(nullptr),
		next_gamestate(nullptr),
		status(Status::STARTING)
	{
	}

	void GameStateManager::AddGameState(GameState& gamestate) {
		gamestates.push_back(&gamestate);
	}

	void GameStateManager::SetNextGameState(int index) {
		next_gamestate = gamestates[index];

	}

	void GameStateManager::ReloadState() {
		status = Status::UNLOADING;
	}

	void GameStateManager::ClearNextGameState() {
		status = Status::UNLOADING;
		next_gamestate = nullptr;
	}

	bool GameStateManager::HasGameEnded() {
		return status == Status::EXIT;
	}

	void GameStateManager::Update(double dt) {


		switch (status) {
		case Status::STARTING:

			if (gamestates.empty() == false) {
				SetNextGameState(0);
				status = Status::LOADING;
			}
			else {
				status = Status::STOPPING;
			}
			break;
		case Status::LOADING:
			current_gamestate = next_gamestate;
			Engine::GetLogger().LogEvent("Load " + current_gamestate->GetName());
			current_gamestate->Load();
			Engine::GetLogger().LogEvent("Load Complete");
			status = Status::UPDATING;
			break;
		case Status::UPDATING:
			if (current_gamestate != next_gamestate) {
				Engine::GetLogger().LogVerbose("Update " + current_gamestate->GetName());
				status = Status::UNLOADING;
			}
			else {
				status = Status::UPDATING;
			}
			current_gamestate->Update(dt);
			current_gamestate->Draw();

			break;
		case Status::UNLOADING:
			Engine::GetLogger().LogEvent("Unload " + current_gamestate->GetName());
			current_gamestate->Unload();
			Engine::GetTextureManager().Unload();
			Engine::GetLogger().LogEvent("Unload Complete");
			if (next_gamestate != nullptr)
				status = Status::LOADING;
			else
				status = Status::STOPPING;
			break;

		case Status::STOPPING:
			status = Status::EXIT;
			break;
		case Status::EXIT:

			break;
		}
	}
}