#include "TuneScreen.h"
#include "speedy-comm.h"

void TuneScreen::OnEnter()
{

}

void TuneScreen::Tick()
{

}

void TuneScreen::Render()
{
    if (dataUpdated) {
        tft.setCursor(1, 12);
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setTextSize(8);
        int rpmTwoDigits = (int)floor((float)importantData.RPM / 100.0f);
        if (rpmTwoDigits < 100) {
            tft.printf("%02d", rpmTwoDigits);
        }
        tft.setCursor(113, 20);
        tft.setTextSize(5);
        tft.printf("%02.1f", (float)importantData.AFR / 10.0f);

        tft.setCursor(1, 91);
        tft.setTextSize(8);
        tft.printf("%03d", importantData.MAP);

        tft.setCursor(130, 91);
        tft.setTextSize(2);
        tft.setCursor(130, 111);
        tft.printf("kPA");
        
        tft.setCursor(130, 72);
        tft.setTextSize(3);
        tft.printf("%02.1fV ", (float)importantData.Voltage / 10.0f);
        
        tft.setCursor(1, 152);
        tft.setTextSize(2);
        tft.printf("IAT:%02dC TPS:%02d ", importantData.IAT, importantData.TPS);
        tft.println();
        switch(fuelError){
            case 0:
                tft.printf("FUEL:%d/5   ", importantData.fuelLevel);
                break;
            case 1:
                tft.printf("FUEL: ERROR");
                break;
        }
        tft.setCursor(1, 185);
        tft.setTextSize(4);
        tft.printf("CLT:%02dC ", importantData.CLT);
        }
}

void TuneScreen::OnExit()
{
}