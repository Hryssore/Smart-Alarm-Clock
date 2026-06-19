#include <Arduino.h>
#include <stdio.h>

#include "lcd_wrapper.h"
#include "screens.h"



//\\                                                       \\//
//\\                         DEMO                          \\//
//\\                                                       \\//



int demo_button_click(int *ready1, int *ready2, int *ready3, int *ready4){
    return button_confirm_click(button1_pressed, ready1) ||
        button_confirm_click(button2_pressed, ready2) ||
        button_confirm_click(button3_pressed, ready3) ||
        button_confirm_click(button4_pressed, ready4);
}

int serial_password_waiting(){
    int first = 0;

    if(!Serial.available()){
        return 0;
    }

    first = Serial.peek();
    if(first == '*' || first == '\n' || first == '\r' || first == ' ' || first == '\t'){
        return 0;
    }

    return 1;
}

int demo_background(struct context *ctx){
    int activity = 0;

    state_check_console_password();
    if(alarm_ring_if_needed()){
        activity = 1;
    }
    if(timer_ring_if_needed()){
        activity = 1;
    }
    state_nightlight_refresh(ctx);

    return activity;
}

// Demo uses the normal sleep screen, but still checks alarm, timer, nightlight and Serial.
int demo_sleep_until_activity(struct context *ctx, enum screen shown, unsigned long *last_activity){
    enum screen sleep_result = SLEEP_MODE;

    state_sleep_prepare(ctx, shown);

    while(sleep_result == SLEEP_MODE){
        if(demo_background(ctx)){
            lcd_backlight(true);
            *last_activity = millis();
            return 0;
        }

        if(serial_password_waiting()){
            lcd_backlight(true);
            lcd_clear();
            *last_activity = millis();
            return 1;
        }

        sleep_result = sleep_screen(ctx);
    }

    *last_activity = millis();
    return 1;
}

int demo_should_exit(struct context *ctx, enum screen shown, unsigned long *last_activity, int *ready1, int *ready2, int *ready3, int *ready4){
    if(demo_background(ctx)){
        *last_activity = millis();
    }

    if(serial_password_waiting()){
        *last_activity = millis();
        return 1;
    }

    if(demo_button_click(ready1, ready2, ready3, ready4)){
        *last_activity = millis();
        return 1;
    }

    if(millis() - *last_activity > 60000UL){
        return demo_sleep_until_activity(ctx, shown, last_activity);
    }

    return 0;
}

char* demo_mode_name(int step){
    if(step == 0){
        return (char*)"Clock";
    }
    if(step == 1){
        return (char*)"Date";
    }
    if(step == 2){
        return (char*)"Alarm";
    }
    if(step == 3){
        return (char*)"Env";
    }
    if(step == 4){
        return (char*)"Timer";
    }
    if(step == 5){
        return (char*)"Stopw";
    }

    return (char*)"Night";
}

enum screen show_demo_step(int step, struct context *ctx){
    if(step == 0){
        clock_screen(ctx);
        return CLOCK;
    }
    if(step == 1){
        show_date_screen(ctx);
        return SHOW_DATE;
    }
    if(step == 2){
        alarm_screen(ctx);
        return ALARM;
    }
    if(step == 3){
        show_env_screen(ctx);
        return SHOW_ENV;
    }
    if(step == 4){
        timer_screen(ctx);
        return TIMER;
    }
    if(step == 5){
        stopwatch_screen(ctx);
        return STOPWATCH;
    }

    nightlight_screen(ctx);
    return NIGHTLIGHT;
}

enum screen demo_screen(struct context *ctx){
    enum screen shown = CLOCK;
    int step = 0;
    int previous_step = 6;
    int ticks = 0;
    int exit_demo = 0;
    int ready1 = !button1_pressed();
    int ready2 = !button2_pressed();
    int ready3 = !button3_pressed();
    int ready4 = !button4_pressed();
    unsigned long last_activity = millis();
    char line[17];

    lcd_backlight(true);
    lcd_clear();
    lcd_print_center(0, (char*)"Demo rezim");
    lcd_print_center(1, (char*)"Tap any button");

    ticks = 0;
    while(ticks < 20 && !exit_demo){
        if(demo_should_exit(ctx, shown, &last_activity, &ready1, &ready2, &ready3, &ready4)){
            exit_demo = 1;
        }
        if(!exit_demo){
            delay(100);
            ticks++;
        }
    }

    lcd_clear();

    while(!exit_demo){
        if(demo_should_exit(ctx, shown, &last_activity, &ready1, &ready2, &ready3, &ready4)){
            exit_demo = 1;
        }

        if(!exit_demo){
            lcd_clear();
            lcd_print_center(0, demo_mode_name(step));
            snprintf(line, sizeof(line), "%s >> %s", demo_mode_name(previous_step), demo_mode_name(step));
            lcd_print_center(1, line);

            ticks = 0;
            while(ticks < 10 && !exit_demo){
                if(demo_should_exit(ctx, shown, &last_activity, &ready1, &ready2, &ready3, &ready4)){
                    exit_demo = 1;
                }
                if(!exit_demo){
                    delay(100);
                    ticks++;
                }
            }

            lcd_clear();
        }

        if(!exit_demo){
            shown = show_demo_step(step, ctx);
        }

        ticks = 0;
        while(ticks < 50 && !exit_demo){
            if(demo_should_exit(ctx, shown, &last_activity, &ready1, &ready2, &ready3, &ready4)){
                exit_demo = 1;
            }
            if(!exit_demo){
                delay(100);
                ticks++;
            }
        }

        previous_step = step;
        step++;
        if(step > 6){
            step = 0;
        }
        lcd_clear();
    }

    wait_buttons_released();
    return shown;
}
