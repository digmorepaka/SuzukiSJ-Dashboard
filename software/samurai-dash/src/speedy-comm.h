#pragma once

#include <TFT_eSPI.h>
#include <SPI.h>

extern TFT_eSPI tft;
extern HardwareSerial Serial2;
extern bool dataUpdated;
extern int fuelError;
extern int oilError;

struct ImportantData
{
    uint16_t RPM;
    uint16_t MAP;
    uint16_t AFR;
    uint16_t TPS;
    float OILPRSR;
    int Voltage;
    int16_t IAT;
    int16_t CLT;
    int fuelLevel;

};

extern struct ImportantData importantData;

void fillRectInside(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color);