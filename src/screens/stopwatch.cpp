#include <Arduino.h>
#include <EEPROM.h>

#include "lcd_wrapper.h"
#include "rtc_wrapper.h"
#include "screens.h"

#define EEPROM_MAGIC 0x42
#define EEPROM_STOPWATCH_RUNNING_ADDRESS 120
#define EEPROM_STOPWATCH_ELAPSED_ADDRESS 122
#define EEPROM_STOPWATCH_START_SECONDS_ADDRESS 126
#define EEPROM_STOPWATCH_START_MILLIS_ADDRESS 130



//\\                                                       \\//
//\\                       STOPWATCH                       \\//
//\\                                                       \\//



unsigned long stopwatch_elapsed_centiseconds(){
    unsigned long elapsed = eeprom_read_ulong(EEPROM_STOPWATCH_ELAPSED_ADDRESS);
    unsigned long start_seconds;
    unsigned long current_seconds;
    unsigned long start_millis;
    unsigned long current_millis;

    if(EEPROM.read(EEPROM_STOPWATCH_RUNNING_ADDRESS) != EEPROM_MAGIC){
        return elapsed;
    }

    start_seconds = eeprom_read_ulong(EEPROM_STOPWATCH_START_SECONDS_ADDRESS);
    current_seconds = datetime_to_seconds(now());

    start_millis = eeprom_read_ulong(EEPROM_STOPWATCH_START_MILLIS_ADDRESS);
    current_millis = millis();
    if(current_millis >= start_millis){
        elapsed += (current_millis - start_millis) / 10UL;
    }
    else if(current_seconds >= start_seconds){
        elapsed += (current_seconds - start_seconds) * 100UL;
    }

    return elapsed;
}

void stopwatch_start(){
    EEPROM.put(EEPROM_STOPWATCH_START_SECONDS_ADDRESS, datetime_to_seconds(now()));
    EEPROM.put(EEPROM_STOPWATCH_START_MILLIS_ADDRESS, millis());
    EEPROM.update(EEPROM_STOPWATCH_RUNNING_ADDRESS, EEPROM_MAGIC);
}

void stopwatch_pause(){
    EEPROM.put(EEPROM_STOPWATCH_ELAPSED_ADDRESS, stopwatch_elapsed_centiseconds());
    EEPROM.update(EEPROM_STOPWATCH_RUNNING_ADDRESS, 0);
}

void stopwatch_clear(){
    unsigned long value = 0;

    EEPROM.put(EEPROM_STOPWATCH_ELAPSED_ADDRESS, value);
    EEPROM.update(EEPROM_STOPWATCH_RUNNING_ADDRESS, 0);
}

int state_stopwatch_is_paused(){
    return EEPROM.read(EEPROM_STOPWATCH_RUNNING_ADDRESS) != EEPROM_MAGIC && stopwatch_elapsed_centiseconds() > 0;
}

void state_stopwatch_toggle(){
    if(EEPROM.read(EEPROM_STOPWATCH_RUNNING_ADDRESS) == EEPROM_MAGIC){
        stopwatch_pause();
        Serial.println(F("SW pause"));
    }
    else{
        stopwatch_start();
        Serial.println(F("SW start"));
    }
}

void state_stopwatch_reset(){
    if(EEPROM.read(EEPROM_STOPWATCH_RUNNING_ADDRESS) == EEPROM_MAGIC){
        return;
    }
    if(stopwatch_elapsed_centiseconds() == 0){
        return;
    }

    stopwatch_clear();
    Serial.println(F("SW reset"));
}

enum screen stopwatch_screen(struct context *){
    unsigned long elapsed = stopwatch_elapsed_centiseconds();

    if(EEPROM.read(EEPROM_STOPWATCH_RUNNING_ADDRESS) == EEPROM_MAGIC){
        lcd_print_big_centiseconds(elapsed);
        lcd_print_center(1, (char*)"4 pause");
    }
    else if(elapsed > 0){
        lcd_print_big_centiseconds(elapsed);
        lcd_print_center(1, (char*)"4 res 3 rst");
    }
    else{
        lcd_print_center(0, (char*)"Stopwatch");
        lcd_print_center(1, (char*)"4 start");
    }
    delay_or_button(200);
    return STOPWATCH;
}
