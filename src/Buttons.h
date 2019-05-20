#pragma once

#include <vector>

#include "mbed.h"

namespace nxpcup {

using ButtonId = int;

class Buttons {
public:
    struct Config {
        struct ButtonSetting {
            uint16_t id; /**< identificator of the button **/
            uint16_t boundaryValue; /**< upper boundary value for this button **/
        };

        PinName analogPin; /**< analogPin for the buttons **/
        std::vector<ButtonSetting> buttonSettings; /**< vector of the struct @{ButtonSetting} **/
    };

    /**
     * Constructor of class Buttons.
     *
     * @param config structure @{Config}
     */
    Buttons(const Config& config)
        : m_config(config)
        , m_analogIn(config.analogPin){};

    /**
     * Return the id of pressed button.
     *
     * @return @{ButtonId} type
     */
    ButtonId getPressed() { return decode(); }

    /**
     * Return the id of pressed button and save it to parameter buttonId.
     *
     * @param buttonId variable for saving current value
     * @return @{ButtonId} type
     */
    ButtonId getPressed(ButtonId& buttonId)
    {
        return (buttonId = decode());
    }

    /**
     * Check button with specific id.
     *
     * @param buttonId id of the button
     * @return true if is pressed, else false
     */
    bool isPressed(ButtonId buttonId)
    {
        return decode() == buttonId;
    }

    /**
     * Wait for press button with specific id.
     *
     * @param buttonId id of button on which will wait
     */
    void waitForPress(ButtonId buttonId)
    {
        while (!isPressed(buttonId)) {
            wait(0.1);
        }
    }

private:
    /**
     * Decode the actual state of buttons.
     *
     * @return actual pressed buttonId
     */
    ButtonId decode()
    {
        int analogValue = m_analogIn.read_u16();
        for (const auto& button : m_config.buttonSettings) {
            if (analogValue < button.boundaryValue) {
                return button.id;
            }
        }
        return -1;
    }

    const Config& m_config;
    AnalogIn m_analogIn;
};

} // namespace nxpcup
