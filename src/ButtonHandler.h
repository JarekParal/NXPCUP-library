#pragma once

#include <stdint.h>

#include "mbed.h"

namespace nxpcup {

class ButtonHandler {
public:
	enum ButtonName {
		none,
		S2,
		S3,
		S4,
		S5,
		S6,
	};

	ButtonHandler(PinName pin)
		:analog(pin)
	{
	}

	ButtonName getPressed()	{
		return decode();
	}

	ButtonName getPressed(ButtonName & buttonName)	{
		buttonName = decode();
		return buttonName;
	}

	bool isPressed(ButtonName buttonID)
	{
		return decode() == buttonID;
	}

private:
	ButtonName decode() {
		buttonData data;
		data.d16b = analog.read_u16();
		if(data.d8b[1] < 15) {
			return S3;
		}
		if(data.d8b[1] > 220) {
			return none;
		}
		if(data.d8b[1] < 184) {
			if(data.d8b[1] < 158) {
				return S2;
			}
			else {
				return S4;
			}
		} else {
			if(data.d8b[1] < 200) {
				return S5;
			} else {
				return S6;
			}
		}
	}

	AnalogIn analog;
	union buttonData {
		uint16_t d16b;
		uint8_t d8b[2];
	};
};

} // namespace nxpcup
