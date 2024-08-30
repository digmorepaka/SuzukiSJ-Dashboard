#include "MainScreen.h"
#include "speedy-comm.h"

void MainScreen::OnEnter()
{
    tft.setCursor(18, 178 + 5);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(3);
    tft.print("RPM:");
    tft.setCursor(18, 178 + 5 + 20 + 5);
    tft.print("OIL:");

    // Rectangle frame Fuel
    tft.drawRect(18, 18, 142, 160, TFT_WHITE);
    //  Rectangle  frame Temperature
    tft.drawRect(178, 18, 44, 160, TFT_WHITE);

    // Lines Fuel
    tft.drawFastHLine(18, 44, 142, TFT_WHITE);
    tft.drawFastHLine(18, 112, 142, TFT_WHITE);
    tft.drawFastHLine(18, 152, 142, TFT_WHITE);

    // Lines  Tempreature
    tft.drawFastHLine(178, 44, 44, TFT_WHITE);
    tft.drawFastHLine(178, 152, 44, TFT_WHITE);
}

void MainScreen::Tick()
{
    static int16_t prevCLT = 5666;
    static int prevFuelLevel = 64;
    if (importantData.CLT != prevCLT) {
        prevCLT = importantData.CLT;
        this->coolerChanged =true;
    }
    if (importantData.fuelLevel != prevFuelLevel)
    {
        prevFuelLevel = importantData.fuelLevel;
        this->fuelChanged = true;
    }
}

void MainScreen::Render()
{
    { // Data Rendering
        if (dataUpdated)
        {
           
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
            tft.setTextSize(3);
            tft.setCursor(18 + 80, 178 + 5);
            tft.printf("%d", importantData.RPM);
            tft.printf("  ");
            tft.setCursor(88, 178 + 5 + 20 + 5);
            switch(oilError){
                case 0:
                    tft.printf("%.2f", importantData.OILPRSR);
                    tft.printf(" ");
                    break;
                case 1:
                    tft.printf("ERROR");
            }
            tft.setCursor(180, 178 + 5 + 20 + 5);
            tft.setTextSize(3);
            tft.printf("%2dV ", importantData.Voltage / 10);
        }
    }

    if (fuelError){
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setTextSize(3);
        tft.setCursor(25, 50);
        tft.printf("FUEL");
        tft.setCursor(25, 80);
        tft.printf("ERROR");
    }
    if (fuelChanged) {
        switch (importantData.fuelLevel)
        {
        case 0:
            fillRectInside(18, 18, 142, 27, TFT_DARKGREEN);
            fillRectInside(18, 44, 142, 69, TFT_DARKGREEN);
            fillRectInside(18, 112, 142, 41, 0xA100);
            fillRectInside(18, 152, 142, 26, 0x4004);
            break;
        case 1:
            fillRectInside(18, 18, 142, 27, TFT_DARKGREEN);
            fillRectInside(18, 44, 142, 69, TFT_DARKGREEN);
            fillRectInside(18, 112, 142, 41, 0xA100);
            fillRectInside(18, 152, 142, 26, TFT_RED);
            break;
        case 2:
            fillRectInside(18, 18, 142, 27, TFT_DARKGREEN);
            fillRectInside(18, 44, 142, 69, TFT_DARKGREEN);
            fillRectInside(18, 112, 142, 41, TFT_ORANGE);
            fillRectInside(18, 152, 142, 26, TFT_RED);
            break;
        case 3:
            fillRectInside(18, 18, 142, 27, TFT_DARKGREEN);
            fillRectInside(18, 44, 142, 69, TFT_GREEN);
            fillRectInside(18, 112, 142, 41, TFT_ORANGE);
            fillRectInside(18, 152, 142, 26, TFT_RED);
            break;
        case 4:
            fillRectInside(18, 18, 142, 27, TFT_GREEN);
            fillRectInside(18, 44, 142, 69, TFT_GREEN);
            fillRectInside(18, 112, 142, 41, TFT_ORANGE);
            fillRectInside(18, 152, 142, 26, TFT_RED);
            break;
        case 5:
            fillRectInside(18, 18, 142, 27, TFT_GREEN);
            fillRectInside(18, 44, 142, 69, TFT_GREEN);
            fillRectInside(18, 112, 142, 41, TFT_GREEN);
            fillRectInside(18, 152, 142, 26, TFT_GREEN);
            break;
        default:
            break;
        }
        fuelChanged = false;
    }

    if (coolerChanged) {
        if (importantData.CLT < 55)
        {
            fillRectInside(178, 18, 44, 26, 0x4004);
            fillRectInside(178, 44, 44, 109, TFT_DARKGREEN);
            fillRectInside(178, 152, 44, 26, TFT_BLUE);
        }
        else if (importantData.CLT > 55 && importantData.CLT < 95)
        {
            fillRectInside(178, 18, 44, 26, 0x4004);
            fillRectInside(178, 44, 44, 109, TFT_GREEN);
            fillRectInside(178, 152, 44, 26, TFT_BLUE);
        }
        else if (importantData.CLT > 95)
        {
            fillRectInside(178, 18, 44, 26, TFT_RED);
            fillRectInside(178, 44, 44, 109, TFT_RED);
            fillRectInside(178, 152, 44, 26, TFT_RED);
        }
        coolerChanged = false;
    }
}

void MainScreen::OnExit()
{

}