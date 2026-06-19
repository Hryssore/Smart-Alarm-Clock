#include <Arduino.h>
#include <EEPROM.h>
#include <stdio.h>
#include "config.h"
#include "lcd_wrapper.h"
#include "rtc_wrapper.h"
#include "screens.h"

#define EEPROM_MAGIC 0x42
#define EEPROM_ALARM_SET_ADDRESS 90
#define EEPROM_ALARM_TIME_ADDRESS 92
#define CANCEL_EDIT -32000

//\\                                                       \\//
//\\                           ALARM                       \\//
//\\                                                       \\//



void alarm_format_seconds(unsigned long seconds, char* buffer){
    unsigned long days = seconds / 86400UL;
    unsigned long hours = (seconds % 86400UL) / 3600UL;
    unsigned long minutes = (seconds % 3600UL) / 60UL;
    unsigned long secs = seconds % 60UL;

    if(days > 0){
        snprintf(buffer, 17, "%lud %02lu:%02lu", days, hours, minutes);
    }
    else{
        snprintf(buffer, 17, "%02lu:%02lu:%02lu", hours, minutes, secs);
    }
}

int alarm_is_set(){
    return EEPROM.read(EEPROM_ALARM_SET_ADDRESS) == EEPROM_MAGIC;
}

unsigned long alarm_time(){
    return eeprom_read_ulong(EEPROM_ALARM_TIME_ADDRESS);
}

void alarm_save(unsigned long value){
    EEPROM.put(EEPROM_ALARM_TIME_ADDRESS, value);
    EEPROM.update(EEPROM_ALARM_SET_ADDRESS, EEPROM_MAGIC);
}

void alarm_clear(){
    EEPROM.update(EEPROM_ALARM_SET_ADDRESS, 0);
}

void alarm_snooze(){
    alarm_save(datetime_to_seconds(now()) + 300UL);
    Serial.println(F("Snooze +5m"));
    lcd_backlight(true);
    lcd_clear();
    lcd_print_center(0, (char*)"Snooze +5 min");
    delay(1200);
    wait_buttons_released();
    stop_signal();
}

enum screen alarm_screen(struct context *){
    struct dt current = now();
    char line[17];
    char time_left[17];
    unsigned long current_seconds = datetime_to_seconds(current);
    unsigned long alarm_seconds = alarm_time();

    snprintf(line, sizeof(line), "Time: %02d:%02d:%02d", current.hours, current.minutes, current.seconds);
    lcd_print_center(0, line);

    if(alarm_is_set() && alarm_seconds > current_seconds){
        alarm_format_seconds(alarm_seconds - current_seconds, time_left);
        snprintf(line, sizeof(line), "Wake in:%s", time_left);
        lcd_print_center(1, line);
    }
    else if((millis() / 3000UL) % 2 == 0){
        snprintf(line, sizeof(line), "%02d.%02d.%04d", current.day, current.month, current.year);
        lcd_print_center(1, line);
    }
    else{
        lcd_print_center(1, (char*)"No alarm");
    }
    delay(25);
    return ALARM;
}

int alarm_ring_if_needed(){
    unsigned long current_seconds = datetime_to_seconds(now());
    int stop_ready = 0;
    int snooze_ready = 0;
    int ring_result = 0;

    if(!alarm_is_set()){
        return 0;
    }
    if(alarm_time() > current_seconds){
        return 0;
    }

    lcd_backlight(true);
    lcd_clear();
    lcd_print_center(0, (char*)"Wake up");
    lcd_print_center(1, (char*)"2 snooze 4 off");

    Serial.println(F("Alarm ring"));
    stop_ready = !button4_pressed();
    snooze_ready = !button2_pressed();

    while(true){
        ring_result = ring_wait(20, &stop_ready, &snooze_ready, 1);
        if(ring_result == 1){
            break;
        }
        if(ring_result == 2){
            alarm_snooze();
            return 1;
        }

        tone(BUZZER_PIN, 400, 200);
        ColorSet(255, 255, 255);
        ring_result = ring_wait(500, &stop_ready, &snooze_ready, 1);
        if(ring_result == 1){
            break;
        }
        if(ring_result == 2){
            alarm_snooze();
            return 1;
        }
    }

    alarm_clear();
    Serial.println(F("Alarm off"));
    stop_signal();
    wait_buttons_released();
    lcd_clear();
    return 1;
}

enum screen state_alarm_options(){
    int ready1 = !button1_pressed();
    int ready3 = !button3_pressed();
    int ready4 = !button4_pressed();

    if(!alarm_is_set()){
        return state_set_alarm();
    }

    lcd_print_center(0, (char*)"What you want");
    lcd_print_center(1, (char*)"4Chg 3Rst 1Can");

    while(true){
        if(button_confirm_click(button1_pressed, &ready1)){
            wait_buttons_released();
            show_operation_canceled();
            return ALARM;
        }

        if(button_confirm_click(button3_pressed, &ready3)){
            wait_buttons_released();
            alarm_clear();
            Serial.println(F("Alarm rst"));
            lcd_clear();
            lcd_print_center(0, (char*)"Alarm reset");
            delay(1000);
            lcd_clear();
            return ALARM;
        }

        if(button_confirm_click(button4_pressed, &ready4)){
            wait_buttons_released();
            return state_set_alarm();
        }

        lcd_print_center(0, (char*)"What you want");
        lcd_print_center(1, (char*)"4Chg 3Rst 1Can");
        delay(10);
    }
}

enum screen state_set_alarm(){
    struct dt dte = now();

    int d = dte.day;
    int min = dte.minutes;
    int s = dte.seconds;
    int mon = dte.month;
    int h = dte.hours;
    int y = dte.year;
    int value = 0;
    unsigned long selected_seconds = 0;
    unsigned long current_seconds = 0;
    char line[24];

    value = edit_time_value((char*)"Hr", h, min, s, 0, 23);
    if(value == CANCEL_EDIT){
        show_operation_canceled();
        return ALARM;
    }
    h = value;

    value = edit_time_value((char*)"Min", h, min, s, 1, 59);
    if(value == CANCEL_EDIT){
        show_operation_canceled();
        return ALARM;
    }
    min = value;

    value = edit_time_value((char*)"Sec", h, min, s, 2, 59);
    if(value == CANCEL_EDIT){
        show_operation_canceled();
        return ALARM;
    }
    s = value;

    value = edit_date_value((char*)"Yr", d, mon, y, 0);
    if(value == CANCEL_EDIT){
        show_operation_canceled();
        return ALARM;
    }
    y = value;
    if(d > days_in_month(mon, y)){
        d = days_in_month(mon, y);
    }

    value = edit_date_value((char*)"Mon", d, mon, y, 1);
    if(value == CANCEL_EDIT){
        show_operation_canceled();
        return ALARM;
    }
    mon = value;
    if(d > days_in_month(mon, y)){
        d = days_in_month(mon, y);
    }

    value = edit_date_value((char*)"Day", d, mon, y, 2);
    if(value == CANCEL_EDIT){
        show_operation_canceled();
        return ALARM;
    }
    d = value;

    selected_seconds = datetime_to_seconds_values(y, mon, d, h, min, s);
    current_seconds = datetime_to_seconds(now());

    if(selected_seconds <= current_seconds){
        lcd_clear();
        lcd_print_center(0, (char*)"Failed");
        lcd_print_center(1, (char*)"Past time");
        Serial.println(F("Alarm past"));
        delay(1500);
        lcd_clear();
        return ALARM;
    }

    if(confirm_operation((char*)"Set alarm?")){
        alarm_save(selected_seconds);
        snprintf(line, sizeof(line), "%02d.%02d.%04d %02d:%02d:%02d", d, mon, y, h, min, s);
        Serial.print(F("Alarm:"));
        Serial.println(line);
        show_operation_success();
    }
    else{
        show_operation_canceled();
    }

    return ALARM;

}




