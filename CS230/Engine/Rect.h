/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Rect.h
Project:    CS230 Engine
Author:     Junwoo Lee
Created:    May 2, 2025
*/

#pragma once
#include "Vec2.h"

namespace Math {
    struct [[nodiscard]] rect {
        Math::vec2 bottom_left{ 0.0, 0.0 };
        Math::vec2 top_right{ 0.0, 0.0 };

        Math::vec2 Size() const noexcept {
            return {
                top_right.x - bottom_left.x,
                std::abs(top_right.y - bottom_left.y)
            };
        }
    };
    struct [[nodiscard]] irect {
        Math::ivec2 bottom_left{ 0, 0 };
        Math::ivec2 top_right{ 0, 0 };

        Math::ivec2 Size() const noexcept {
            return {
                top_right.x - bottom_left.x,
                std::abs(top_right.y - bottom_left.y)
            };
        }
    };
}
