#pragma once 

#include "Screen.h"

class TuneScreen : public Screen
{
public:
    void OnEnter() override;
    void Tick() override;
    void Render() override;
    void OnExit() override;
};