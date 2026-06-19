# AlarmClock

AlarmClock is a compact Arduino-based bedside clock built as an embedded systems project.  
It combines a real-time clock, LCD user interface, alarm features, environment monitoring, stopwatch, and countdown timer in one small device.

This project was made with a simple goal: build a clock that is useful, understandable, and easy to explain during presentation or defense.

## What this project can do

- show current time and date
- store time using RTC backup battery
- set and save an alarm
- stop alarm with a button
- snooze alarm by adding or subtracting minutes
- display temperature and humidity from DHT11
- run a stopwatch
- run a countdown timer
- show seconds and hundredths in stopwatch mode
- provide a simple first-setup guide after reset

## Hardware used

- Arduino Uno
- RTC module
- 16x2 I2C LCD
- DHT11 sensor
- buzzer
- buttons
- RGB LED

## Interface

The device is controlled only with buttons, so the whole menu works directly on the LCD.

You can:
- switch between modes
- edit time and date
- set alarm values
- start or stop stopwatch
- set countdown timer
- snooze alarm without resetting everything

## Main modes

- clock
- date
- alarm
- environment
- stopwatch
- countdown timer
- factory reset / first setup

## Why this project is interesting

This is not just another digital clock.  
The main idea of the project is to combine several practical embedded features into one device while keeping the code readable and the behavior stable.

The project focuses on:
- state machine logic
- RTC time handling
- EEPROM saving
- button debounce
- LCD rendering without unnecessary flicker
- simple embedded-friendly code style

## Build

The project is built with PlatformIO and Arduino framework.

## Notes

The code was written in a simple embedded style without unnecessary overengineering.  
The goal was to keep the project practical, explainable, and suitable for school presentation as well as further extension.

## Author

Student embedded systems project based on Arduino and PlatformIO.
