#include <Arduino.h>
#include <EEPROM.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "lcd_wrapper.h"
#include "rtc_wrapper.h"
#include "screens.h"

#define CANCEL_EDIT -32000



//\\                                                       \\//
//\\                           COMMON                      \\//
//\\                                                       \\//



void rgb_write(int pin, int value, int inverted){
    if(inverted){
        value = 255 - value;
    }

    pinMode(pin, OUTPUT);

    if(value <= 0){
        digitalWrite(pin, LOW);
    }
    else if(value >= 255){
        digitalWrite(pin, HIGH);
    }
    else{
        analogWrite(pin, value);
    }
}

void ColorSet(int R, int G, int B){
    rgb_write(RGB_LED_RED, R, RGB_RED_INVERTED);
    rgb_write(RGB_LED_GREEN, G, RGB_GREEN_INVERTED);
    rgb_write(RGB_LED_BLUE, B, RGB_BLUE_INVERTED);
}

void stop_signal(){
    noTone(BUZZER_PIN);
    ColorSet(0, 0, 0);
}

int button1_pressed(){
    return digitalRead(BTN1_PIN) == LOW;
}

int button2_pressed(){
    return digitalRead(BTN2_PIN) == LOW;
}

int button3_pressed(){
    return digitalRead(BTN3_PIN) == HIGH;
}

int button4_pressed(){
    return digitalRead(BTN4_PIN) == LOW;
}

int any_button_pressed(){
    return button1_pressed() || button2_pressed() || button3_pressed() || button4_pressed();
}

void wait_buttons_released(){
    unsigned long started = millis();

    while(any_button_pressed() && millis() - started < 180){
        delay(10);
    }
}

void delay_or_button(int wait_time){
    int waited = 0;

    while(waited < wait_time && !any_button_pressed()){
        delay(20);
        waited += 20;
    }
}

// button_pressed is a pointer to a button function, for example button4_pressed.
// ready blocks repeated clicks while the same button is still held down.
int button_confirm_click(int (*button_pressed)(), int *ready){
    if(!button_pressed()){
        *ready = 1;
        return 0;
    }

    if(*ready){
        delay(20);
        if(button_pressed()){
            *ready = 0;
            return 1;
        }
    }

    return 0;
}

int ring_wait(int wait_time, int *stop_ready, int *snooze_ready, int allow_snooze){
    int passed = 0;

    while(passed < wait_time){
        if(button_confirm_click(button4_pressed, stop_ready)){
            return 1;
        }
        if(allow_snooze && button_confirm_click(button2_pressed, snooze_ready)){
            return 2;
        }

        delay(20);
        passed += 20;
    }

    return 0;
}

void show_menu(char* text){
    delay(50);
    lcd_clear();
    lcd_print_center(0, (char*)"MENU");
    delay(200);
    lcd_print_center(1, text);
    delay(1000);
    lcd_clear();
}

void lcd_print_center(int row, char* text){
    char line[17];
    int len = strlen(text);
    int start = 0;
    int i = 0;

    if(len > 16){
        len = 16;
    }

    start = (16 - len) / 2;

    while(i < 16){
        line[i] = ' ';
        i++;
    }
    line[16] = '\0';

    i = 0;
    while(i < len){
        line[start + i] = text[i];
        i++;
    }

    lcd_print_at(0, row, line);
}

void big_line_clear(byte* line){
    int i = 0;

    while(i < 16){
        line[i] = ' ';
        i++;
    }
}

void digit_pattern(int digit, byte* data){
    data[0] = 0b00000;
    data[1] = 0b00000;
    data[2] = 0b00000;
    data[3] = 0b00000;
    data[4] = 0b00000;
    data[5] = 0b00000;
    data[6] = 0b00000;
    data[7] = 0b00000;

    if(digit == 0){
        data[1] = 0b01110;
        data[2] = 0b01010;
        data[3] = 0b01010;
        data[4] = 0b01010;
        data[5] = 0b01010;
        data[6] = 0b01110;
    }
    if(digit == 1){
        data[1] = 0b00010;
        data[2] = 0b00110;
        data[3] = 0b01010;
        data[4] = 0b00010;
        data[5] = 0b00010;
        data[6] = 0b00010;
    }
    if(digit == 2){
        data[1] = 0b01110;
        data[2] = 0b01010;
        data[3] = 0b00010;
        data[4] = 0b01110;
        data[5] = 0b01000;
        data[6] = 0b01110;
    }
    if(digit == 3){
        data[1] = 0b01110;
        data[2] = 0b00010;
        data[3] = 0b00010;
        data[4] = 0b01110;
        data[5] = 0b00010;
        data[6] = 0b01110;
    }
    if(digit == 4){
        data[1] = 0b01010;
        data[2] = 0b01010;
        data[3] = 0b01110;
        data[4] = 0b00010;
        data[5] = 0b00010;
        data[6] = 0b00010;
    }
    if(digit == 5){
        data[1] = 0b01110;
        data[2] = 0b01000;
        data[3] = 0b01000;
        data[4] = 0b01110;
        data[5] = 0b00010;
        data[6] = 0b01110;
    }
    if(digit == 6){
        data[1] = 0b01110;
        data[2] = 0b01010;
        data[3] = 0b01000;
        data[4] = 0b01110;
        data[5] = 0b01010;
        data[6] = 0b01110;
    }
    if(digit == 7){
        data[1] = 0b01110;
        data[2] = 0b01010;
        data[3] = 0b00010;
        data[4] = 0b00110;
        data[5] = 0b00010;
        data[6] = 0b00010;
    }
    if(digit == 8){
        data[1] = 0b01110;
        data[2] = 0b01010;
        data[3] = 0b01110;
        data[4] = 0b01010;
        data[5] = 0b01010;
        data[6] = 0b01110;
    }
    if(digit == 9){
        data[1] = 0b01110;
        data[2] = 0b01010;
        data[3] = 0b01110;
        data[4] = 0b00010;
        data[5] = 0b00010;
        data[6] = 0b01110;
    }
}

// Finds which custom LCD character slot already contains this digit.
int digit_find(int digit, int* digits, int count){
    int i = 0;

    while(i < count){
        if(digits[i] == digit){
            return i;
        }
        i++;
    }

    return -1;
}

// Adds a digit to the custom character list only once.
void digit_add(int digit, int* digits, int *count){
    if(digit_find(digit, digits, *count) < 0 && *count < 8){
        digits[*count] = digit;
        (*count)++;
    }
}

// Loads selected digit bitmaps into the LCD custom character memory.
void digit_load(int* digits, int count){
    byte data[8][8];
    int i = 0;

    while(i < count){
        digit_pattern(digits[i], data[i]);
        i++;
    }

    lcd_create_chars(count, data);
}

// Puts a custom digit slot into the prepared 16-character LCD row.
void big_set_digit(byte* line, int pos, int digit, int* digits, int count){
    int slot = digit_find(digit, digits, count);

    if(slot >= 0){
        line[pos] = slot;
    }
}

// Prints two two-digit values, like HH:MM, and optional small seconds.
void lcd_print_big_pair(int left, char separator, int right, int small){
    byte top[16];
    byte bottom[16];
    int digits[4];
    int count = 0;
    int start = 5;

    big_line_clear(top);
    big_line_clear(bottom);
    digit_add((left / 10) % 10, digits, &count);
    digit_add(left % 10, digits, &count);
    digit_add((right / 10) % 10, digits, &count);
    digit_add(right % 10, digits, &count);
    digit_load(digits, count);

    big_set_digit(top, start, (left / 10) % 10, digits, count);
    big_set_digit(top, start + 1, left % 10, digits, count);
    top[start + 2] = separator;
    big_set_digit(top, start + 3, (right / 10) % 10, digits, count);
    big_set_digit(top, start + 4, right % 10, digits, count);
    if(small >= 0){
        bottom[14] = '0' + (small / 10) % 10;
        bottom[15] = '0' + small % 10;
    }

    lcd_write_bytes_at(0, 0, top, 16);
    lcd_write_bytes_at(0, 1, bottom, 16);
}

void lcd_print_big_time(int h, int m, int s){
    lcd_print_big_pair(h, ':', m, s);
}

// Prints date as big day/month on top and year on bottom.
void lcd_print_big_date(int d, int m, int y){
    byte top[16];
    byte bottom[16];
    int digits[8];
    int count = 0;
    int top_start = 5;
    int bottom_start = 6;

    big_line_clear(top);
    big_line_clear(bottom);
    digit_add((d / 10) % 10, digits, &count);
    digit_add(d % 10, digits, &count);
    digit_add((m / 10) % 10, digits, &count);
    digit_add(m % 10, digits, &count);
    digit_add((y / 1000) % 10, digits, &count);
    digit_add((y / 100) % 10, digits, &count);
    digit_add((y / 10) % 10, digits, &count);
    digit_add(y % 10, digits, &count);
    digit_load(digits, count);

    big_set_digit(top, top_start, (d / 10) % 10, digits, count);
    big_set_digit(top, top_start + 1, d % 10, digits, count);
    top[top_start + 2] = '.';
    big_set_digit(top, top_start + 3, (m / 10) % 10, digits, count);
    big_set_digit(top, top_start + 4, m % 10, digits, count);
    top[top_start + 5] = '.';

    big_set_digit(bottom, bottom_start, (y / 1000) % 10, digits, count);
    big_set_digit(bottom, bottom_start + 1, (y / 100) % 10, digits, count);
    big_set_digit(bottom, bottom_start + 2, (y / 10) % 10, digits, count);
    big_set_digit(bottom, bottom_start + 3, y % 10, digits, count);

    lcd_write_bytes_at(0, 0, top, 16);
    lcd_write_bytes_at(0, 1, bottom, 16);
}

void lcd_print_big_centiseconds(unsigned long centiseconds){
    unsigned long hours = centiseconds / 360000UL;
    unsigned long minutes = (centiseconds % 360000UL) / 6000UL;
    unsigned long seconds = (centiseconds % 6000UL) / 100UL;
    unsigned long hundredths = centiseconds % 100UL;
    char line[17];

    if(hours > 0){
        snprintf(line, sizeof(line), "%02lu:%02lu:%02lu.%02lu", hours % 100, minutes, seconds, hundredths);
    }
    else{
        snprintf(line, sizeof(line), "%02lu:%02lu.%02lu", minutes, seconds, hundredths);
    }

    lcd_print_center(0, line);
}

// Reads a 4-byte unsigned long from EEPROM. Empty EEPROM reads as 0.
unsigned long eeprom_read_ulong(int address){
    unsigned long value = 0;

    EEPROM.get(address, value);
    if(value == 0xFFFFFFFFUL){
        value = 0;
    }

    return value;
}

int is_leap_year(int year){
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int days_in_month(int month, int year){
    if(month == 2){
        if(is_leap_year(year)){
            return 29;
        }
        return 28;
    }

    if(month == 4 || month == 6 || month == 9 || month == 11){
        return 30;
    }

    return 31;
}

// Converts date and time to one increasing number of seconds since year 2000.
unsigned long datetime_to_seconds_values(int year, int month, int day, int hours, int minutes, int seconds){
    unsigned long days = 0;
    int value = 2000;

    if(year < 2000){
        year = 2000;
    }

    while(value < year){
        days += 365;
        if(is_leap_year(value)){
            days++;
        }
        value++;
    }

    value = 1;
    while(value < month){
        days += days_in_month(value, year);
        value++;
    }

    if(day > 0){
        days += day - 1;
    }

    return days * 86400UL + (unsigned long)hours * 3600UL + (unsigned long)minutes * 60UL + seconds;
}

// Same conversion, but takes the RTC dt structure instead of separate values.
unsigned long datetime_to_seconds(struct dt value){
    return datetime_to_seconds_values(value.year, value.month, value.day, value.hours, value.minutes, value.seconds);
}

void show_operation_success(){
    lcd_clear();
    lcd_print_center(0, (char*)"Success");
    delay(1200);
    lcd_clear();
}

void show_operation_canceled(){
    lcd_clear();
    lcd_print_center(0, (char*)"Canceled");
    delay(1000);
    lcd_clear();
}

char* edit_label(char* part){
    if(part[0] == 'H'){
        return (char*)"Hr";
    }
    if(part[0] == 'S'){
        return (char*)"Sc";
    }
    if(part[0] == 'Y'){
        return (char*)"Yr";
    }
    if(part[0] == 'D'){
        return (char*)"Dy";
    }
    if(part[1] == 'o'){
        return (char*)"Mo";
    }
    return (char*)"Mn";
}

void show_edit_time(char* part, int h, int m, int s){
    lcd_print_big_time(h, m, s);
    lcd_print_at(14, 0, edit_label(part));
}

void show_edit_date(char* part, int d, int m, int y){
    lcd_print_big_date(d, m, y);
    lcd_print_at(14, 0, edit_label(part));
}

int confirm_operation(char* text){
    int ready3 = !button3_pressed();
    int ready4 = !button4_pressed();

    lcd_print_center(0, text);
    lcd_print_center(1, (char*)"4 yes 3 no");

    while(true){
        if(button_confirm_click(button3_pressed, &ready3)){
            wait_buttons_released();
            return 0;
        }

        if(button_confirm_click(button4_pressed, &ready4)){
            wait_buttons_released();
            return 1;
        }

        lcd_print_center(0, text);
        lcd_print_center(1, (char*)"4 yes 3 no");
        delay(10);
    }
}

// Edits one time field: 0 hours, 1 minutes, 2 seconds.
int edit_time_value(char* part, int h, int m, int s, int field, int max_value){
    int value = h;
    int ready1 = !button1_pressed();
    int ready2 = !button2_pressed();
    int ready3 = !button3_pressed();
    int ready4 = !button4_pressed();
    int redraw = 0;

    if(field == 1){
        value = m;
    }
    if(field == 2){
        value = s;
    }

    if(field == 0){
        h = value;
    }
    if(field == 1){
        m = value;
    }
    if(field == 2){
        s = value;
    }

    show_edit_time(part, h, m, s);

    while(true){
        if(button_confirm_click(button3_pressed, &ready3)){
            return CANCEL_EDIT;
        }

        if(button_confirm_click(button4_pressed, &ready4)){
            return value;
        }

        if(button_confirm_click(button2_pressed, &ready2)){
            value++;
            if(value > max_value){
                value = 0;
            }
            redraw = 1;
        }
        if(button_confirm_click(button1_pressed, &ready1)){
            value--;
            if(value < 0){
                value = max_value;
            }
            redraw = 1;
        }

        if(field == 0){
            h = value;
        }
        if(field == 1){
            m = value;
        }
        if(field == 2){
            s = value;
        }

        if(redraw){
            show_edit_time(part, h, m, s);
            redraw = 0;
        }
        delay(10);
    }
}

int edit_date_value(char* part, int d, int m, int y, int field){
    int value = y;
    int min_value = 2026;
    int max_value = 2099;
    int ready1 = !button1_pressed();
    int ready2 = !button2_pressed();
    int ready3 = !button3_pressed();
    int ready4 = !button4_pressed();
    int redraw = 0;

    if(field == 1){
        value = m;
        min_value = 1;
        max_value = 12;
    }
    if(field == 2){
        value = d;
        min_value = 1;
        max_value = days_in_month(m, y);
    }

    if(field == 0){
        y = value;
        if(d > days_in_month(m, y)){
            d = days_in_month(m, y);
        }
    }
    if(field == 1){
        m = value;
        if(d > days_in_month(m, y)){
            d = days_in_month(m, y);
        }
    }
    if(field == 2){
        d = value;
    }

    show_edit_date(part, d, m, y);

    while(true){
        if(button_confirm_click(button3_pressed, &ready3)){
            return CANCEL_EDIT;
        }

        if(button_confirm_click(button4_pressed, &ready4)){
            return value;
        }

        if(button_confirm_click(button2_pressed, &ready2)){
            value++;
            if(value > max_value){
                value = min_value;
            }
            redraw = 1;
        }
        if(button_confirm_click(button1_pressed, &ready1)){
            value--;
            if(value < min_value){
                value = max_value;
            }
            redraw = 1;
        }

        if(field == 0){
            y = value;
            if(d > days_in_month(m, y)){
                d = days_in_month(m, y);
            }
        }
        if(field == 1){
            m = value;
            if(d > days_in_month(m, y)){
                d = days_in_month(m, y);
            }
        }
        if(field == 2){
            d = value;
        }

        if(redraw){
            show_edit_date(part, d, m, y);
            redraw = 0;
        }
        delay(10);
    }
}
