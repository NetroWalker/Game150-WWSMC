/*
Copyright (C) 2023 DigiPen Institute of Technology
Reproduction or distribution of this file or its contents without
prior written consent is prohibited
File Name:  Animation.h
Project:    CS230 Engine
Author:     Jonathan Holmes, Junwoo Lee
Created:    April 20, 2025
*/

#pragma once
#include <vector>
#include<filesystem>
#include"Engine.h"

namespace CS230 {
    enum class CommandType {
        PlayFrame,
        Loop,
        End,
    };
    class Command {
    public:
        virtual ~Command() {}
        virtual CommandType Type() = 0;
    };


    class End : public Command {
    public:
        virtual CommandType Type() override { return CommandType::End; }
    private:
    };


    class Loop : public Command {
    public:
        Loop(int loop_index);
        virtual CommandType Type() override { return CommandType::Loop; }
        int LoopIndex();
    private:
        int loop_index;
    };

    class PlayFrame : public Command {
    public:
        PlayFrame(int frame, double duration);
        virtual CommandType Type() override { return CommandType::PlayFrame; }
        void Update(double dt);
        bool Ended();
        void ResetTime();
        int Frame();
    private:
        int frame;
        double target_time;
        double timer;
    };


    class Animation {
    public:
        Animation();
        Animation(const std::filesystem::path& animation_file);
        ~Animation();

        void Update(double dt);
        int CurrentFrame();
        void Reset();
        bool Ended();
    private:
        
        int current_command;
        std::vector<Command*> commands;
        bool ended;
        PlayFrame* current_frame;
    };

}
