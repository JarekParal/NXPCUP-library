#pragma once

#include "Adafruit_GFX/Adafruit_SSD1306.h"

class SPIPreInit : public SPI
{
public:
    struct Pinout{
        PinName mosi;
        PinName miso;
        PinName clk;
    };

    SPIPreInit(Pinout pinout) 
        : SPIPreInit(pinout.mosi, pinout.miso, pinout.clk) {
    }

    SPIPreInit(PinName mosi, PinName miso, PinName clk) : SPI(mosi,miso,clk)
    {
        format(8,3);
        frequency(2000000);
    };
};

class Adafruit_SSD1306_SpiTfc : public Adafruit_SSD1306_Spi 
{
public:
    struct Pinout{
        PinName dataComand;
        PinName reset;
        PinName chipSelect;
    };

    Adafruit_SSD1306_SpiTfc(SPI &spi, 
                            const Pinout & pinout, 
                            uint8_t rawHieght = 32, 
                            uint8_t rawWidth = 128) 
        : Adafruit_SSD1306_Spi(
            spi,
            pinout.dataComand,
            pinout.reset,
            pinout.chipSelect,
            rawHieght,
            rawWidth
        )
    {};
};

void displayCameraDataOLED(Adafruit_SSD1306 &display, uint16_t *  data) {
	display.fillScreen(0);
	for (int i = 0; i < 128; ++i) {
		display.drawFastVLine(i, 0, static_cast<int16_t>((data[i] >> 11)), 1);
	}
}
