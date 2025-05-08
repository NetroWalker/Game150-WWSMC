/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Input.h
Project:    CS230 Engine
Author:     Jonathan Holmes, Junwoo Lee
Created:    March 8, 2023
Updated:    March 23, 2025
*/

#pragma once
#include <vector>

namespace CS230 {
    class Input {
    public:
        enum class Keys {
            A, B, C, D, E, F, G, H, I, J, K, L, M,
            N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
            Space, Enter, Left, Up, Right, Down,
            Escape, Count
        };

        Input();
        bool KeyDown(Keys key);
        bool KeyJustPressed(Keys key);
        bool KeyJustReleased(Keys key);
        void SetKeyDown(Keys key, bool value);
        void Update();

    private:
        std::vector<bool> keys_down;
        std::vector<bool> previous_keys_down;

    };
}