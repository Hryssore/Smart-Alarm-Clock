#include <Arduino.h>
#include <stdio.h>


#include "rtc_wrapper.h"
#include "screens.h"



#define CANCEL_EDIT -32000





//\\                                                       \\//
//\\                           CLOCK                       \\//
//\\                                                       \\//




enum screen clock_screen(struct context *){
    struct dt current = now();

    lcd_print_big_time(current.hours, current.minutes, current.seconds);
    delay(25);
    return CLOCK;
}

enum screen state_reset_clock(){
    struct dt t = now();

    int h = t.hours;
    int m = t.minutes;
    int s = t.seconds;
    int value = 0;
    char line[17];

    value = edit_time_value((char*)"Hr", h, m, s, 0, 23);
    if(value == CANCEL_EDIT){
        show_operation_canceled();
        return CLOCK;
    }
    h = value;

    value = edit_time_value((char*)"Min", h, m, s, 1, 59);
    if(value == CANCEL_EDIT){
        show_operation_canceled();
        return CLOCK;
    }
    m = value;

    value = edit_time_value((char*)"Sec", h, m, s, 2, 59);
    if(value == CANCEL_EDIT){
        show_operation_canceled();
        return CLOCK;
    }
    s = value;

    if(confirm_operation((char*)"Save clock?")){
        set_time(h, m, s);
        snprintf(line, sizeof(line), "%02d:%02d:%02d", h, m, s);
        Serial.print(F("Clock:"));
        Serial.println(line);
        show_operation_success();
    }
    else{
        show_operation_canceled();
    }

    return CLOCK;

}

