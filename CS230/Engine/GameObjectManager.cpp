/*
Copyright(C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name : GameObjectManager.cpp
Project : CS230 Engine
Author : Jonathan Holmes, Junwoo Lee
Created : March 8, 2025
Updated : April 20, 2025
*/
#include "GameObjectManager.h"

void CS230::GameObjectManager::Add(GameObject* object)
{
		objects.push_back(object);
}

void CS230::GameObjectManager::Unload()
{
	for (auto* objecto : objects) {
		delete objecto;
	}
	objects.clear();
}

void CS230::GameObjectManager::UpdateAll(double dt)
{
	for (auto* objecto : objects) {
		objecto->Update(dt);
	}
}

void CS230::GameObjectManager::DrawAll(Math::TransformationMatrix camera_matrix)
{
	for (auto* objecto : objects) {
		objecto->Draw(camera_matrix);
	}
}
