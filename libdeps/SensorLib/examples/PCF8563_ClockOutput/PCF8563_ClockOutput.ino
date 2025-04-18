/**
 *
 * @license MIT License
 *
 * Copyright (c) 2024 lewis he
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file      PCF8563_ClockOutput.ino
 * @author    Lewis He (lewishe@outlook.com)
 * @date      2024-10-16
 *
 */
#include <Wire.h>
#include <SPI.h>
#include <Arduino.h>
#include "SensorPCF8563.hpp"

#ifndef SENSOR_SDA
#define SENSOR_SDA  17
#endif

#ifndef SENSOR_SCL
#define SENSOR_SCL  18
#endif

#ifndef SENSOR_IRQ
#define SENSOR_IRQ  7
#endif


SensorPCF8563::ClockHz clock_array[] = {
    SensorPCF8563::CLK_32_768KHZ,
    SensorPCF8563::CLK_1024KHZ,
    SensorPCF8563::CLK_32HZ,
    SensorPCF8563::CLK_1HZ,
    SensorPCF8563::CLK_DISABLE,
};

String freq_hz_str[] = {
    "32.768KHZ",
    "1.024KHZ",
    "32HZ",
    "1HZ",
    "DISABLE",
};


SensorPCF8563 rtc;
uint32_t lastMillis;
uint8_t i = 0;


void setup()
{
    Serial.begin(115200);
    while (!Serial);
    if (!rtc.begin(Wire, PCF8563_SLAVE_ADDRESS, SENSOR_SDA, SENSOR_SCL)) {
        Serial.println("Failed to find PCF8563 - check your wiring!");
        while (1) {
            delay(1000);
        }
    }
}


void loop()
{
    if (millis() - lastMillis > 5000) {
        lastMillis = millis();
        Serial.print("Set freq : ");
        Serial.println(freq_hz_str[i]);
        rtc.setClockOutput(clock_array[i]);
        i++;
        i %= 5;
    }
}


