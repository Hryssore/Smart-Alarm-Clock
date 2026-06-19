# ChronoGuard

ChronoGuard is a smart alarm clock built on Arduino Uno.

What started as a clock became a complete state-machine project with persistent
settings, sensors, security, timers and enough wires to make the breadboard
look mildly concerned.

## Features

- Current time and date using an RTC module
- Time and date configuration with physical buttons
- Persistent alarm stored in EEPROM
- Buzzer and RGB alarm notification
- Five-minute Snooze
- Countdown timer with hundredths of a second
- Stopwatch with pause, resume and reset
- Temperature and humidity monitoring
- Password protection using buttons or Serial Monitor
- Password changing through Serial Monitor
- Demo mode with automatic screen rotation
- Sleep mode after inactivity
- Automatic nightlight controlled by an LDR
- Persistent nightlight state and running time
- Factory reset
- Two-line LCD interface with custom characters

## Controls

- Button 1: decrease value
- Button 2: increase value
- Button 3: cancel, reset or move to the next state
- Button 4: confirm, start, pause or resume

The exact action depends on the current state.

## Project Structure

- `src/main.cpp` contains the main state machine
- `src/screens/` contains individual application states
- `src/rtc_wrapper.cpp` controls the RTC module
- `src/lcd_wrapper.cpp` provides the LCD interface
- `src/sensors.cpp` reads temperature and humidity
- `include/context.h` stores runtime state without global variables
- `include/config.h` contains the hardware pin configuration

## Data Storage

The RTC module keeps real time running independently.

EEPROM stores:

- Alarm time
- Timer state
- Stopwatch state
- Password
- First-setup state
- Nightlight state and elapsed time

This allows important settings to survive power loss.

## Hardware

- Arduino Uno
- 16x2 I2C LCD
- DS1302 RTC
- DHT11 temperature and humidity sensor
- LDR photoresistor
- RGB LED
- Passive buzzer
- Four buttons
- Approximately 35-40 wires and a respectable amount of patience

## Serial Monitor

Default baud rate:

```text
9600
