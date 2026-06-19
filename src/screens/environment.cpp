#include <Arduino.h>
#include <stdio.h>

#include "lcd_wrapper.h"
#include "screens.h"
#include "sensors.h"



//\\                                                       \\//
//\\                      ENVIRONMENT                      \\//
//\\                                                       \\//



enum screen show_env_screen(struct context *){
    float temperature = get_temperature();
    int humidity = get_humidity();
    char buffer1[17], buffer2[17];
    int t = temperature * 10;

    snprintf(buffer1, sizeof(buffer1),  "Temp: %d.%dC", t/10, abs(t%10));
    lcd_print_center(0, buffer1);

    snprintf(buffer2, sizeof(buffer2), "Humidity: %d%%", humidity);
    lcd_print_center(1, buffer2);

    delay(100);
    return SHOW_ENV;
}
