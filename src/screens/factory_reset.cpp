#include <Arduino.h>
#include <EEPROM.h>

#include "lcd_wrapper.h"
#include "rtc_wrapper.h"
#include "screens.h"

#define EEPROM_APP_MAGIC_ADDRESS 80
#define EEPROM_STOPWATCH_START_MILLIS_ADDRESS 130



//\\                                                       \\//
//\\                     FACTORY RESET                     \\//
//\\                                                       \\//



enum screen factory_reset_screen(struct context *ctx){
    int address = EEPROM_APP_MAGIC_ADDRESS;
    // ready flags make one physical press count as one action.
    int ready3 = !button3_pressed();
    int ready4 = !button4_pressed();

    lcd_print_center(0, (char*)"Factory reset");
    lcd_print_center(1, (char*)"4 reset 3 back");

    while(true){
        // Button 3 leaves factory reset without changing saved data.
        if(button_confirm_click(button3_pressed, &ready3)){
            wait_buttons_released();
            show_menu((char*)"RESET>CLOCK");
            return CLOCK;
        }

        // Button 4 starts reset, then confirm_operation asks once more.
        if(button_confirm_click(button4_pressed, &ready4)){
            wait_buttons_released();
            if(confirm_operation((char*)"Reset all?")){
                while(address <= EEPROM_STOPWATCH_START_MILLIS_ADDRESS + 3){
                    EEPROM.update(address, 0);
                    address++;
                }

                if(ctx != 0){
                    ctx->nightlight_on = 0;
                    ctx->nightlight_started = 0;
                    ctx->nightlight_elapsed = 0;
                    ctx->nightlight_level = 0;
                }

                stop_signal();
                set_datetime(1, 1, 2026, 0, 0, 0);
                Serial.println(F("Reset done"));
                lcd_clear();
                lcd_print_center(0, (char*)"Reset done");
                delay(1200);
                lcd_clear();
                state_check_first_setup();
                return DEMO;
            }

            show_operation_canceled();
            return FACTORY_RESET;
        }

        lcd_print_center(0, (char*)"Factory reset");
        lcd_print_center(1, (char*)"4 reset 3 back");
        delay(10);
    }
}
