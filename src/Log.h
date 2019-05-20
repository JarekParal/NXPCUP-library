#pragma once

// Header file with logging functions

#include "BorderDetector.h"
#include "mbed.h"
#include <optional>
#include <type_traits>

template <typename T>
void send32bits(Serial& serial, T data)
{
    static_assert(sizeof(T) == 4);
    int32_t cdata = static_cast<int32_t>(data);
    serial.putc(cdata >> 24);
    serial.putc(cdata >> 16);
    serial.putc(cdata >> 8);
    serial.putc(cdata);
}

template <typename T>
void send16bits(Serial& serial, T data)
{
    static_assert(sizeof(T) == 2);
    int16_t cdata = static_cast<int16_t>(data);
    serial.putc(cdata >> 8);
    serial.putc(cdata);
}

template <>
void send32bits<float>(Serial& serial, float data)
{
    uint8_t* cdata = reinterpret_cast<uint8_t*>(&data);
    for (int i : { 0, 1, 2, 3 }) {
        serial.putc(cdata[i]);
    }
}

template <typename T>
void send64bits(Serial& serial, T data)
{
    static_assert(sizeof(T) == 8);
    int64_t cdata = static_cast<int64_t>(data);
    serial.putc(cdata >> 56);
    serial.putc(cdata >> 48);
    serial.putc(cdata >> 40);
    serial.putc(cdata >> 32);
    serial.putc(cdata >> 24);
    serial.putc(cdata >> 16);
    serial.putc(cdata >> 8);
    serial.putc(cdata);
}

template <>
void send64bits<double>(Serial& serial, double data)
{
    uint8_t* cdata = reinterpret_cast<uint8_t*>(&data);
    for (int i : { 0, 1, 2, 3, 4, 5, 6, 7 }) {
        serial.putc(cdata[i]);
    }
}

void sendCameraDataLorris(Serial& serial, const std::array<uint16_t, 128>& data)
{
    serial.putc(0x80); // Header
    serial.putc(0x01); // Command: 0x01 = camera
    serial.putc(128); // Packet data length: N bytes data after this packet
    for (uint16_t elem : data) {
        serial.putc(elem >> 8);
    }
}

void sendDetectorDataLorris(Serial& serial, nxpcup::BorderDetector& detector)
{
    serial.putc(0x80); // Header
    serial.putc(0x02); // Command: 0x02 = detector
    serial.putc(9); // Packet data length: 3 bytes data after this packet
    send32bits(serial, detector.leftBorder());
    send32bits(serial, detector.rightBorder());
    serial.putc(detector.error());
}

void sendTimeDataLorris(
    Serial& serial, Timer& loopTime, const uint16_t loopTimePeriodOverflowCounter)
{
    serial.putc(0x80); // Header
    serial.putc(0x03); // Command: 0x03 = time
    serial.putc(6); // Packet data length
    send32bits(serial, loopTime.read_us()); // 4 bytes
    send16bits(serial, loopTimePeriodOverflowCounter); // 2 bytes
}

void sendEncoderDataLorris(
    Serial& serial, uint16_t dataLeft, uint16_t dataRight)
{
    serial.putc(0x80); // Header
    serial.putc(0x04); // Command: 0x04 = counter
    serial.putc(4); // Packet data length: x bytes data (int) after this packet
    serial.putc(dataLeft >> 8);
    serial.putc(dataLeft);
    serial.putc(dataRight >> 8);
    serial.putc(dataRight);
}

void sendPeaksDataLorris(Serial& serial, const int16_t peaks)
{
    serial.putc(0x80); // Header
    serial.putc(0x05); // Command: 0x05 = peaks from border detector
    serial.putc(2); // Packet data length
    send16bits(serial, peaks);
}

void sendObstacleDataLorris(
    Serial& serial,
    const int obstacleDistance,
    const int obstacleAngle,
    const int distanceThatTriggered,
    const int avoidingObstacle)
{
    serial.putc(0x80); // Header
    serial.putc(0x06); // Command: 0x06 = distance from obstacle sensors
    serial.putc(4 * 3 + 1); // Packet data length
    send32bits(serial, obstacleDistance);
    send32bits(serial, obstacleAngle);
    send32bits(serial, distanceThatTriggered);
    serial.putc(static_cast<int8_t>(avoidingObstacle));
}

void sendObstacleDetectorDataLorris(
    Serial& serial,
    const int leftSensorValue,
    const int rightSensorValue,
    const int avoidingObstacle)
{
    serial.putc(0x80); // Header
    serial.putc(0x06); // Command: 0x06 = distance from obstacle sensors
    serial.putc(4 * 2 + 1); // Packet data length
    send32bits(serial, leftSensorValue);
    send32bits(serial, rightSensorValue);
    serial.putc(static_cast<int8_t>(avoidingObstacle));
}

void sendSteeringDataLorris(
    Serial& serial, nxpcup::BorderDetector& detector, const int roadError)
{
    serial.putc(0x80); // Header
    serial.putc(0x07); // Command: 0x07 - steering data
    serial.putc(5);
    serial.putc(detector.error());
    send32bits(serial, roadError);
}

void sendMotorDataLorris(
    Serial& serial,
    const float motorLD,
    const float motorLA,
    const float motorRD,
    const float motorRA)
{
    serial.putc(0x80); // Header
    serial.putc(0x08); // Command: 0x08 - motor data
    serial.putc(16);
    send32bits(serial, motorLD * 1000);
    send32bits(serial, motorLA * 1000);
    send32bits(serial, motorRD * 1000);
    send32bits(serial, motorRA * 1000);
}

void sendEncoderDistanceLorris(
    Serial& serial, float distanceLeft, float distanceRight)
{
    serial.putc(0x80); // Header
    serial.putc(0x09); // Command: 0x09 = encoder distance
    serial.putc(8); // Packet data length: x bytes data (int) after this packet
    send32bits(serial, int(distanceLeft * 1000));
    send32bits(serial, int(distanceRight * 1000));
}

void sendCameraDataTerminal(
    Serial& serial, const std::array<uint16_t, 128>& data)
{
    serial.printf("L:");
    for (uint16_t d : data) {
        serial.printf("%X,", d / 16);
    }
}
