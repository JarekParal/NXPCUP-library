#pragma once

#include "mbed.h"
#include "util.h"

namespace nxpcup {

class Servo {
public:
	/**
	 * Constructor for Servo.
	 * @param pinName name of the servo pin
	 * @param minUs minimal pulse wide (default for 0 degree)
	 * @param minUs maximal pulse wide (default for 180 degree)
	 */
	Servo(PinName pinName, uint16_t minUs = 1000, uint16_t maxUs = 2000)
		: servo(pinName),
		  minUs(minUs),
		  maxUs(maxUs)
	{
		servo.period_us(PERIOD_US);
		servo.pulsewidth_us(CENTER_US);
	}

	/**
	 * Set correction angle for functions working with angle.
	 * @param angle which will be add to the set angle
	 */
	void angleCorrection(int8_t angle) {
		correctionAngle = angle;
	}

	/**
	 * Set maximal allowed maximal and minimal angle.
	 * @param min suggested range  (0 <-> 180)
	 * @param max suggested range  (0 <-> 180)
	 */
	void angleMinMax(uint8_t min, uint8_t max) {
		minAngle = min + correctionAngle;
		maxAngle = max + correctionAngle;
	}

	/**
	 * Set maximal allowed angle from center (middle).
	 * @param diffAngle allowed +- range from center
	 * 		  suggested range (0 <-> 90) - not checked by this function
	 */
	void angleMinMaxCenter(uint8_t diffAngle) {
		minAngle = 90 - diffAngle + correctionAngle;
		maxAngle = 90 + diffAngle + correctionAngle;
	}

	/**
	 * Set position of the servo in degree.
	 * @param degree in range (0 <-> 180)
	 */
	void angle(uint8_t degree) {
		lastAngle = nxpcup::clamp<uint8_t>(degree + correctionAngle, minAngle, maxAngle);
		uint16_t positionInMicrosecond =
				minUs + (lastAngle * usDiff) / 180; // 180 degree = max range
		microsecond(positionInMicrosecond);
	}

	/**
	 * Set position of the servo in degree around center (middle position).
	 * @param degree in range (-90 <-> 90)
	 * @param reverse the range (0 = 0, 90 => -90, -90 => 90)
	 */
	void angleCenter(int8_t degree, bool reverse = false) {
		angle(90 + ((reverse ? -1 : 1) * degree));
	}

	/**
	 * Get last set angle - value could be clamped in @{angle()}
	 */
	uint8_t getAngle() const
	{
		return lastAngle;
	}

	/**
	 * Get last set angle from center - value could be clamped in @{angle()}
	 */
	int8_t getCenterAngle() const
	{
		return lastAngle - 90;
	}

private:
	/**
	 * Set position (pulse wide) of the servo in microsecond.
	 */
	void microsecond(uint16_t microsecond) {
		servo.pulsewidth_us(microsecond);
	}

	uint8_t lastAngle = 0;
	PwmOut servo;
	uint8_t minAngle = 0;
	uint8_t maxAngle = 180;
	int8_t correctionAngle = 0;

	const uint32_t minUs = 1000;
	const uint32_t maxUs = 2000;
	const uint32_t usDiff = maxUs - minUs;

	static const uint16_t PERIOD_US = 20000;  // 50 Hz
	static const uint16_t CENTER_US = 1500;  // 50 Hz
};

} // namespace nxpcup
