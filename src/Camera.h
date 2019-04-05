#pragma once

#include <array>
#include <numeric>

#include "mbed.h"
#include "util.h"

namespace nxpcup {

class Camera {
public:
    struct Pinout {
        PinName analog_out; /**< Analog out - data from Camera */
        PinName clk; /**< CLK */
        PinName si; /**< Scan Impulse - finish of exposition */
    };

    /**
     * Constructor of camera class
     *
     * @param struct @{Pinout}
     */
    Camera(const Pinout &pinout)
		: AOUT(pinout.analog_out),
		  CLK(pinout.clk),
		  SI(pinout.si)
    {
      CLK.write(false);
      SI.write(true);
    }

    /**
     * Set the exposition time in millisecond
     * @param exposition_ms time of exposition in millisecond (0 <-> 150)
     */
    void expositionMs(uint32_t exposition_ms) {
    	expositionUs(exposition_ms * 1000);
    }

    /**
     * Set the exposition time in microsecond
     * @param exposition_us time of exposition in microsecond (0 <-> 150000)
     */
    void expositionUs(uint32_t exposition_us) {
    	exposition_us = nxpcup::clamp<uint32_t>(
    			exposition_us,
				EXPOSURE_TIME_MIN,
				EXPOSURE_TIME_MAX
		);
        exposureUs = exposition_us;
    }

    /**
     * Get one pixel from line image
     * @param index of the pixel (0 - 127)
     * @return one pixel
     */
    uint16_t pixel(uint8_t index) {
    	index = nxpcup::clamp<uint8_t>(index, 0, 127);
        return image[index];
    }

    /**
     * Get pointer to array with pixels
     * @return pointer to array of pixels
     */
    uint16_t* pixels() {
        return &(image[0]);
    }
    
    /**
     * Update the data from camera.
     */
    void update() {
        SI.write(true);
        CLK.write(true);
        SI.write(false);
        // Skip one image due to the correct exposureTime
        for(uint8_t i = 0; i < numberOfPixels; i++) {
            CLK.write(false);
            CLK.write(true);
        }
        CLK.write(false);
        wait_us(exposureUs);
        updateImage();
    }

    /**
     * Differentiate the image.
     * @param inputArray new array for result after differentiate the image
     */
    void difference(uint16_t* inputArray) {
        for(uint8_t i = 1; i < numberOfPixels; i++) {
        	inputArray[i] = abs(image[i] - image[i - 1]);
        }
    }

    template<uint8_t medianSize>
    void median(uint16_t* inputArray) {
    	std::array< int8_t, medianSize*2 + 1 > indexes;
    	std::iota(indexes.begin(), indexes.end(), -medianSize);
    	for(uint8_t i = medianSize; i < numberOfPixels - medianSize; ++i) {

    		std::sort(indexes.begin(), indexes.end(), [&](int8_t lh, int8_t rh){
    			return image[i + lh] < image[i + rh];
    		});

    		inputArray[i] = image[i + indexes[medianSize]];
    	}
    }

private:
    void updateImage() {
        SI.write(true);
        CLK.write(true);
        SI.write(false);

        for(uint8_t j = 0; j < numberOfPixels; j++) {
            image[j] = AOUT.read_u16();
            CLK.write(false);
            CLK.write(true);
        }
        CLK.write(false);
    }

    static const uint8_t numberOfPixels = 128;
    static const uint32_t EXPOSURE_TIME_MIN = 0;
    static const uint32_t EXPOSURE_TIME_MAX = 150000;

    AnalogIn AOUT;
    DigitalOut CLK;
    DigitalOut SI;

    uint16_t image[numberOfPixels];
    uint32_t exposureUs = 10000;
};

} // namespace nxpcup
