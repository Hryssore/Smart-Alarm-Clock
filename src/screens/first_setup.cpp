#include <Arduino.h>
#include <EEPROM.h>

#include "lcd_wrapper.h"
#include "screens.h"

#define EEPROM_MAGIC 0x42
#define EEPROM_APP_MAGIC_ADDRESS 80
#define EEPROM_SETUP_ADDRESS 81
#define EEPROM_PASSWORD_SET_ADDRESS 82
#define EEPROM_ALARM_SET_ADDRESS 90
#define EEPROM_ALARM_TIME_ADDRESS 92
#define EEPROM_TIMER_SET_ADDRESS 100
#define EEPROM_TIMER_RUNNING_ADDRESS 101
#define EEPROM_TIMER_REMAINING_ADDRESS 102
#define EEPROM_TIMER_START_SECONDS_ADDRESS 106
#define EEPROM_TIMER_START_MILLIS_ADDRESS 110
#define EEPROM_NIGHTLIGHT_ADDRESS 114
#define EEPROM_NIGHTLIGHT_SECONDS_ADDRESS 115
#define EEPROM_STOPWATCH_RUNNING_ADDRESS 120
#define EEPROM_STOPWATCH_ELAPSED_ADDRESS 122
#define EEPROM_STOPWATCH_START_SECONDS_ADDRESS 126
#define EEPROM_STOPWATCH_START_MILLIS_ADDRESS 130



//\\                                                       \\//
//\\                     FIRST SETUP                       \\//
//\\                                                       \\//



void app_eeprom_init(){
    unsigned long value = 0;

    if(EEPROM.read(EEPROM_APP_MAGIC_ADDRESS) == EEPROM_MAGIC){
        return;
    }

    EEPROM.update(EEPROM_APP_MAGIC_ADDRESS, EEPROM_MAGIC);
    EEPROM.update(EEPROM_SETUP_ADDRESS, 0);
    EEPROM.update(EEPROM_PASSWORD_SET_ADDRESS, 0);
    EEPROM.update(EEPROM_ALARM_SET_ADDRESS, 0);
    EEPROM.update(EEPROM_TIMER_SET_ADDRESS, 0);
    EEPROM.update(EEPROM_TIMER_RUNNING_ADDRESS, 0);
    EEPROM.update(EEPROM_NIGHTLIGHT_ADDRESS, 0);
    EEPROM.update(EEPROM_STOPWATCH_RUNNING_ADDRESS, 0);

    EEPROM.put(EEPROM_ALARM_TIME_ADDRESS, value);
    EEPROM.put(EEPROM_TIMER_REMAINING_ADDRESS, value);
    EEPROM.put(EEPROM_TIMER_START_SECONDS_ADDRESS, value);
    EEPROM.put(EEPROM_TIMER_START_MILLIS_ADDRESS, value);
    EEPROM.put(EEPROM_NIGHTLIGHT_SECONDS_ADDRESS, value);
    EEPROM.put(EEPROM_STOPWATCH_ELAPSED_ADDRESS, value);
    EEPROM.put(EEPROM_STOPWATCH_START_SECONDS_ADDRESS, value);
    EEPROM.put(EEPROM_STOPWATCH_START_MILLIS_ADDRESS, value);
}

void state_check_first_setup(){
    app_eeprom_init();

    if(EEPROM.read(EEPROM_PASSWORD_SET_ADDRESS) != EEPROM_MAGIC){
        lcd_clear();
        lcd_print_center(0, (char*)"Pass is 0000");
        lcd_print_center(1, (char*)"Change by *");
        save_default_password();
        delay(1500);
        lcd_clear();
    }

    if(EEPROM.read(EEPROM_SETUP_ADDRESS) == EEPROM_MAGIC){
        return;
    }

    Serial.println(F("Setup start"));
    lcd_clear();
    lcd_print_center(0, (char*)"First setup");
    lcd_print_center(1, (char*)"Use buttons");
    delay(1200);
    lcd_clear();

    state_reset_clock();
    state_reset_date();

    EEPROM.update(EEPROM_SETUP_ADDRESS, EEPROM_MAGIC);
    Serial.println(F("Setup done"));
    lcd_clear();
}
