#ifndef CONFIG_H
#define CONFIG_H

#define BAUD_RATE 9600

#define DHT_PIN 8
#define LDR_PIN A0

// RTC Module
#define RTC_CLK_PIN 5
#define RTC_DAT_PIN 4
#define RTC_RST_PIN 9

#define BUZZER_PIN 6

#define LCD_SDA_PIN A4
#define LCD_SCL_PIN A5
#define SDA_PIN LCD_SDA_PIN
#define SCL_PIN LCD_SCL_PIN

// LCD Display
#define LCD_I2C_ADDRESS 0x27
#define LCD_ROWS 2
#define LCD_COLS 16

#define BTN1_PIN 7
#define BTN2_PIN 3
#define BTN3_PIN 11
#define BTN4_PIN 12

// for testing purposes
// #define __DATE__ "1.1.2000"
// #define __TIME__ "00:00:00"

#define RGB_LED_BLUE 13
#define RGB_LED_RED 2
#define RGB_LED_GREEN 10
#define RGB_RED_INVERTED 0
#define RGB_GREEN_INVERTED 0
#define RGB_BLUE_INVERTED 0
#define RGB_RED_PIN RGB_LED_RED
#define RGB_GREEN_PIN RGB_LED_GREEN

#endif
