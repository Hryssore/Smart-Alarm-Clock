#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#include "config.h"
#include "lcd_wrapper.h"

// Finds the real I2C address of the LCD backpack, usually 0x27 or 0x3F.
byte lcd_runtime_address() {
    byte candidate;
    byte addresses[2] = {LCD_I2C_ADDRESS, 0x3F};
    byte index = 0;

    Wire.begin();

    while (index < 2) {
        candidate = addresses[index];
        Wire.beginTransmission(candidate);
        if (Wire.endTransmission() == 0) {
            return candidate;
        }

        index++;
    }

    return LCD_I2C_ADDRESS;
}

// address is the I2C LCD address; value is one byte sent to the expander chip.
void lcd_expander_write(byte address, byte value, byte backlight) {
    Wire.beginTransmission(address);
    Wire.write(value | backlight);
    Wire.endTransmission();
}

// Pulses the LCD enable bit so the display accepts the prepared data.
void lcd_pulse_enable(byte address, byte value, byte backlight) {
    lcd_expander_write(address, value | En, backlight);
    delayMicroseconds(1);
    lcd_expander_write(address, value & ~En, backlight);
    delayMicroseconds(50);
}

// Sends one 4-bit half-byte because this LCD works in 4-bit mode.
void lcd_write4bits(byte address, byte value, byte backlight) {
    lcd_expander_write(address, value, backlight);
    lcd_pulse_enable(address, value, backlight);
}

// Splits one byte into two 4-bit sends. mode selects command or character.
void lcd_send(byte address, byte value, byte mode, byte backlight) {
    byte high = value & 0xF0;
    byte low = (value << 4) & 0xF0;

    lcd_write4bits(address, high | mode, backlight);
    lcd_write4bits(address, low | mode, backlight);
}

void lcd_command_to(byte address, byte value) {
    lcd_send(address, value, 0, LCD_BACKLIGHT);
}

void lcd_set_cursor_to(byte address, int y, int x){
    int row_offsets[2] = {0x00, 0x40};

    if(x >= LCD_ROWS){
        x = LCD_ROWS - 1;
    }

    lcd_command_to(address, LCD_SETDDRAMADDR | (y + row_offsets[x]));
}

void lcd_create_char_to(byte address, byte location, byte* data) {
    byte i = 0;

    location &= 0x07;
    lcd_command_to(address, LCD_SETCGRAMADDR | (location << 3));
    while(i < 8){
        lcd_send(address, data[i], Rs, LCD_BACKLIGHT);
        i++;
    }
}

void lcd_write_to(byte address, char* text) {
    int i = 0;

    while(text[i] != '\0'){
        lcd_send(address, text[i], Rs, LCD_BACKLIGHT);
        i++;
    }
}

void lcd_init(){
    byte address = lcd_runtime_address();
    // These flags configure LCD size, cursor and text direction.
    byte displayfunction = LCD_4BITMODE | LCD_2LINE | LCD_5x8DOTS;
    byte displaycontrol = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
    byte displaymode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
    Wire.begin();

    delay(50);
    lcd_expander_write(address, 0, LCD_BACKLIGHT);
    delay(1000);

    lcd_write4bits(address, 0x03 << 4, LCD_BACKLIGHT);
    delayMicroseconds(4500);
    lcd_write4bits(address, 0x03 << 4, LCD_BACKLIGHT);
    delayMicroseconds(4500);
    lcd_write4bits(address, 0x03 << 4, LCD_BACKLIGHT);
    delayMicroseconds(150);
    lcd_write4bits(address, 0x02 << 4, LCD_BACKLIGHT);

    lcd_command_to(address, LCD_FUNCTIONSET | displayfunction);
    lcd_command_to(address, LCD_DISPLAYCONTROL | displaycontrol);
    lcd_command_to(address, LCD_CLEARDISPLAY);
    delayMicroseconds(2000);
    lcd_command_to(address, LCD_ENTRYMODESET | displaymode);
    lcd_command_to(address, LCD_RETURNHOME);
    delayMicroseconds(2000);

}

void lcd_create_chars(byte count, byte data[][8]) {
    byte address = lcd_runtime_address();
    byte i = 0;

    while(i < count && i < 8){
        lcd_create_char_to(address, i, data[i]);
        i++;
    }
}

void lcd_clear(){
    byte address = lcd_runtime_address();

    lcd_command_to(address, LCD_CLEARDISPLAY);
    delayMicroseconds(2000);
}

void lcd_set_cursor(int y, int x){
    byte address = lcd_runtime_address();

    lcd_set_cursor_to(address, y, x);

}

void lcd_print(char* text) {
    byte address = lcd_runtime_address();

    lcd_write_to(address, text);
}

void lcd_print_at(int y, int x, char* text) {
    lcd_set_cursor(y, x);
    lcd_print(text);
}

void lcd_write_bytes_at(int y, int x, byte* values, int length) {
    byte address = lcd_runtime_address();
    int i = 0;

    lcd_set_cursor_to(address, y, x);
    while(i < length){
        lcd_send(address, values[i], Rs, LCD_BACKLIGHT);
        i++;
    }
}

void lcd_backlight(bool state) {
    byte address = lcd_runtime_address();

    if (state) {
        lcd_send(address, LCD_DISPLAYCONTROL | LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF, 0, LCD_BACKLIGHT);
        lcd_expander_write(address, 0, LCD_BACKLIGHT);
    } else {
        lcd_send(address, LCD_DISPLAYCONTROL | LCD_CURSOROFF | LCD_BLINKOFF, 0, LCD_NOBACKLIGHT);
        lcd_expander_write(address, 0, LCD_NOBACKLIGHT);
    }
}
