#include <TFT_eSPI.h>
#include <SPI.h>
#include <Adafruit_NeoPixel.h>
#include "speedy-comm.h"
#include "Screen.h"
#include "MainScreen.h"
#include "TuneScreen.h"

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(3, PB11, NEO_GRB + NEO_KHZ800);

TFT_eSPI tft = TFT_eSPI();
HardwareSerial Serial2(USART2); // for some reason this isn't defined in arduino_core_stm32

bool dataUpdated = false;
int z = 0;
int oilraw = 0;
int pageNumber = 0;
int oldPage = 0;
int fuelraw = 0;
int fuelError = 0;
int oilError = 0;
volatile bool nextScreen = false;
Screen* currentScreen = NULL;
struct ImportantData importantData = {0};

void changeScreen(Screen* screen) {
    tft.fillScreen(TFT_BLACK);

    if (currentScreen != NULL) {
        currentScreen->OnExit();
    }
    delete currentScreen;
    currentScreen = screen;
    currentScreen->OnEnter();
}

void addUI()
{
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    if (interrupt_time - last_interrupt_time > 200)
    {
        nextScreen = true;   
    }
    last_interrupt_time = interrupt_time;
}

void  setup() {
    Serial.begin(115200); // for debugging

    // Speeduino init
    Serial2.begin(115200); // baudrate for Speeduino is 115200

    pinMode(PB0, INPUT_ANALOG);
    pinMode(PA4, INPUT_ANALOG);
    pinMode(PA6, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PA6), addUI, FALLING);
    analogReadResolution(12);
    // Display init
    tft.init(); // initialize a ST7789 chip, 240×240 pixels
    tft.setRotation(2);
    tft.fillScreen(TFT_BLACK);
 
    delay(2000);
    changeScreen(new MainScreen());
    //changeScreen(new TuneScreen());
    pixels.begin();
    pixels.setBrightness(100);
}

void updateSensorsData()
{
    fuelraw = analogRead(PA4);
    oilraw = analogRead(PB0);
    // importantData.OILPRSR = map(oilraw,  696, 3675, 0, 100);
    importantData.OILPRSR = (((float)oilraw - 670.0f) / (3675.0f - 670.0f)) * 10.0f; // min max  min
    // Serial2.printf("Data %.2f\n", importantData.OILPRSR);
}

bool processSpeeduinoNMessage()
{
    static uint8_t buffer[256];
    if (Serial2.read() != 'n') {
        // FAILED!!!
        return false;
    }
    Serial2.read();
    //Serial.println("Got data");
    byte length = Serial2.read();
    //byte length = 75;
    int returnedLength = Serial2.readBytes(buffer, length);
    //Serial.printf("Length: %d, returned: %d\n",  length, returnedLength);

    if (
        (((uint16_t)buffer[15] << 8) | buffer[14]) > 8000 || // RPM
        (buffer[6] - 40) > 142 ||  //clt
        (buffer[7] - 40) > 142 ||  //iat
        (buffer[10] < 80) || buffer[10] > 200
    )
    {
        return true;
    } 

    importantData.RPM = ((uint16_t)buffer[15] << 8) | buffer[14];
    importantData.MAP = ((uint16_t)buffer[5] << 8) | buffer[4];
    importantData.Voltage = buffer[9];
    importantData.AFR = buffer[10];
    importantData.TPS = buffer[24];
    importantData.IAT = buffer[6] - 40;
    importantData.CLT = buffer[7] - 40;

    
    updateSensorsData();

    dataUpdated = true;
    return true;
}

void updateLeds() {
    if (dataUpdated) {
        switch (importantData.fuelLevel)
        {
            case 0:
                pixels.setPixelColor(1, pixels.Color(255, 0, 0));
                break;
            case 1:
                pixels.setPixelColor(1, pixels.Color(255, 0, 0));
                break;
            case 2:
                pixels.setPixelColor(1, pixels.Color(255, 70, 10));
                break;
            case 3:
                pixels.setPixelColor(1, pixels.Color(0, 255, 0));
                break;
            case 4:
                pixels.setPixelColor(1, pixels.Color(0, 255, 0));
                break;
            case 5:
                pixels.setPixelColor(1, pixels.Color(0, 255, 0));
                break;
        }

        if (importantData.CLT < 55)
        {
            pixels.setPixelColor(0,  pixels.Color(0, 0, 255));
        }
        else if (importantData.CLT > 55 && importantData.CLT < 95)
        {
            pixels.setPixelColor(0, pixels.Color(0, 255, 0));
        }
        else if (importantData.CLT > 95)
        {
            pixels.setPixelColor(0, pixels.Color(255, 0, 0));
        }

        pixels.show();
    }
    //min 2085, max 2730 calculated
    if (fuelraw > 2000 && fuelraw < 2281)
    { // second value should be minimum for engine to run
        importantData.fuelLevel = 5;
    }
    /*else if (fuelraw > 2101 && fuelraw < 2210)
    {
        importantData.fuelLevel = 4;
    }*/
    else if (fuelraw > 2280 && fuelraw < 2480)
    {
        importantData.fuelLevel = 3;
    }
    else if (fuelraw > 2481 && fuelraw < 2610)
    {
        importantData.fuelLevel = 2;
    }
    else if (fuelraw > 2611 && fuelraw < 2680)
    {
        importantData.fuelLevel = 1;
    }
    else if (fuelraw < 2680)
    {
        importantData.fuelLevel = 0;
    }
    //Serial.println(fuelraw);
    
    if (oilraw < 650 || oilraw > 3750)
    {
        oilError = 1;
    }
    else
    {
        oilError = 0;
    }
    
    if (fuelraw < 1980 || fuelraw > 3000)
    {
        fuelError = 1;
        importantData.fuelLevel = 0;
    }
    else
    {
        fuelError = 0;
    }
    if (oilError || fuelError)
    {
        pixels.setPixelColor(2, pixels.Color(255, 0, 0));
    }
    else
    {
        pixels.setPixelColor(2, pixels.Color(0, 0, 0));
    }
}

void loop()
{
    
    static uint8_t speeduinoState = 1;
    { // Speeduino handling
        if (speeduinoState == 1)
        {
            //Serial.println("Sending n");
            Serial2.write('n');
            speeduinoState = 2;
        }
        else if (speeduinoState == 2 && Serial2.available() >= 75)
        {
            if  (processSpeeduinoNMessage()) {
                speeduinoState = 0;
            }
        }
    }
    { // Data Updating
        static uint32_t lastUpdateTime = 0;
        if (millis() - lastUpdateTime >= 150) {
            speeduinoState = 1;
            lastUpdateTime =  millis();
        }
    }

    if (nextScreen) {
        pageNumber = (pageNumber + 1) % 2;
        switch (pageNumber)
        {
        case 0:
            changeScreen(new MainScreen());
            break;
        case 1:
            changeScreen(new TuneScreen());
            break;
        }
        nextScreen = false;
    }

    //zobrauje, necita ?

    static uint32_t lastDataUpdateTime = millis();

    if (dataUpdated) {
        lastDataUpdateTime = millis();
    } else {
        if (millis() - lastDataUpdateTime >= 5000) {
            lastDataUpdateTime = millis();
            updateSensorsData();
            dataUpdated = true;
        }
    }

    updateLeds();
    currentScreen->Tick();
    currentScreen->Render();

    if (dataUpdated) {
        dataUpdated = false;
    }
}

void fillRectInside(int32_t x, int32_t y, int32_t w, int32_t h, uint32_t color)
{
    tft.fillRect(x + 1, y + 1, w - 2, h - 2, color);
}


/*
Red: 0xF800
Green: 0x07E0
Blue: 0x001F
Orange: 0xFDA0

Dark red: 0x4004
Dark green: 0x03E0
Dark blue: 
Dark orange: 0xA100

Main Page


Frame
  // Rectangle Fuel
    tft.drawRect(18,  18, 142, 160, TFT_WHITE);
    //  Rectangle   Temperature
    tft.drawRect(178, 18, 44, 160, TFT_WHITE);

    // Lines Fuel
    tft.drawFastHLine(18, 44, 142, TFT_WHITE);
    tft.drawFastHLine(18, 112, 142, TFT_WHITE);
    tft.drawFastHLine(18, 152, 142, TFT_WHITE);

    // Lines  Tempreature
    tft.drawFastHLine(178, 44, 44, TFT_WHITE);
    tft.drawFastHLine(178, 152, 44, TFT_WHITE);

Text

    tft.setCursor(18, 178 + 5);
    tft.setTextColor(TFT_WHITE, TFT_BLACK);
    tft.setTextSize(3);
    tft.print("RPM:");
    // tft.setTextColor(TFT_GREEN, TFT_BLACK);
    tft.setCursor(18, 178 + 5 + 20 +5);
    tft.print("OIL:");

5/5 fuel
    // Rectangles Fuel
    tft.fillRect(18, 18, 142, 26, TFT_GREEN);
    tft.fillRect(18, 44, 142, 86, TFT_GREEN);
    tft.fillRect(18, 112, 142, 40, TFT_GREEN);
    tft.fillRect(18, 152, 142, 26, TFT_GREEN);
4/5 fuel 
    // Rectangles Fuel
    tft.fillRect(18, 18, 142, 26, TFT_GREEN);
    tft.fillRect(18, 44, 142, 86, TFT_GREEN);
    tft.fillRect(18, 112, 142, 40, TFT_ORANGE);
    tft.fillRect(18, 152, 142, 26, TFT_RED);

3/5  fuel
    // Rectangles Fuel
    tft.fillRect(18, 18, 142, 26, TFT_DARKGREEN);
    tft.fillRect(18, 44, 142, 86, TFT_GREEN);
    tft.fillRect(18, 112, 142, 40, TFT_ORANGE);
    tft.fillRect(18, 152, 142, 26, TFT_RED);
2/5 fuel
    // Rectangles Fuel
    tft.fillRect(18, 18, 142, 26, TFT_DARKGREEN);
    tft.fillRect(18, 44, 142, 86, TFT_DARKGREEN);
    tft.fillRect(18, 112, 142, 40, TFT_ORANGE);
    tft.fillRect(18, 152, 142, 26, TFT_RED);
1/5 fuel
    // Rectangles Fuel
    tft.fillRect(18, 18, 142, 26, TFT_DARKGREEN);
    tft.fillRect(18, 44, 142, 86, TFT_DARKGREEN);
    tft.fillRect(18, 112, 142, 40, 0xA100);
    tft.fillRect(18, 152, 142, 26, TFT_RED);
0/5 fuel
    // Rectangles Fuel
    tft.fillRect(18, 18, 142, 26, TFT_DARKGREEN);
    tft.fillRect(18, 44, 142, 86, TFT_DARKGREEN);
    tft.fillRect(18, 112, 142, 40, 0xA100);
    tft.fillRect(18, 152, 142, 26, 0x4004);

3/3 CLT
    // Rectangles Temperature
    tft.fillRect(178, 18, 44, 26, TFT_RED);
    tft.fillRect(178, 44, 44, 126, TFT_RED);
    tft.fillRect(178, 152, 44, 26, TFT_RED);

2/3  CLT
    // Rectangles Temperature
    tft.fillRect(178, 18, 44, 26, 0x4004);
    tft.fillRect(178, 44, 44, 126, TFT_GREEN);
    tft.fillRect(178, 152, 44, 26, TFT_BLUE);

1/3  CLT
     // Rectangles Temperature
    tft.fillRect(178, 18, 44, 26, 0x4004);
    tft.fillRect(178, 44, 44, 126, TFT_DARKGREEN);
    tft.fillRect(178, 152, 44, 26, TFT_BLUE);

    
    
    
    
    
    
    
*/