#include <Arduino.h>

#include "lcd_wrapper.h"
#include "screens.h"



//\\                                                       \\//
//\\                           SLEEP                       \\//
//\\                                                       \\//



void state_sleep_prepare(struct context *ctx, enum screen return_screen){
    if(ctx == 0){
        return;
    }

    ctx->sleep_return_screen = return_screen;
    ctx->sleep_ready1 = !button1_pressed();
    ctx->sleep_ready2 = !button2_pressed();
    ctx->sleep_ready3 = !button3_pressed();
    ctx->sleep_ready4 = !button4_pressed();
    lcd_clear();
    lcd_backlight(false);
}

enum screen state_sleep_return(struct context *ctx){
    enum screen screen = CLOCK;

    if(ctx != 0){
        screen = (enum screen)ctx->sleep_return_screen;
    }

    if(screen == SLEEP_MODE || screen == DEMO || screen == INIT_SCR){
        return CLOCK;
    }

    return screen;
}

enum screen sleep_screen(struct context *ctx){
    state_nightlight_refresh(ctx);

    if(ctx != 0 && (
        button_confirm_click(button1_pressed, &ctx->sleep_ready1) ||
        button_confirm_click(button2_pressed, &ctx->sleep_ready2) ||
        button_confirm_click(button3_pressed, &ctx->sleep_ready3) ||
        button_confirm_click(button4_pressed, &ctx->sleep_ready4)
    )){
        lcd_backlight(true);
        lcd_clear();
        wait_buttons_released();
        return state_sleep_return(ctx);
    }

    delay(50);
    return SLEEP_MODE;
}
