#pragma once

#include "Screen.h"

class MainScreen : public Screen
{
public:
    void OnEnter() override;
    void Tick() override;
    void Render() override;
    void OnExit() override;
private:
    bool fuelChanged = true;
    bool coolerChanged = true;
};