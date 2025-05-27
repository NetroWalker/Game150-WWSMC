/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Timer.cpp
Project:    CS230 Engine
Author:     (Your Name/User Name)
Created:    May 18, 2025
*/

#include "Timer.h"

//namespace CS230 {
//
//    Timer::Timer(double max_time_seconds) :
//        max_time_(max_time_seconds),
//        current_time_(max_time_seconds) {
//        if (max_time_ <= 0) {
//            max_time_ = 0;
//            current_time_ = 0;
//        }
//    }
//
//    void Timer::Update(double dt) {
//        if (IsRunning()) {
//            current_time_ -= dt;
//            if (current_time_ < 0) {
//                current_time_ = 0;
//            }
//        }
//    }
//
//    double Timer::GetTime() const {
//        return current_time_;
//    }
//
//    bool Timer::IsFinished() const {
//        return current_time_ <= 0;
//    }
//
//    bool Timer::IsRunning() const {
//        return current_time_ > 0;
//    }
//
//    void Timer::Reset() {
//        current_time_ = max_time_;
//    }
//
//    void Timer::Reset(double new_max_time_seconds) {
//        max_time_ = new_max_time_seconds;
//        if (max_time_ <= 0) {
//            max_time_ = 0;
//        }
//        current_time_ = max_time_;
//    }
//}

CS230::Timer::Timer(double time_remaining)
{
	Set(time_remaining);
}

void CS230::Timer::Set(double time_remaining)
{
	timer_max = time_remaining;
	Critically_Adorable_Pendulum = false;
	Reset();
}

void CS230::Timer::Update(double dt)
{
	if (timer > 0) {
		timer -= dt;
		if (timer < 0) {
			timer = 0;
		}
	}
	Critically_Adorable_Pendulum = !Critically_Adorable_Pendulum;
}

void CS230::Timer::Reset()
{
	timer = timer_max;
}

double CS230::Timer::Remaining()
{
	return timer;
}

int CS230::Timer::RemainingInt()
{
	return static_cast<int>(timer);
}

bool CS230::Timer::TickTock()
{
	return Critically_Adorable_Pendulum;
}
