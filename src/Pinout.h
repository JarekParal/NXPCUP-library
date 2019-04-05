#pragma once

#include "Motor.h"
#include "Camera.h"
#include "Display.h"

namespace nxpcup
{   
    namespace detail
    {
        struct bluetooth {
            PinName TX; 
            PinName RX;
        };
    } // detail  

    namespace alamak
    {
        namespace kl25z
        {
            struct {
                nxpcup::Motor::Pinout MOTOR_LEFT {PTA4, PTA5};
                nxpcup::Motor::Pinout MOTOR_RIGHT {PTC9, PTC8};
                
                nxpcup::Camera::Pinout CAMERA1 {PTC2, PTB9, PTB8};
                nxpcup::Camera::Pinout CAMERA2 {PTC1, PTB11, PTB10};

                PinName SERVO1{PTA12};
                PinName SERVO2{PTA13};

                PinName BUTTONS{PTC0};

                SPIPreInit::Pinout SPIpinout {PTE3, PTE1, PTE2};
                Adafruit_SSD1306_SpiTfc::Pinout DISPLAY {PTE31, PTE5, PTE4};

                detail::bluetooth BLUETOOTH {PTE22, PTE23};
            } Pinout;
        } // kl25z        
    } // alamak  
} // nxpcup

