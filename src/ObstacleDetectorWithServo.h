#pragma once

#include <optional>

#include "Image.h"
#include "Servo.h"
#include "mbed.h"

namespace nxpcup {

class ObstacleDetectorWithServo {
public:
    struct Config {
        PinName sensorPin; /**< analog in pin for optical obstacle sensor **/
        Servo::Config servoConfig; /**< configuration for servo which move with sensor **/
        int thresholdDistance; /**< value from optical sensor that trigger the detector **/
        int moveFromObstacle; /**< how far from detected obstacle go - lower value -> closer to line detected **/
        float encoderAvoidDistance; /**< when the detector find obstacle, the distance which must robot go then return to center of road and again search for obstacle  **/
        bool isDeactivate = false; /**< deactivate the detector -> still check the obstacle, but not correct the robot path  **/

        // 5 degree step is 36 image size...
        // 10 degree step is 18 image size...
        static constexpr int IMAGE_SIZE = 30;
        static constexpr int SERVO_MAX_RANGE_DEGREE = 180;
    };

    enum class AvoidingObstacle {
        no,
        onRightSide,
        onLeftSide
    };

    using ImageType = uint16_t;
    using ObstacleImage = Image<ImageType, Config::IMAGE_SIZE>;

    /**
     * Constructor of class ObstacleDetectorWithServo.
     *
     * @param config struct @{Config}
     */
    ObstacleDetectorWithServo(Config config)
        : m_sensor(config.sensorPin)
        , m_servo(config.servoConfig)
        , m_config(config){};

    /**
     * Get the angle of servo for given index.
     *
     * @param index of the position (the range is define by @{Config::IMAGE_SIZE})
     * @return angle for the index
     */
    int servoAngle(int index)
    {
        int servoAngleRange = m_servo.getMaxAngle() - m_servo.getMinAngle();
        auto angle = m_servo.getMinAngle() + servoAngleRange * index / m_image.size;
        return angle;
    }

    /**
     * Get current servo position in degree.
     */
    int servoPositionDegree() const
    {
        return m_servoPositionDegree;
    }

    /**
     * Get current value from sensor (withnout unit).
     */
    int sensorValue() const
    {
        return m_sensorValue;
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
        update();
        check(encoderDistance);

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
        ; // see just left line
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
     * Get the distance that trigger the detector.
     *
     * @return value without unit
     */
    int distanceThatTriggered() const
    {
        return m_distanceThatTriggered;
    }

    /**
     * Get the actual configuration of the @{ObstacleDetectorWithServo}.
     */
    Config config() const
    {
        return m_config;
    }

    /**
     * Set new configuration for @{ObstacleDetectorWithServo}.
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
    int update()
    {
        // filtration - average
        m_sensorValue = 0;
        for (int i = 0; i < 10; ++i) {
            m_sensorValue += m_sensor.read_u16();
        }
        m_sensorValue /= 10;

        m_image[m_index] = m_sensorValue;

        if (m_direction == 1) {
            m_index++;
            if (m_index == (m_image.size - 1)) {
                m_direction = -1;
            }
        } else if (m_direction == -1) {
            m_index--;
            if (m_index == 0) {
                m_direction = 1;
            }
        }

        m_servoPositionDegree = servoAngle(m_index);
        m_servo.setAngle(m_servoPositionDegree);

        return m_sensorValue;
    }

    std::optional<int> checkObstacleAngle()
    {
        Image processed = m_image;
        int maxElementIndex = std::max_element(processed.begin(), processed.end())
            - processed.begin();

        int maxElementValue = processed[maxElementIndex];
        if (maxElementValue < m_config.thresholdDistance) {
            return {};
        }
        m_distanceThatTriggered = maxElementValue;
        return { servoAngle(maxElementIndex) };
    }

    void check(float encoderDistance)
    {
        if (m_avoidingObstacle == AvoidingObstacle::no) {
            auto obstacle = checkObstacleAngle();

            if (!obstacle) {
                return;
            }
            if (*obstacle < 90) {
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

    AnalogIn m_sensor;
    Servo m_servo;
    Config m_config;

    AvoidingObstacle m_avoidingObstacle = AvoidingObstacle::no;
    float m_encoderDistanceStart = 0;
    int m_sensorValue = 0;
    int m_servoPositionDegree = 90;
    int m_direction = 1; // -1 = left; 1 = right
    int m_index = 0;
    int m_distanceThatTriggered = 0;
    ObstacleImage m_image;
};

} // namespace nxpcup
