#include <Arduino.h>
#include <Wire.h>

#include "config.h"
#include "context.h"
#include "lcd_wrapper.h"
#include "rtc_wrapper.h"
#include "screens.h"
#include "sensors.h"

enum screen init_screen(struct context *ctx){
    init();
    Wire.begin();
    Serial.begin(BAUD_RATE);

    pinMode(BTN1_PIN, INPUT_PULLUP);
    pinMode(BTN2_PIN, INPUT_PULLUP);
    pinMode(BTN3_PIN, INPUT);
    pinMode(BTN4_PIN, INPUT_PULLUP);
    pinMode(LDR_PIN, INPUT);

    pinMode(RGB_LED_BLUE, OUTPUT);
    pinMode(RGB_LED_RED, OUTPUT);
    pinMode(RGB_LED_GREEN, OUTPUT);

    lcd_init();
    clock_init();
    sensors_init();
    app_eeprom_init();
    state_nightlight_load(ctx);

    return DEMO;
}
