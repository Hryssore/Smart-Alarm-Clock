#include <Arduino.h>
#include "lcd_wrapper.h"
#include "screens.h"

int main() {
    // init
    struct context context;
    enum screen screen = INIT_SCR;
    unsigned long last_activity = 0;
    int button1_ready = 1;
    int button2_ready = 1;
    int button3_ready = 1;
    int button4_ready = 1;
    int any_button_was_pressed = 0;
    int buttons_pressed = 0;

    context.nightlight_on = 0;
    context.nightlight_started = 0;
    context.nightlight_elapsed = 0;
    context.nightlight_level = 0;
    context.sleep_return_screen = CLOCK;
    context.sleep_ready1 = 1;
    context.sleep_ready2 = 1;
    context.sleep_ready3 = 1;
    context.sleep_ready4 = 1;

    // main loop
    for(;;){
        if(screen == INIT_SCR){
            screen = init_screen(&context);
            last_activity = millis();
            button1_ready = !button1_pressed();
            button2_ready = !button2_pressed();
            button3_ready = !button3_pressed();
            button4_ready = !button4_pressed();
            any_button_was_pressed = any_button_pressed();
            continue;
        }

        state_check_console_password();
        if(alarm_ring_if_needed()){
            last_activity = millis();
            if(screen == SLEEP_MODE){
                screen = state_sleep_return(&context);
            }
        }
        if(timer_ring_if_needed()){
            last_activity = millis();
            if(screen == SLEEP_MODE){
                screen = state_sleep_return(&context);
            }
        }

        if(screen != NIGHTLIGHT && screen != SLEEP_MODE){
            state_nightlight_refresh(&context);
        }

        buttons_pressed = any_button_pressed();
        if(buttons_pressed && !any_button_was_pressed){
            last_activity = millis();
            lcd_backlight(true);
        }
        any_button_was_pressed = buttons_pressed;

        if(screen != DEMO && screen != SLEEP_MODE && millis() - last_activity > 60000UL){
            state_sleep_prepare(&context, screen);
            screen = SLEEP_MODE;
        }

        // in loop()
        switch(screen){
            case DEMO:
                screen = state_password_unlock(demo_screen(&context));
                last_activity = millis();
                break;

            case CLOCK:
                screen = clock_screen(&context);
                if(button_confirm_click(button4_pressed, &button4_ready)){
                    screen = state_reset_clock();
                    last_activity = millis();
                }
                if(button_confirm_click(button3_pressed, &button3_ready)){
                    show_menu((char*)"CLOCK>DATE");
                    screen = SHOW_DATE;
                    last_activity = millis();
                }
                break;
            case SHOW_DATE:
                screen = show_date_screen(&context);
                if(button_confirm_click(button4_pressed, &button4_ready)){
                    screen = state_reset_date();
                    last_activity = millis();
                }
                if(button_confirm_click(button3_pressed, &button3_ready)){
                    show_menu((char*)"DATE>ALARM");
                    screen = ALARM;
                    last_activity = millis();
                }
                break;
            case ALARM:
                screen = alarm_screen(&context);
                if(button_confirm_click(button4_pressed, &button4_ready)){
                    screen = state_alarm_options();
                    last_activity = millis();
                }
                if(button_confirm_click(button3_pressed, &button3_ready)){
                    show_menu((char*)"ALARM>ENV");
                    screen = SHOW_ENV;
                    last_activity = millis();
                }
                break;

            case SHOW_ENV:
                screen = show_env_screen(&context);
                if(button_confirm_click(button3_pressed, &button3_ready)){
                    show_menu((char*)"ENV>TIMER");
                    screen = TIMER;
                    last_activity = millis();
                }
                break;

            case TIMER: {
                int timer_had_value = state_timer_has_value();
                int button1_click = 0;
                int button2_click = 0;
                int button3_click = 0;
                int button4_click = 0;

                screen = timer_screen(&context);
                button1_click = button_confirm_click(button1_pressed, &button1_ready);
                button2_click = button_confirm_click(button2_pressed, &button2_ready);
                button3_click = button_confirm_click(button3_pressed, &button3_ready);
                button4_click = button_confirm_click(button4_pressed, &button4_ready);
                if(button2_click && !button4_click){
                    state_timer_add_minute();
                    last_activity = millis();
                }
                if(button1_click && !button4_click){
                    state_timer_sub_minute();
                    last_activity = millis();
                }
                if(button4_click){
                    if(state_timer_has_value()){
                        state_timer_toggle();
                        last_activity = millis();
                    }
                    else if(!timer_had_value){
                        screen = state_set_timer();
                        last_activity = millis();
                    }
                }
                if(button3_click){
                    if(state_timer_is_paused()){
                        state_timer_reset();
                        last_activity = millis();
                    }
                    else{
                        show_menu((char*)"TIMER>STOPW");
                        screen = STOPWATCH;
                        last_activity = millis();
                    }
                }
                break;
            }

            case STOPWATCH:
                screen = stopwatch_screen(&context);
                if(button_confirm_click(button4_pressed, &button4_ready)){
                    state_stopwatch_toggle();
                    last_activity = millis();
                }
                if(button_confirm_click(button3_pressed, &button3_ready)){
                    if(state_stopwatch_is_paused()){
                        state_stopwatch_reset();
                        last_activity = millis();
                    }
                    else{
                        show_menu((char*)"STOPW>NIGHT");
                        screen = NIGHTLIGHT;
                        last_activity = millis();
                    }
                }
                break;

            case NIGHTLIGHT:
                screen = nightlight_screen(&context);
                if(button_confirm_click(button4_pressed, &button4_ready)){
                    if(state_nightlight_is_on(&context)){
                        state_nightlight_stop(&context);
                    }
                    else{
                        state_nightlight_start(&context);
                    }
                    last_activity = millis();
                }
                if(button_confirm_click(button3_pressed, &button3_ready)){
                    show_menu((char*)"NIGHT>RESET");
                    screen = FACTORY_RESET;
                    last_activity = millis();
                }
                break;

            case SLEEP_MODE:
                screen = sleep_screen(&context);
                if(screen != SLEEP_MODE){
                    last_activity = millis();
                    button1_ready = !button1_pressed();
                    button2_ready = !button2_pressed();
                    button3_ready = !button3_pressed();
                    button4_ready = !button4_pressed();
                    any_button_was_pressed = any_button_pressed();
                }
                break;
            
            case FACTORY_RESET:
                screen = factory_reset_screen(&context);
                last_activity = millis();
                break;

            default:
                screen = CLOCK;
                break;
        }
    }
}
