#pragma once

#include "mbed.h"

#include "util.h"

namespace nxpcup {

class Servo {
public:
    struct Config {
        PinName pin; /**< pin with PWM for servo */
        int8_t correctionAngle = 0; /**< correction of the zero angle in degree */
        uint8_t servoMinMaxAngle = 90; /**< minimal and maximal angle in degree */
        int8_t defaultCenterAngle = 0; /**< default angle after start in degree */
        bool isReverseSignal = false; /**< reverse the setting of angle (@{setAngle()}, @{setAngleCenter()})
                                        *  due to some servo with inverse interpretation of the signal.
                                        *  IMPORTANT: could change meaning of other parameters (@{correctionAngle}, @{servoMinMaxAngle}...) */

        uint32_t minUs = 900; /**< minimal time of servo pulse in microseconds */
        uint32_t maxUs = 2100; /**< maximal time of servo pulse in microseconds */

        static constexpr uint16_t PERIOD_US = 20000; // 50 Hz
        static constexpr uint16_t CENTER_US = 1500;
    };

    /**
     * Constructor for Servo class.
     *
     * @param config structure @{Config}
     */
    Servo(Config config)
        : Servo(config.pin, config.minUs, config.maxUs)
    {
        m_isReverseSignal = config.isReverseSignal;
        setAngleCorrection(config.correctionAngle);
        setAngleMinMaxCenter(config.servoMinMaxAngle);
        setAngleCenter(config.defaultCenterAngle);
    }

    /**
     * Constructor for Servo class.
     *
     * @param pinName name of the servo pin
     * @param minUs minimal pulse wide (default pulse for 0 degree)
     * @param minUs maximal pulse wide (default pulse for 180 degree)
     */
    Servo(PinName pin, uint16_t minUs = 1000, uint16_t maxUs = 2000)
        : m_minUs(minUs)
        , m_maxUs(maxUs)
    {
#if defined SERVO_HARDWARE_PWM
        servo = new PwmOut(pin);
#elif defined SERVO_SOFTWARE_PWM
        servo = new SoftPWM(pin);
#endif

        servo->period_us(Config::PERIOD_US);
        servo->pulsewidth_us(Config::CENTER_US);
    }

    /**
     * Set correction angle for functions working with angle.
     *
     * @param angle which will be add to the set angle
     */
    void setAngleCorrection(int8_t angle) { m_correctionAngle = angle; }

    /**
     * Set minimal and maximal allowed angle.
     *
     * @param min suggested range (0 <-> 180) - not checked by this function
     * @param max suggested range (0 <-> 180) - not checked by this function
     */
    void setAngleMinMax(uint8_t min, uint8_t max)
    {
        m_minAngle = min + m_correctionAngle;
        m_maxAngle = max + m_correctionAngle;
    }

    /**
     * Set maximal allowed angle from center (middle).
     *
     * @param diffAngle allowed +- range from center
     * 		  suggested range (0 <-> 90) - not checked by this function
     */
    void setAngleMinMaxCenter(uint8_t diffAngle)
    {
        m_minAngle = 90 - diffAngle + m_correctionAngle;
        m_maxAngle = 90 + diffAngle + m_correctionAngle;
    }

    /**
     * Set position of the servo in degree.
     *
     * @param degree in range (0 <-> 180)
     */
    void setAngle(uint8_t degree)
    {
        m_lastAngle = nxpcup::clamp<uint8_t>(degree + m_correctionAngle, m_minAngle, m_maxAngle);
        uint16_t positionInMicrosecond = 0;
        if (m_isReverseSignal) {
            positionInMicrosecond = m_maxUs - (m_lastAngle * usDiff) / 180; // 180 degree = max range
        } else {
            positionInMicrosecond = m_minUs + (m_lastAngle * usDiff) / 180; // 180 degree = max range
        }
        setMicrosecond(positionInMicrosecond);
    }

    /**
     * Set position of the servo in degree around center (middle position).
     *
     * @param degree in range (-90 <-> 90) - not checked by this function
     * @param reverse the range (0 = 0, 90 => -90, -90 => 90)
     */
    void setAngleCenter(int8_t degree, bool reverse = false)
    {
        setAngle(90 + ((reverse ? -1 : 1) * degree));
    }

    /**
     * Get last set angle - value could be clamped in @{angle()}
     */
    uint8_t angle() const { return m_lastAngle; }

    /**
     * Get last center set angle - value could be clamped in @{angle()}
     */
    int8_t centerAngle() const { return angle() - 90; }

    /**
     * Get minimal allowed angle.
     *
     * Set in constructor or with functions @{angleMinMax}, @{angleMinMaxCenter}.
     */
    int getMinAngle() const { return m_minAngle; }

    /**
     * Get maximal allowed angle.
     *
     * Set in constructor or with functions @{angleMinMax}, @{angleMinMaxCenter}.
     */
    int getMaxAngle() const { return m_maxAngle; }

private:
    /**
     * Set position (pulse wide) of the servo in microsecond.
     */
    void setMicrosecond(uint16_t microsecond)
    {
        servo->pulsewidth_us(microsecond);
    }

#if defined SERVO_HARDWARE_PWM
    PwmOut* servo;
#elif defined SERVO_SOFTWARE_PWM
    SoftPWM* servo;
#endif

    uint8_t m_minAngle = 0;
    uint8_t m_maxAngle = 180;
    int8_t m_correctionAngle = 0;
    uint8_t m_lastAngle = 0;
    bool m_isReverseSignal = false;

    const uint32_t m_minUs;
    const uint32_t m_maxUs;
    const uint32_t usDiff = m_maxUs - m_minUs;
};

} // namespace nxpcup
