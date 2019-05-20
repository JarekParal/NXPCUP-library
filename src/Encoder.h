#pragma once

#include "mbed.h"

namespace nxpcup {

class Encoder {
public:
    struct Config {
        PinName pin; /**< pin with interrupt for signal from encoder **/
        int pulsePerRevolution = 400; /**< number of pulse on encoder per revolution**/
        float gearRatio = 78 / 36.0; /**< gear ration between encoder and wheel **/
        int wheelCircumference = 204; /**< wheel circumference in milimeters **/
    };

    /**
     * Constructor of class Encoder.
     *
     * @param config struct @{Config}
     */
    Encoder(Config config)
        : m_config(config)
        , m_interrupt(config.pin)
    {
        m_interrupt.rise(callback(this, &Encoder::increment));

        // set the PullUp for the encoder pin
        DigitalIn inputPin(config.pin);
        inputPin.mode(PullUp);
    }

    /**
     * Update speed and distance.
     *
     * @param timeSinceLastCallUs time in microseconds from last call this function (NOT USED)
     * @return velocity in [m/s]
     */
    float update(uint16_t timeSinceLastCallUs)
    {
        m_speed = (1000.0 * m_config.wheelCircumference * m_count) / (m_config.pulsePerRevolution * m_config.gearRatio * timeSinceLastCallUs);
        m_distanceCount += m_count;
        m_count = 0;

        return m_speed;
    }

    /**
     * Get the actual speed.
     *
     * @return speed in [m/s]
     */
    float speed() const { return m_speed; }

    /**
     * Get the distance from start of the program or last reset @{resetDistance}
     *
     * @return distance in meters
     */
    float distance() const
    {
        float encoderParams = m_config.pulsePerRevolution * m_config.gearRatio * 1000 /* mm => meters */;
        return static_cast<float>(m_config.wheelCircumference * m_distanceCount) / (encoderParams);
    }

    /**
     * Reset the internal counter for distance measurement.
     */
    void resetDistance()
    {
        m_distanceCount = 0;
    }

private:
    /**
     * Increment the internal variable m_count.
     */
    void increment() { m_count++; }

    Config m_config;
    InterruptIn m_interrupt;
    volatile uint16_t m_count = 0;
    float m_speed = 0;
    long m_distanceCount = 0;
};

} // namespace nxpcup
