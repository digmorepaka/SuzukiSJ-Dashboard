#pragma once

#include <Arduino.h>

class Screen
{
public:
    virtual void OnEnter();
    virtual void Tick();
    virtual void Render();
    virtual void OnExit();

private:
    bool needToRender = true;
};