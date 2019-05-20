#pragma once

#include "mbed.h"

#include "Image.h"
#include "util.h"

namespace nxpcup {

class Camera {
public:
    static constexpr int ImageSize = 128;
    using ImageType = uint16_t;
    using CameraImage = Image<ImageType, ImageSize>;

    struct Config {
        PinName analogOut; /**< Analog out pin on camera - data from Camera **/
        PinName clk; /**< CLK pin **/
        PinName si; /**< Scan Impulse pin - finish of exposition **/
        uint32_t exposition_us; /**< setting for the exposition in microseconds **/

        static constexpr uint32_t EXPOSURE_TIME_MIN = 0;
        static constexpr uint32_t EXPOSURE_TIME_MAX = 150000;
    };

    /**
     * Constructor of class Camera.
     *
     * @param config struct @{Config}
     */
    Camera(const Config& config)
        : m_analogOut(config.analogOut)
        , m_clk(config.clk)
        , m_si(config.si)
    {
        m_clk.write(false);
        m_si.write(true);
        setExpositionUs(config.exposition_us);
    }

    /**
     * Set the exposition time in millisecond.
     *
     * @param exposition_ms time of exposition in millisecond - clamped (0 <-> 150)
     */
    void setExpositionMs(uint32_t exposition_ms)
    {
        setExpositionUs(exposition_ms * 1000);
    }

    /**
     * Set the exposition time in microsecond.
     *
     * @param exposition_us time of exposition in microsecond - clamped (0 <-> 150000)
     */
    void setExpositionUs(uint32_t exposition_us)
    {
        exposition_us = nxpcup::clamp<uint32_t>(
            exposition_us, Config::EXPOSURE_TIME_MIN, Config::EXPOSURE_TIME_MAX);
        this->m_expositionUs = exposition_us;
    }

    /**
     * Get the actual exposition in microsecond.
     */
    uint32_t expositionUs() const { return m_expositionUs; }

    /**
     * Get one pixel from line image.
     *
     * @param index of the pixel (0 - 127) - clamped
     * @return one pixel
     */
    ImageType pixel(int index) const
    {
        index = nxpcup::clamp<int>(index, 0, 127);
        return m_image[index];
    }

    /**
     * Returns reference to internal @{Image}.
     */
    CameraImage& image() { return m_image; }

    /**
     * Get pointer to array with pixels.
     *
     * @return pointer to array of pixels
     */
    [[deprecated]] std::array<ImageType, CameraImage::size>& pixels() {
        return m_image.data;
    }

    /**
     * Update the data from camera with exact exposition.
     *
     * Load one image, skip the data.
     * Wait for exposition time.
     * Load the final image and save.
     */
    void update()
    {
        m_si.write(true);
        m_clk.write(true);
        m_si.write(false);
        // Skip one image due to the correct m_exposition_us
        for (std::size_t i = 0; i < CameraImage::size; i++) {
            m_clk.write(false);
            m_clk.write(true);
        }
        m_clk.write(false);
        wait_us(m_expositionUs);
        updateImage();
    }

protected:
    /**
     * Update the data from camera immediate (without set exposition).
     */
    void updateImage()
    {
        m_si.write(true);
        m_clk.write(true);
        m_si.write(false);

        for (std::size_t j = 0; j < CameraImage::size; j++) {
            m_image[j] = m_analogOut.read_u16();
            m_clk.write(false);
            m_clk.write(true);
        }
        m_clk.write(false);
    }

    AnalogIn m_analogOut; /**< Read the analog data from camera */
    DigitalOut m_clk;
    DigitalOut m_si;

    CameraImage m_image;
    uint32_t m_expositionUs = 10000;
};

} // namespace nxpcup
