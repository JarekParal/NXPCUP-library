#pragma once

#include "mbed.h"

#include "Buttons.h"
#include "Camera.h"
#include "Motor.h"
#include "MotorControl.h"
#include "ObstacleDetector.h"
#include "ObstacleDetectorWithServo.h"
#include "SoftPWM.h"
#include "atoms/control/pid.h"

#if !(defined MOTOR_HARDWARE_PWM || defined MOTOR_SOFTWARE_PWM)
#error It is neccesary to define which type of PWM you want use in Motor class: #define MOTOR_HARDWARE_PWM or #define MOTOR_SOFTWARE_PWM
#endif

#if !(defined SERVO_HARDWARE_PWM || defined SERVO_SOFTWARE_PWM)
#error It is neccesary to define which type of PWM you want use in Servo class: #define SERVO_HARDWARE_PWM or #define SERVO_SOFTWARE_PWM
#endif

namespace nxpcup {
namespace detail {
    struct Bluetooth {
        PinName TX;
        PinName RX;
    };
} // namespace detail

namespace alamak {
    namespace kl25z {
        namespace config {

            nxpcup::Motor::Config MOTOR_LEFT{ PTA4, PTA5 }; // pins with PWM
            nxpcup::Motor::Config MOTOR_RIGHT{ PTC9, PTC8 }; // pins with PWM

            nxpcup::Camera::Config CAMERA1{
                PTC2, // analog in pin
                PTB9, // clock pin
                PTB8, // scan impulse pin
                4000 // exposition in us
            };
            nxpcup::Camera::Config CAMERA2{
                PTC1, // analog in pin
                PTB11, // clock pin
                PTB10, // scan impulse pin
                4000 // exposition in us
            };

            nxpcup::Servo::Config SERVO1{
                PTA12, // pin with PWM
                -10, // angle correction,
                45, // servo min/max angle,
                0, // servo default angle
                true // inverse the servo signal - IMPORTANT information in @{Servo::Config::isReverseSignal}
            };
            nxpcup::Servo::Config SERVO2{
                PTA13, // pin with PWM
                0, // angle correction,
                45, // servo min/max angle,
                0 // servo default angle
            };

            nxpcup::MotorControl::Config MOTOR_CONTROL{}; // use the default config

            nxpcup::BorderDetector::Config BORDER_DETECTOR{};

            nxpcup::ObstacleDetector::Config OBSTACLE_DETECTOR{
                PTB2, // pin for left sensor with ADC
                PTB3, // pin for right sensor with ADC
                23000, // thresholdDistance
                18, // moveFromObstacle - lower value -> closer to detected line
                0.8, // encoderAvoidDistance (meters)
                false // obstacle detector is deactivate
            };

            nxpcup::Buttons::Config BUTTONS{
                PTC0, // analog in pin
                // { button id, boundary analog value }
                { { 3, 3840 }, { 2, 40448 }, { 4, 47104 }, { 5, 51200 }, { 6, 56320 } }
            };

            detail::Bluetooth BLUETOOTH{ PTE22, PTE23 }; // TX, RX

            nxpcup::Encoder::Config ENCODER_LEFT{
                PTD4, // pin with interrupt
                400 // pulsePerRevolution
            };
            nxpcup::Encoder::Config ENCODER_RIGHT{
                PTD6, // pin with interrupt
                400 // pulsePerRevolution
            };

            atoms::Pid<float>::Config STEERING_PID_CONFIG{
                2.5, // P constant
                0, // I constant
                0.8, // D constant
                -90, // minimal value
                90 // maximal value
            };

            static constexpr uint8_t LORISS_SEND_PERIOD_MS = 150;

        } // namespace config
    } // namespace kl25z

    namespace k66f {
        namespace config {

            nxpcup::Motor::Config MOTOR_LEFT{ PTC12, PTC5 }; // pins with PWM
            nxpcup::Motor::Config MOTOR_RIGHT{ PTA25, PTC2 }; // pins with PWM

            nxpcup::Camera::Config CAMERA1{
                PTB3, // analog in pin
                PTA26, // clock pin
                PTA27, // scan impulse pin
                4000 // exposition in us
            };
            nxpcup::Camera::Config CAMERA2{
                PTB2, // analog in pin
                PTA6, // clock pin
                PTA4, // scan impulse pin
                4000 // exposition in us
            };

            nxpcup::Servo::Config SERVO1{
                PTC8, // pin with PWM
                -10, // angle correction,
                45, // servo min/max angle,
                0, // servo default angle
                true // inverse the servo signal - IMPORTANT information in @{Servo::Config::isReverseSignal}

            };
            nxpcup::Servo::Config SERVO2{
                PTB18, // pin with PWM
                5, // angle correction,
                60, // servo min/max angle,
                0, // servo default angle
                500, // minUs pulse width
                2400 // maxUs pulse width
                // SG90 pulse width: https://servodatabase.com/servo/towerpro/sg90
            };

            nxpcup::MotorControl::Config MOTOR_CONTROL{}; // use the default config

            nxpcup::BorderDetector::Config BORDER_DETECTOR{};

            nxpcup::ObstacleDetectorWithServo::Config OBSTACLE_DETECTOR{
                PTB7, // analog in pin
                SERVO2, // servo for moving sensor
                48000, // triggerDistance
                15, // moveFromObstacle - lower value -> closer to line
                0.8 // encoderAvoidDistance (meters)
            };

            //nxpcup::Buttons::Config BUTTONS // not available due to missing ADC on the pin PTE11

            detail::Bluetooth BLUETOOTH{ PTC4, PTC3 }; // { TX, RX }

            nxpcup::Encoder::Config ENCODER_LEFT{
                PTC16, // pin with interrupt
                400 // pulsePerRevolution
            };
            nxpcup::Encoder::Config ENCODER_RIGHT{
                PTD13, // pin with interrupt
                400 // pulsePerRevolution
            };

            atoms::Pid<float>::Config STEERING_PID_CONFIG{
                2.5, // P constant
                0, // I constant
                0.8, // D constant
                -90, // minimal value
                90 // maximal value
            };

            constexpr int LORISS_SEND_PERIOD_MS = 150;

        } // namespace config
    } // namespace k66f


} // namespace alamak
} // nxpcup
