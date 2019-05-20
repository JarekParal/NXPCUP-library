#pragma once

#include "mbed.h"

#include "SoftPWM.h"
#include "util.h"

namespace nxpcup {

class Motor {
public:
    struct Config {
        PinName pwm0; /**< pin0 with PWM for motor driver **/
        PinName pwm1; /**< pin1 with PWM for motor driver **/
        bool inverse = false; /**< inverse the direction of the motor **/

        static constexpr int PERIOD_US = 500; // 2000 Hz
        static constexpr int MAX_POWER = 1000; // MUST BE DIVISIBLE BY PERIOD_US
        static constexpr int POWER_DIVIDER = MAX_POWER / PERIOD_US; // 1000 / 2 = 500
        static_assert(POWER_DIVIDER * PERIOD_US == MAX_POWER);
    };

    /**
     * Constructor of class Motor.
     *
     * @param pin0 name of the pin for motor driver (require PWM)
     * @param pin0 name of the pin for motor driver (require PWM)
     */
    Motor(const PinName pin0, const PinName pin1)
        : m_maxPowerPercent(100)
    {
#if defined MOTOR_HARDWARE_PWM
        m_in0 = new PwmOut(pin0);
        m_in1 = new PwmOut(pin1);
#elif defined MOTOR_SOFTWARE_PWM
        m_in0 = new SoftPWM(pin0);
        m_in1 = new SoftPWM(pin1);
#endif

        m_in0->period_us(Config::PERIOD_US);
        m_in1->period_us(Config::PERIOD_US);

        m_in0->pulsewidth_us(0);
        m_in1->pulsewidth_us(0);
    }

    /**
     * Constructor of class Motor.
     *
     * @param config structure @{Config}
     */
    Motor(const Config& config)
        : Motor(config.pwm0, config.pwm1)
    {
        m_inverse = config.inverse;
    }

    /**
     * Set motor power.
     *
     * @param power of the motor (-1000 <-> 1000)
     * 		  under 0 => backward, over 0 => forward, 0 stop
     */
    void power(int power)
    {
        power = nxpcup::clamp<int>(power, -Config::MAX_POWER, Config::MAX_POWER);

        power = (m_inverse ? -1 : 1) * power;
        if (m_maxPowerPercent != 100) {
            power = (power * m_maxPowerPercent) / 100;
        }

        if (power > 0) {
            m_in0->pulsewidth_us(power / Config::POWER_DIVIDER);
            m_in1->pulsewidth_us(0);
        } else {
            m_in0->pulsewidth_us(0);
            m_in1->pulsewidth_us(-power / Config::POWER_DIVIDER);
        }
    }

    /**
     * Set the maximal motor power in percent.
     *
     * @param percent of the maximal power (0 <-> 100)
     * 		  function power() has still same range
     */
    void setMaxPowerPercent(int percent)
    {
        m_maxPowerPercent = nxpcup::clamp<int>(percent, 0, 100);
    }

    /**
     * Return the actual maximal motor power in percent (0 <-> 100).
     */
    int maxPowerPercent() const { return m_maxPowerPercent; }

    /**
     * Return the constant maximal power.
     */
    int maxPower() const { return Config::MAX_POWER; }

private:
#if defined MOTOR_HARDWARE_PWM
    PwmOut* m_in0;
    PwmOut* m_in1;
#elif defined MOTOR_SOFTWARE_PWM
    SoftPWM* m_in0;
    SoftPWM* m_in1;
#endif

    bool m_inverse = false;
    int m_maxPowerPercent;
};

} // namespace nxpcup
