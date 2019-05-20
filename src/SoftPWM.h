/*
 * Copyright 2013-2018 syouichi imamori
 * (https://os.mbed.com/users/komaida424/code/SoftPWM/)
 * Copyright 2019 JarekParal (https://github.com/JarekParal)
 *
 * Partially reformat by JarekParal (github.com/JarekParal) in 2019.
 * - rewrite to one header file
 * - add namespace nxpcup
 * - rewrite _ticker.attach() params due to deprecated variant:
 * https://os.mbed.com/docs/mbed-os/v5.7/mbed-os-api-doxy/deprecated.html#_deprecated000007
 *
 * This file is under the Apache License 2.0
 * (https://choosealicense.com/licenses/apache-2.0/).
 */

#pragma once

#include "mbed.h"

#include "SoftPWM.h"

namespace nxpcup {

class SoftPWM {
public:
    SoftPWM(PinName _outpin, bool _positive = true)
        : pulse(_outpin)
    {
        if (_positive) {
            pulse = 0;
        } else {
            pulse = 1;
        }
        positive = _positive;
        interval = 0.02;
        width = 0;
        start();
    }

    operator float()
    {
        if (width <= 0.0) {
            return 0.0;
        }
        if (width > 1.0) {
            return 1.0;
        }
        return width / interval;
    }

    SoftPWM& operator=(float duty)
    {
        width = interval * duty;
        if (duty <= 0.0) {
            width = 0.0;
        }
        if (duty > 1.0) {
            width = interval;
        }
        return *this;
    }

    float read()
    {
        if (width <= 0.0) {
            return 0.0;
        }
        if (width > 1.0) {
            return 1.0;
        }
        return width / interval;
    }

    void write(float duty)
    {
        width = interval * duty;
        if (duty <= 0.0) {
            width = 0.0;
        }
        if (duty > 1.0) {
            width = interval;
        }
    }

    void start()
    {
        _ticker.attach(callback(this, &SoftPWM::TickerInterrapt), interval);
    }

    void stop()
    {
        _ticker.detach();
        if (positive) {
            pulse = 0;
        } else {
            pulse = 1;
        }
        wait(width);
    }

    void period(float _period)
    {
        interval = _period;
        start();
    }

    void period_ms(int _period)
    {
        period(float(_period) / 1000);
        start();
    }

    void period_us(int _period)
    {
        period(float(_period) / 1000000);
        start();
    }

    void pulsewidth(float _width)
    {
        width = _width;
        if (width < 0.0) {
            width = 0.0;
        }
    }

    void pulsewidth_ms(int _width) { pulsewidth(float(_width) / 1000); }

    void pulsewidth_us(int _width) { pulsewidth(float(_width) / 1000000); }

private:
    Timeout _timeout;
    Ticker _ticker;
    DigitalOut pulse;
    bool positive;
    float width;
    float interval;

    void end()
    {
        if (positive) {
            pulse = 0;
        } else {
            pulse = 1;
        }
        //    _timeout.detach();
    }

    void TickerInterrapt()
    {
        if (width <= 0) {
            return;
        }
        _timeout.attach(callback(this, &SoftPWM::end), width);
        if (positive) {
            pulse = 1;
        } else {
            pulse = 0;
        }
    }
};

} // namespace nxpcup
