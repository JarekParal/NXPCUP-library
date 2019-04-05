#pragma once

#include "mbed.h"
#include "util.h"

namespace nxpcup {

class Motor {
public:
	struct Pinout {
		PinName pwm0; /**< pin0 with PWM for motor driver **/
		PinName pwm1; /**< pin1 with PWM for motor driver **/
	};

	bool inverse = false;

	/**
	 * Constructor of class Motor.
	 * @param pin0 name of the pin for motor driver (require PWM)
	 * @param pin0 name of the pin for motor driver (require PWM)
	 */
	Motor(const PinName pin0, const PinName pin1)
		: max_power_percent(100)
	{
		in0 = new PwmOut(pin0);
		in1 = new PwmOut(pin1);

		in0->period_us(PERIOD_US);
		in1->period_us(PERIOD_US);

		in0->pulsewidth_us(0);
		in1->pulsewidth_us(0);
	}
	
	/**
	 * Constructor of class Motor.
	 * @param pinout structure @{Pinout}
	 */
	Motor(const Pinout &pinout) : Motor(pinout.pwm0, pinout.pwm1) {
	}

	/**
	 * Set motor power.
	 * @param power of the motor (-1000 <-> 1000)
	 * 		  under 0 => backward, over 0 => forward, 0 stop
	 */
	void power(int32_t power) {
		power = nxpcup::clamp<int16_t>(power, -MAX_POWER, MAX_POWER);

		power = (inverse ? -1 : 1) * power;
		if(max_power_percent != 100) {
			power = (power * max_power_percent) / 100;
		}

		if (power > 0) {
			in0->pulsewidth_us(power / POWER_DIVIDER);
			in1->pulsewidth_us(0);
		} else {
			in0->pulsewidth_us(0);
			in1->pulsewidth_us(-power / POWER_DIVIDER);
		}
	}

	/**
	 * Set the maximal motor power in percent.
	 * @param percent of the maximal power (0 <-> 100)
	 * 		  function power() has still same range
	 */
	void maxPowerPercent(uint8_t percent) {
		max_power_percent = nxpcup::clamp<uint8_t>(percent, 0, 100);
	}

	/**
	 * Return the actual maximal motor power in percent (0 <-> 100).
	 */
	uint8_t maxPowerPercent() {
		return max_power_percent;
	}

	/**
	 * Return the constant maximal power.
	 */
	uint16_t maxPower() {
		return MAX_POWER;
	}

private:
	PwmOut *in0;
	PwmOut *in1;
	uint8_t max_power_percent;

	static const int16_t PERIOD_US = 500;  // 2000 Hz
	static const int16_t MAX_POWER = 1000; // MUST BE DIVISIBLE BY PERIOD_US
	static const uint8_t POWER_DIVIDER = MAX_POWER / PERIOD_US; // 1000 / 2 = 500
};

} // namespace nxpcup
