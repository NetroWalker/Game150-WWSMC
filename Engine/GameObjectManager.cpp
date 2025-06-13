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
#include "Engine.h"
#include <algorithm> // For std::find
#include <vector>    // For std::vector
#include <utility>   // For std::pair

void CS230::GameObjectManager::Add(GameObject* object)
{
    objects.push_back(object);
}

void CS230::GameObjectManager::Remove(GameObject* object_to_remove)
{
    auto it = std::find(objects.begin(), objects.end(), object_to_remove);

    if (it != objects.end())
    {
        delete* it;
        objects.erase(it);
    }
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
    // A backwards loop is safer in case an object's update function removes itself.
    for (size_t i = objects.size(); i > 0; --i) {
        objects[i - 1]->Update(dt);
    }
}

void CS230::GameObjectManager::DrawAll(Math::TransformationMatrix camera_matrix)
{
    for (auto* objecto : objects) {
        objecto->Draw(camera_matrix);
    }
}

void CS230::GameObjectManager::Update(double dt) {
    UpdateAll(dt);
    CollisionTest();
}

// =================================================================================================
// NEW & ROBUST: CollisionTest()
// This function now uses a two-phase system to prevent crashes.
// Phase 1 detects all collisions without changing the object list.
// Phase 2 resolves the detected collisions safely.
// This is the standard, correct way to handle collision resolution.
// =================================================================================================
void CS230::GameObjectManager::CollisionTest() {

    // A temporary list to store pairs of objects that are colliding.
    std::vector<std::pair<GameObject*, GameObject*>> collision_pairs;

    // --- PHASE 1: DETECT COLLISIONS ---
    // Use simple forward loops to find all colliding pairs.
    // This is safe because we are NOT modifying the 'objects' list here.
    for (size_t i = 0; i < objects.size(); ++i) {
        for (size_t j = i + 1; j < objects.size(); ++j) {
            GameObject* object_1 = objects[i];
            GameObject* object_2 = objects[j];

            // Check if either object can collide with the other and if they are intersecting.
            if ((object_1->CanCollideWith(object_2->Type()) && object_1->IsCollidingWith(object_2)) ||
                (object_2->CanCollideWith(object_1->Type()) && object_2->IsCollidingWith(object_1))) {

                // Store the colliding pair to be resolved later.
                collision_pairs.push_back({ object_1, object_2 });
            }
        }
    }

    // --- PHASE 2: RESOLVE COLLISIONS ---
    // Now, iterate through the pairs we found and resolve them.
    for (const auto& pair : collision_pairs) {
        GameObject* object_1 = pair.first;
        GameObject* object_2 = pair.second;

        // IMPORTANT: Before resolving, we must check if both objects still exist.
        // A previous collision resolution in this loop might have removed one of them.
        bool obj1_exists = (std::find(objects.begin(), objects.end(), object_1) != objects.end());
        bool obj2_exists = (std::find(objects.begin(), objects.end(), object_2) != objects.end());

        if (obj1_exists && obj2_exists) {
            Engine::GetLogger().LogEvent("Resolving Collision: " + object_1->TypeName() + " and " + object_2->TypeName());
            object_1->ResolveCollision(object_2);
            object_2->ResolveCollision(object_1);
        }
    }
}