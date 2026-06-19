#include <Arduino.h>
#include <stdio.h>

#include "lcd_wrapper.h"
#include "rtc_wrapper.h"
#include "screens.h"

#define CANCEL_EDIT -32000



//\\                                                       \\//
//\\                           DATE                        \\//
//\\                                                       \\//



enum screen show_date_screen(struct context *){
    struct dt current = now();

    lcd_print_big_date(current.day, current.month, current.year);
    delay(25);
    return SHOW_DATE;
}

enum screen state_reset_date(){
    struct dt t = now();

    int d = t.day;
    int m = t.month;
    int y = t.year;
    int value = 0;
    char line[17];

    value = edit_date_value((char*)"Yr", d, m, y, 0);
    if(value == CANCEL_EDIT){
        show_operation_canceled();
        return SHOW_DATE;
    }
    y = value;
    if(d > days_in_month(m, y)){
        d = days_in_month(m, y);
    }

    value = edit_date_value((char*)"Mon", d, m, y, 1);
    if(value == CANCEL_EDIT){
        show_operation_canceled();
        return SHOW_DATE;
    }
    m = value;
    if(d > days_in_month(m, y)){
        d = days_in_month(m, y);
    }

    value = edit_date_value((char*)"Day", d, m, y, 2);
    if(value == CANCEL_EDIT){
        show_operation_canceled();
        return SHOW_DATE;
    }
    d = value;

    if(confirm_operation((char*)"Save date?")){
        set_date(d, m, y);
        snprintf(line, sizeof(line), "%02d.%02d.%04d", d, m, y);
        Serial.print(F("Date:"));
        Serial.println(line);
        show_operation_success();
    }
    else{
        show_operation_canceled();
    }

    return SHOW_DATE;
}
