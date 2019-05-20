#pragma once

#include "mbed.h"

namespace nxpcup {

class ObstacleDetector {
public:
    struct Config {
        PinName leftSensorPin; /**< analog in pin for left optical obstacle sensor  **/
        PinName rightSensorPin; /**< analog in pin for right optical obstacle sensor  **/
        int thresholdDistance; /**< value from optical sensor that trigger the detector  **/
        int moveFromObstacle; /**< how far from detected obstacle go - lower value -> closer to line detected  **/
        float encoderAvoidDistance; /**< when the detector find obstacle, the distance which must robot go then return to center of road and again search for obstacle  **/
        bool isDeactivate = false; /**< deactivate the detector -> still check the obstacle, but not correct the robot path  **/
    };

    enum class AvoidingObstacle {
        no,
        onRightSide,
        onLeftSide
    };

    /**
     * Constructor of class ObstacleDetector.
     *
     * @param config struct @{Config}
     */
    ObstacleDetector(Config& config)
        : m_leftSensor(config.leftSensorPin)
        , m_rightSensor(config.rightSensorPin)
        , m_config(config)
    {
    }

    /**
     * Check the error on sensors and modify the trajectory if find obstacle.
     *
     * @param encoderDistance distance from one encoder
     * @param borderDetectorError error from border detector
     * @param leftBorder positon of left border from border detector
     * @param rightBorder positon of right border from border detector
     */
    int error(float encoderDistance,
        int borderDetectorError,
        int leftBorder,
        int rightBorder)
    {
        updateSensorValue();
        checkObstacle(encoderDistance);

        if (m_config.isDeactivate) {
            return borderDetectorError;
        }

        if (m_avoidingObstacle == AvoidingObstacle::no) {
            return borderDetectorError;
        }
        if (m_avoidingObstacle == AvoidingObstacle::onLeftSide) {
            return leftBorder - (64 - m_config.moveFromObstacle); // see just right line;
        }
        // obstacle on the right side
        return rightBorder - (64 + m_config.moveFromObstacle);
    }

    /**
     * Get the last measured value from left sensor.
     *
     * @return measured distance without unit
     */
    int leftSensorValue() const
    {
        return m_leftSensorValue;
    }

    /**
     * Get the last measured value from right sensor.
     *
     * @return measured distance without unit
     */
    int rightSensorValue() const
    {
        return m_rightSensorValue;
    }

    /**
     * Get actual state of obstacle avoiding.
     *
     * @return @{AvoidingObstacle}
     */
    AvoidingObstacle avoidingObstacle() const
    {
        return m_avoidingObstacle;
    }

    /**
     * Get actual state of obstacle in integer value.
     *
     * @return transformed @{AvoidingObstacle}: -1 = left, 1 = right, 0 = nothing
     */
    int avoidingObstacleInteger() const
    {
        switch (m_avoidingObstacle) {
        case AvoidingObstacle::no:
        default:
            return 0;
        case AvoidingObstacle::onLeftSide:
            return -1;
        case AvoidingObstacle::onRightSide:
            return 1;
        }
    }

    /**
     * Get the actual configuration of the @{ObstacleDetector}.
     */
    Config config() const
    {
        return m_config;
    }

    /**
     * Set new configuration for @{ObstacleDetector}.
     *
     * @param config struct @{Config}
     */
    void setConfig(Config& config)
    {
        m_config = config;
        reset();
    }

    /**
     * Reset this detector to initial state.
     */
    void reset()
    {
        m_encoderDistanceStart = 0;
        m_avoidingObstacle = AvoidingObstacle::no;
    }

private:
    /**
     * Update data from sensors.
     */
    void updateSensorValue()
    {
        m_leftSensorValue = m_leftSensor.read_u16();
        m_rightSensorValue = m_rightSensor.read_u16();
    }

    /**
     * Check the actual data from sensors and modify the state of detector @{AvoidingObstacle}
     *
     * @param encoderDistance distance from one encoder
     */
    void checkObstacle(float encoderDistance)
    {
        if (m_avoidingObstacle == AvoidingObstacle::no) {
            if (m_leftSensorValue < m_config.thresholdDistance
                && m_rightSensorValue < m_config.thresholdDistance) {
                return;
            }
            if (m_leftSensorValue > m_config.thresholdDistance) {
                m_avoidingObstacle = AvoidingObstacle::onLeftSide; // see just right line;
            } else {
                m_avoidingObstacle = AvoidingObstacle::onRightSide; // see just left line
            }
            m_encoderDistanceStart = encoderDistance;
        } else {
            if (m_encoderDistanceStart + m_config.encoderAvoidDistance < encoderDistance) {
                m_avoidingObstacle = AvoidingObstacle::no;
            }
        }
    }

    AnalogIn m_leftSensor;
    AnalogIn m_rightSensor;
    Config m_config;

    AvoidingObstacle m_avoidingObstacle = AvoidingObstacle::no;
    int m_leftSensorValue = 0;
    int m_rightSensorValue = 0;
    float m_encoderDistanceStart = 0;
};

} // namespace nxpcup
