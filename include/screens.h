#ifndef SCREENS_H
#define SCREENS_H

#include "context.h"
#include "rtc_wrapper.h"

/**
 * List of available screens / states.
 */
enum screen {
    ALARM,
    CLOCK,
    DEMO,
    NIGHTLIGHT,
    FACTORY_RESET,
    SHOW_DATE,
    SHOW_ENV,
    SLEEP_MODE,
    STOPWATCH,
    TIMER,
    INIT_SCR
};


/**
 * Individual implementation of screens.
 */
//\\                                                       \\//
//\\                          COMMON                       \\//
//\\                                                       \\//
void ColorSet(int R, int G, int B);
void show_menu(char* text);
void stop_signal();
int button1_pressed();
int button2_pressed();
int button3_pressed();
int button4_pressed();
int any_button_pressed();
void wait_buttons_released();
void delay_or_button(int wait_time);
int button_confirm_click(int (*button_pressed)(), int *ready);
int ring_wait(int wait_time, int *stop_ready, int *snooze_ready, int allow_snooze);
void lcd_print_center(int row, char* text);
unsigned long eeprom_read_ulong(int address);
int days_in_month(int month, int year);
unsigned long datetime_to_seconds_values(int year, int month, int day, int hours, int minutes, int seconds);
unsigned long datetime_to_seconds(struct dt value);
int edit_time_value(char* part, int h, int m, int s, int field, int max_value);
int edit_date_value(char* part, int d, int m, int y, int field);
int confirm_operation(char* text);
void show_operation_success();
void show_operation_canceled();
void lcd_print_big_time(int h, int m, int s);
void lcd_print_big_date(int d, int m, int y);
void lcd_print_big_centiseconds(unsigned long centiseconds);

enum screen init_screen(struct context *ctx);

//\\                                                       \\//
//\\                           CLOCK                       \\//
//\\                                                       \\//
enum screen clock_screen(struct context *ctx);
enum screen state_reset_clock();

//\\                                                       \\//
//\\                            DATE                       \\//
//\\                                                       \\//
enum screen show_date_screen(struct context *ctx);
enum screen state_reset_date();

//\\                                                       \\//
//\\                           ALARM                       \\//
//\\                                                       \\//
enum screen alarm_screen(struct context *ctx);
enum screen state_alarm_options();
enum screen state_set_alarm();
int alarm_ring_if_needed();

//\\                                                       \\//
//\\                        ENVIRONMENT                    \\//
//\\                                                       \\//
enum screen show_env_screen(struct context *ctx);

//\\                                                       \\//
//\\                           TIMER                       \\//
//\\                                                       \\//
enum screen timer_screen(struct context *ctx);
enum screen state_set_timer();
int timer_ring_if_needed();
int state_timer_has_value();
int state_timer_is_paused();
void state_timer_toggle();
void state_timer_reset();
void state_timer_add_minute();
void state_timer_sub_minute();

//\\                                                       \\//
//\\                         STOPWATCH                     \\//
//\\                                                       \\//
enum screen stopwatch_screen(struct context *ctx);
int state_stopwatch_is_paused();
void state_stopwatch_toggle();
void state_stopwatch_reset();

//\\                                                       \\//
//\\                        NIGHTLIGHT                     \\//
//\\                                                       \\//
enum screen nightlight_screen(struct context *ctx);
int state_nightlight_is_on(struct context *ctx);
void state_nightlight_load(struct context *ctx);
void state_nightlight_start(struct context *ctx);
void state_nightlight_stop(struct context *ctx);
void state_nightlight_refresh(struct context *ctx);

//\\                                                       \\//
//\\                           SLEEP                       \\//
//\\                                                       \\//
enum screen sleep_screen(struct context *ctx);
void state_sleep_prepare(struct context *ctx, enum screen return_screen);
enum screen state_sleep_return(struct context *ctx);

//\\                                                       \\//
//\\                         PASSWORD                      \\//
//\\                                                       \\//
enum screen demo_screen(struct context *ctx);
enum screen state_password_unlock(enum screen next_state);
void state_check_console_password();
void save_default_password();

//\\                                                       \\//
//\\                       FACTORY RESET                   \\//
//\\                                                       \\//
enum screen factory_reset_screen(struct context *ctx);

//\\                                                       \\//
//\\                         FIRST SETUP                   \\//
//\\                                                       \\//
void app_eeprom_init();
void state_check_first_setup();

#endif
