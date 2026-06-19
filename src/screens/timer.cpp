#include <Arduino.h>
#include <EEPROM.h>
#include <stdio.h>

#include "config.h"
#include "lcd_wrapper.h"
#include "rtc_wrapper.h"
#include "screens.h"

#define EEPROM_MAGIC 0x42
#define EEPROM_TIMER_SET_ADDRESS 100
#define EEPROM_TIMER_RUNNING_ADDRESS 101
#define EEPROM_TIMER_REMAINING_ADDRESS 102
#define EEPROM_TIMER_START_SECONDS_ADDRESS 106
#define EEPROM_TIMER_START_MILLIS_ADDRESS 110
#define CANCEL_EDIT -32000



void timer_change(long centiseconds);



//\\                                                       \\//
//\\                        TIMER                          \\//
//\\                                                       \\//



unsigned long timer_remaining_centiseconds(){
    unsigned long remaining = eeprom_read_ulong(EEPROM_TIMER_REMAINING_ADDRESS);
    unsigned long start_seconds;
    unsigned long current_seconds;
    unsigned long elapsed;
    unsigned long start_millis;
    unsigned long current_millis;

    if(EEPROM.read(EEPROM_TIMER_RUNNING_ADDRESS) != EEPROM_MAGIC || remaining == 0){
        return remaining;
    }

    start_seconds = eeprom_read_ulong(EEPROM_TIMER_START_SECONDS_ADDRESS);
    current_seconds = datetime_to_seconds(now());
    elapsed = 0;

    if(current_seconds >= start_seconds){
        elapsed = (current_seconds - start_seconds) * 100UL;
    }

    start_millis = eeprom_read_ulong(EEPROM_TIMER_START_MILLIS_ADDRESS);
    current_millis = millis();
    if(current_millis >= start_millis){
        elapsed = (current_millis - start_millis) / 10UL;
    }
    else{
        start_seconds = eeprom_read_ulong(EEPROM_TIMER_START_SECONDS_ADDRESS);
        current_seconds = datetime_to_seconds(now());
        elapsed = 0;

        if(current_seconds >= start_seconds){
            elapsed = (current_seconds - start_seconds) * 100UL;
        }
    }

    if(elapsed >= remaining){
        return 0;
    }

    return remaining - elapsed;
}

void timer_save_start(unsigned long centiseconds){
    EEPROM.put(EEPROM_TIMER_REMAINING_ADDRESS, centiseconds);
    EEPROM.put(EEPROM_TIMER_START_SECONDS_ADDRESS, datetime_to_seconds(now()));
    EEPROM.put(EEPROM_TIMER_START_MILLIS_ADDRESS, millis());
    EEPROM.update(EEPROM_TIMER_SET_ADDRESS, EEPROM_MAGIC);
    EEPROM.update(EEPROM_TIMER_RUNNING_ADDRESS, EEPROM_MAGIC);
}

void timer_pause(){
    unsigned long remaining = timer_remaining_centiseconds();

    EEPROM.put(EEPROM_TIMER_REMAINING_ADDRESS, remaining);
    EEPROM.update(EEPROM_TIMER_RUNNING_ADDRESS, 0);
}

void timer_resume(){
    unsigned long remaining = timer_remaining_centiseconds();

    if(remaining > 0){
        timer_save_start(remaining);
    }
}

void timer_clear(){
    unsigned long value = 0;

    EEPROM.update(EEPROM_TIMER_SET_ADDRESS, 0);
    EEPROM.update(EEPROM_TIMER_RUNNING_ADDRESS, 0);
    EEPROM.put(EEPROM_TIMER_REMAINING_ADDRESS, value);
}

int state_timer_has_value(){
    return EEPROM.read(EEPROM_TIMER_SET_ADDRESS) == EEPROM_MAGIC || timer_remaining_centiseconds() > 0;
}

int state_timer_is_paused(){
    return EEPROM.read(EEPROM_TIMER_SET_ADDRESS) == EEPROM_MAGIC && EEPROM.read(EEPROM_TIMER_RUNNING_ADDRESS) != EEPROM_MAGIC;
}

void state_timer_toggle(){
    if(EEPROM.read(EEPROM_TIMER_SET_ADDRESS) != EEPROM_MAGIC){
        return;
    }

    if(EEPROM.read(EEPROM_TIMER_RUNNING_ADDRESS) == EEPROM_MAGIC){
        timer_pause();
        Serial.println(F("T pause"));
    }
    else{
        timer_resume();
        Serial.println(F("T resume"));
    }
}

void state_timer_reset(){
    if(!state_timer_has_value()){
        return;
    }
    if(EEPROM.read(EEPROM_TIMER_RUNNING_ADDRESS) == EEPROM_MAGIC){
        return;
    }

    timer_clear();
    Serial.println(F("T reset"));
}

void state_timer_add_minute(){
    if(EEPROM.read(EEPROM_TIMER_SET_ADDRESS) == EEPROM_MAGIC && EEPROM.read(EEPROM_TIMER_RUNNING_ADDRESS) != EEPROM_MAGIC){
        timer_change(6000L);
        Serial.println(F("T +1m"));
    }
}

void state_timer_sub_minute(){
    if(EEPROM.read(EEPROM_TIMER_SET_ADDRESS) == EEPROM_MAGIC && EEPROM.read(EEPROM_TIMER_RUNNING_ADDRESS) != EEPROM_MAGIC){
        timer_change(-6000L);
        Serial.println(F("T -1m"));
    }
}

void timer_change(long centiseconds){
    unsigned long remaining = timer_remaining_centiseconds();

    if(centiseconds < 0 && remaining < (unsigned long)(0 - centiseconds)){
        remaining = 0;
    }
    else if(centiseconds < 0){
        remaining -= (unsigned long)(0 - centiseconds);
    }
    else{
        remaining += (unsigned long)centiseconds;
    }

    if(remaining == 0){
        timer_clear();
    }
    else if(EEPROM.read(EEPROM_TIMER_RUNNING_ADDRESS) == EEPROM_MAGIC){
        timer_save_start(remaining);
    }
    else{
        EEPROM.put(EEPROM_TIMER_REMAINING_ADDRESS, remaining);
        EEPROM.update(EEPROM_TIMER_SET_ADDRESS, EEPROM_MAGIC);
    }
}

enum screen timer_screen(struct context *){
    unsigned long remaining = timer_remaining_centiseconds();

    if(EEPROM.read(EEPROM_TIMER_SET_ADDRESS) != EEPROM_MAGIC){
        lcd_print_center(0, (char*)"Timer");
        lcd_print_center(1, (char*)"not set");
    }
    else{
        lcd_print_big_centiseconds(remaining);
        if(EEPROM.read(EEPROM_TIMER_RUNNING_ADDRESS) == EEPROM_MAGIC){
            lcd_print_center(1, (char*)"4 pause");
        }
        else{
            lcd_print_center(1, (char*)"4 res 3 rst");
        }
    }

    delay_or_button(300);
    return TIMER;
}

void timer_snooze(){
    stop_signal();
    wait_buttons_released();
    timer_save_start(5UL * 60UL * 100UL);
    Serial.println(F("T +5m"));
    lcd_clear();
    lcd_print_center(0, (char*)"+5 min");
    delay(1200);
    lcd_clear();
}

int timer_ring_if_needed(){
    unsigned long remaining = timer_remaining_centiseconds();
    int stop_ready = 0;
    int snooze_ready = 0;
    int ring_result = 0;

    if(EEPROM.read(EEPROM_TIMER_SET_ADDRESS) != EEPROM_MAGIC || remaining > 0){
        return 0;
    }

    lcd_backlight(true);
    lcd_clear();
    lcd_print_center(0, (char*)"Timer done");
    lcd_print_center(1, (char*)"2 +5min 4 OK");
    Serial.println(F("T done"));

    stop_ready = !button4_pressed();
    snooze_ready = !button2_pressed();
    while(true){
        ring_result = ring_wait(20, &stop_ready, &snooze_ready, 1);
        if(ring_result == 1){
            break;
        }
        if(ring_result == 2){
            timer_snooze();
            return 1;
        }

        tone(BUZZER_PIN, 300, 200);
        ColorSet(255, 255, 255);
        ring_result = ring_wait(500, &stop_ready, &snooze_ready, 1);
        if(ring_result == 1){
            break;
        }
        if(ring_result == 2){
            timer_snooze();
            return 1;
        }
    }

    stop_signal();
    wait_buttons_released();
    timer_clear();
    lcd_clear();
    return 1;
}

enum screen state_set_timer(){

    int h = 0, m = 0, s = 0;
    int value = 0;
    unsigned long total = 0;
    char line[17];

    value = edit_time_value((char*)"Hr", h, m, s, 0, 99);
    if(value == CANCEL_EDIT){
        show_operation_canceled();
        return TIMER;
    }
    h = value;

    value = edit_time_value((char*)"Min", h, m, s, 1, 59);
    if(value == CANCEL_EDIT){
        show_operation_canceled();
        return TIMER;
    }
    m = value;

    value = edit_time_value((char*)"Sec", h, m, s, 2, 59);
    if(value == CANCEL_EDIT){
        show_operation_canceled();
        return TIMER;
    }
    s = value;

    total = ((unsigned long)h * 3600UL + (unsigned long)m * 60UL + s) * 100UL;
    if(total == 0){
        lcd_clear();
        lcd_print_center(0, (char*)"Zero timer");
        delay(1000);
        lcd_clear();
        return TIMER;
    }

    if(confirm_operation((char*)"Set timer?")){
        timer_save_start(total);
        snprintf(line, sizeof(line), "%02d:%02d:%02d", h, m, s);
        Serial.print(F("Timer:"));
        Serial.println(line);
        show_operation_success();
    }
    else{
        show_operation_canceled();
    }

    return TIMER;
}
