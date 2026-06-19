#include <Arduino.h>
#include <EEPROM.h>
#include <stdio.h>

#include "config.h"
#include "lcd_wrapper.h"
#include "screens.h"

#define EEPROM_MAGIC 0x42
#define EEPROM_NIGHTLIGHT_ADDRESS 114
#define EEPROM_NIGHTLIGHT_SECONDS_ADDRESS 115



//\\                                                       \\//
//\\                        NIGHTLIGHT                     \\//
//\\                                                       \\//



int nightlight_level(){
    int raw = analogRead(LDR_PIN);
    int darkness = 1023 - raw;
    int level = 0;

    if(darkness < 60){
        return 0;
    }

    // Converts light sensor darkness to a soft LED brightness range.
    level = map(darkness, 60, 360, 4, 70);
    if(level < 0){
        level = 0;
    }
    if(level > 70){
        level = 70;
    }

    return level;
}

unsigned long nightlight_seconds(struct context *ctx){
    if(ctx == 0 || !ctx->nightlight_on){
        return 0;
    }

    return ctx->nightlight_elapsed + (millis() - ctx->nightlight_started) / 1000UL;
}

void state_nightlight_save_seconds(struct context *ctx){
    unsigned long seconds = 0;

    if(ctx == 0 || !ctx->nightlight_on){
        return;
    }

    seconds = (millis() - ctx->nightlight_started) / 1000UL;
    if(seconds == 0){
        return;
    }

    ctx->nightlight_elapsed += seconds;
    ctx->nightlight_started += seconds * 1000UL;
    EEPROM.put(EEPROM_NIGHTLIGHT_SECONDS_ADDRESS, ctx->nightlight_elapsed);
}

void state_nightlight_refresh(struct context *ctx){
    int target = 0;
    int level = 0;
    int red = 0;
    int green = 0;

    if(ctx == 0 || !ctx->nightlight_on){
        return;
    }

    state_nightlight_save_seconds(ctx);

    target = nightlight_level();
    level = ctx->nightlight_level;

    if(target == 0){
        level = 0;
    }
    else if(target > level + 2){
        level += 2;
    }
    else if(target < level - 2){
        level -= 2;
    }
    else{
        level = target;
    }

    ctx->nightlight_level = level;

    if(level == 0){
        ColorSet(0, 0, 0);
    }
    else{
        red = map(level, 4, 70, 80, 150);
        green = map(level, 4, 70, 8, 28);
        ColorSet(red, green, 0);
    }
}

int state_nightlight_is_on(struct context *ctx){
    if(ctx == 0){
        return 0;
    }

    return ctx->nightlight_on;
}

void state_nightlight_save(int enabled){
    EEPROM.update(EEPROM_NIGHTLIGHT_ADDRESS, enabled ? EEPROM_MAGIC : 0);
}

void state_nightlight_load(struct context *ctx){
    if(ctx == 0){
        return;
    }

    ctx->nightlight_on = EEPROM.read(EEPROM_NIGHTLIGHT_ADDRESS) == EEPROM_MAGIC;
    ctx->nightlight_started = millis();
    ctx->nightlight_elapsed = eeprom_read_ulong(EEPROM_NIGHTLIGHT_SECONDS_ADDRESS);
    ctx->nightlight_level = 0;

    if(ctx->nightlight_on){
        state_nightlight_refresh(ctx);
    }
}

void state_nightlight_start(struct context *ctx){
    if(ctx == 0){
        return;
    }

    if(!ctx->nightlight_on){
        ctx->nightlight_started = millis();
        ctx->nightlight_elapsed = 0;
        ctx->nightlight_level = 0;
        ctx->nightlight_on = 1;
        state_nightlight_save(1);
        EEPROM.put(EEPROM_NIGHTLIGHT_SECONDS_ADDRESS, ctx->nightlight_elapsed);
        Serial.println(F("Night on"));
        lcd_clear();
    }
}

void state_nightlight_stop(struct context *ctx){
    if(ctx == 0){
        return;
    }

    ctx->nightlight_on = 0;
    ctx->nightlight_started = 0;
    ctx->nightlight_elapsed = 0;
    ctx->nightlight_level = 0;
    state_nightlight_save(0);
    EEPROM.put(EEPROM_NIGHTLIGHT_SECONDS_ADDRESS, ctx->nightlight_elapsed);
    stop_signal();
    Serial.println(F("Night off"));
    lcd_clear();
}

void nightlight_print_time(unsigned long seconds){
    char line[17];
    unsigned long hours = seconds / 3600UL;
    unsigned long minutes = (seconds % 3600UL) / 60UL;
    unsigned long secs = seconds % 60UL;

    snprintf(line, sizeof(line), "On %02lu:%02lu:%02lu", hours % 100, minutes, secs);
    lcd_print_center(0, line);
}

void nightlight_wait(struct context *ctx, int wait_time){
    unsigned long started = millis();

    while(millis() - started < (unsigned long)wait_time && !any_button_pressed()){
        state_nightlight_refresh(ctx);
        delay(20);
    }
}

// Shows nightlight state. Button handling is in main, this only draws the screen.
enum screen nightlight_screen(struct context *ctx){
    unsigned long seconds = 0;

    if(ctx != 0 && ctx->nightlight_on){
        state_nightlight_refresh(ctx);
        seconds = nightlight_seconds(ctx);
        if(ctx->nightlight_level == 0){
            lcd_print_center(0, (char*)"Enough light");
        }
        else{
            nightlight_print_time(seconds);
        }
        lcd_print_center(1, (char*)"4 off 3 next");
    }
    else{
        lcd_print_center(0, (char*)"Nightlight");
        lcd_print_center(1, (char*)"4 on 3 next");
    }

    nightlight_wait(ctx, 150);
    return NIGHTLIGHT;
}
