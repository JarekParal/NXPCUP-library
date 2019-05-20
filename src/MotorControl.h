#pragma once

#include "mbed.h"

#include "atoms/control/pid.h"

#include "Encoder.h"
#include "Motor.h"

namespace nxpcup {

class MotorControl {
public:
    struct Config {
        float coefficientP = 0.007; /**< proportional coefficient for PI regulator **/
        float coefficientI = 0.008; /**< integration coefficient for PI regulator **/

        float antiWindup = 0.5; /**< constrain the influence of integration component - <0-1> of output range **/
        float nullSpeedThreshold = 0.05; /**< under this speed is the output power zero - [m/s] **/
        float nullSpeedPower = 0.2; /**< constant for slower start of robot **/
    };

    /**
     * Constructor of class MotorControl.
     *
     * @param motor which will be regulated
     * @param encoder with information about the motor movements
     * @param config struct @{Config}
     */
    MotorControl(Motor& motor, Encoder& encoder, Config config)
        : m_motor(motor)
        , m_encoder(encoder)
        , m_config(config)
    {
        m_motorMaxPower = m_motor.maxPower();
    }

    /**
     * Set desired speed.
     *
     * @param desireSpeed in [m/s]
     */
    void setSpeed(float desiredSpeed) { m_desiredSpeed = desiredSpeed; }

    /**
     * Get desire speed.
     *
     * @return internal state of variable desire speed in [m/s]
     */
    float desiredSpeed() const
    {
        return m_desiredSpeed;
    }

    /**
     * Get actual speed.
     *
     * @return internal state of variable actual speed in [m/s]
     */
    float actualSpeed() const
    {
        return m_actualSpeed;
    }

    /**
     * Calculate and set new power for motor.
     *
     * Take actual speed from encoder and required speed -> calculate error -> update power.
     * @param timeSinceLastCallUs time in microseconds from last call this function (NOT USED)
     */
    void regulate(uint16_t timeSinceLastCallUs)
    {
        m_actualSpeed = m_encoder.speed();

        float error = m_desiredSpeed - m_actualSpeed;
        m_errorSum += m_config.coefficientI * error;
        if (m_errorSum > m_config.antiWindup) {
            m_errorSum = m_config.antiWindup;
        }
        if (m_errorSum < 0) {
            m_errorSum = 0;
        }

        float output = m_config.coefficientP * error + m_errorSum; // normalized output <0.0 - 1.0>
        if (output > 1) { // clamp the output
            output = 1;
        } else if (output < 0 || m_desiredSpeed < m_config.nullSpeedThreshold) { // turn off regulation around zero/low desire speed
            output = 0;
            m_errorSum = 0;
        } else if ( // solve slower start of robot
            m_actualSpeed < m_config.nullSpeedThreshold && output > m_config.nullSpeedPower) {
            output = m_config.nullSpeedPower;
            m_errorSum = 0;
        }
        output = output * m_motorMaxPower; // output <-1000; 1000>
        m_motor.power(int32_t(output));
    }

    /**
     * Get the actual configuration of the @{MotorControl}.
     */
    Config config() const
    {
        return m_config;
    }

    /**
     * Set new configuration for @{MotorControl}.
     *
     * @param config struct @{Config}
     */
    void setConfig(Config& config)
    {
        m_config = config;
        reset();
    }

    /**
     * Reset the whole regulator.
     */
    void reset()
    {
        m_motor.power(0);
        m_errorSum = 0;
    }

private:
    Motor& m_motor;
    Encoder& m_encoder;
    Config m_config;

    float m_desiredSpeed = 0; //[m/s]
    float m_actualSpeed = 0; //[m/s]
    uint16_t m_motorMaxPower = 0;
    float m_errorSum = 0;
};

} // namespace nxpcup
