#include <Arduino.h>
#include <EEPROM.h>
#include <stdio.h>
#include <string.h>

#include "config.h"
#include "lcd_wrapper.h"
#include "screens.h"

#define EEPROM_MAGIC 0x42
#define EEPROM_PASSWORD_SET_ADDRESS 82
#define EEPROM_PASSWORD_ADDRESS 83
#define PASSWORD_LENGTH 4



//\\                                                       \\//
//\\                        PASSWORD                       \\//
//\\                                                       \\//



void save_password(char* password){
    int i = 0;

    while(i < PASSWORD_LENGTH){
        EEPROM.update(EEPROM_PASSWORD_ADDRESS + i, password[i]);
        i++;
    }

    EEPROM.update(EEPROM_PASSWORD_SET_ADDRESS, EEPROM_MAGIC);
    Serial.print(F("Pass:"));
    Serial.println(password);
}

void read_saved_password(char* password){
    int i = 0;

    while(i < PASSWORD_LENGTH){
        password[i] = EEPROM.read(EEPROM_PASSWORD_ADDRESS + i);
        if(password[i] < '0' || password[i] > '9'){
            password[i] = '0';
        }
        i++;
    }

    password[PASSWORD_LENGTH] = '\0';
}

void save_default_password(){
    char password[PASSWORD_LENGTH + 1] = "0000";

    save_password(password);
}

// Clears old serial characters after lock, so next password attempt starts clean.
void clear_serial_input(){
    while(Serial.available()){
        Serial.read();
    }
}

int read_password_from_serial(char* password, unsigned long timeout){
    int i = 0;
    char value = 0;
    unsigned long started = millis();

    while(i < PASSWORD_LENGTH){
        if(Serial.available()){
            value = Serial.read();
            if(value >= '0' && value <= '9'){
                password[i] = value;
                i++;
                started = millis();
            }
            else if(value == '\n' || value == '\r' || value == ' ' || value == '\t' || value == '*'){
                started = millis();
            }
        }
        else{
            if(millis() - started > timeout){
                password[i] = '\0';
                return 0;
            }
            delay(20);
        }
    }

    password[PASSWORD_LENGTH] = '\0';
    return 1;
}

int read_password_from_buttons(char* password, int attempt){
    int position = 0;
    int digit = 0;
    int ready1 = 0;
    int ready2 = 0;
    int ready3 = 0;
    int ready4 = 0;
    char line[17];

    while(position < PASSWORD_LENGTH){
        digit = 0;
        wait_buttons_released();
        ready1 = !button1_pressed();
        ready2 = !button2_pressed();
        ready3 = !button3_pressed();
        ready4 = !button4_pressed();

        while(true){
            if(Serial.available()){
                if(read_password_from_serial(password, 30000UL)){
                    return 1;
                }
            }

            if(button_confirm_click(button3_pressed, &ready3)){
                wait_buttons_released();
                return 0;
            }

            if(button_confirm_click(button4_pressed, &ready4)){
                password[position] = '0' + digit;
                position++;
                wait_buttons_released();
                break;
            }

            if(button_confirm_click(button2_pressed, &ready2)){
                digit++;
                if(digit > 9){
                    digit = 0;
                }
            }
            if(button_confirm_click(button1_pressed, &ready1)){
                digit--;
                if(digit < 0){
                    digit = 9;
                }
            }

            if((millis() / 2000UL) % 2 == 0){
                lcd_print_center(0, (char*)"Enter pass");
            }
            // else{
            //     lcd_print_center(0, (char*)"Try left");
            // }

            snprintf(line, sizeof(line), "Try %d/3", attempt);
            lcd_print_center(1, line);
            delay(20);
        }
    }

    password[PASSWORD_LENGTH] = '\0';
    wait_buttons_released();
    return 1;
}

void wrong_password_alarm(int tries_left){
    char line[17];
    int i = 0;

    Serial.print(F("Bad pass:"));
    Serial.println(tries_left);

    lcd_clear();
    lcd_print_center(0, (char*)"Wrong pass");
    snprintf(line, sizeof(line), "Tries: %d", tries_left);
    lcd_print_center(1, line);

    while(i < 10){
        tone(BUZZER_PIN, 300, 200);
        ColorSet(255, 0, 0);
        delay(250);
        noTone(BUZZER_PIN);
        ColorSet(0, 0, 0);
        delay(250);
        i++;
    }

    stop_signal();
}

enum screen state_password_unlock(enum screen next_state){
    char saved[PASSWORD_LENGTH + 1];
    char entered[PASSWORD_LENGTH + 1];
    int attempt = 0;
    int tries_left = 3;

    app_eeprom_init();
    stop_signal();

    if(EEPROM.read(EEPROM_PASSWORD_SET_ADDRESS) != EEPROM_MAGIC){
        save_default_password();
    }

    read_saved_password(saved);
    wait_buttons_released();

    while(attempt < 3){
        stop_signal();
        lcd_clear();
        lcd_print_center(0, (char*)"Enter pass");
        Serial.println(F("Enter pass"));

        if(!read_password_from_buttons(entered, attempt + 1)){
            return DEMO;
        }

        if(strcmp(saved, entered) == 0){
            Serial.println(F("Pass OK"));
            ColorSet(0, 255, 0);
            lcd_clear();
            lcd_print_center(0, (char*)"Welcome");
            delay(1800);
            stop_signal();
            lcd_clear();
            return next_state;
        }

        attempt++;
        tries_left = 3 - attempt;
        wrong_password_alarm(tries_left);
    }

    Serial.println(F("Lock 30s"));
    lcd_clear();
    lcd_print_center(0, (char*)"Locked");
    lcd_print_center(1, (char*)"Wait 30 sec");
    ColorSet(255, 0, 0);
    delay(30000);
    clear_serial_input();
    stop_signal();
    lcd_clear();
    return DEMO;
}

void state_check_console_password(){
    char password[PASSWORD_LENGTH + 1];
    int first = 0;

    if(!Serial.available()){
        return;
    }

    first = Serial.peek();
    if(first == '\n' || first == '\r' || first == ' ' || first == '\t'){
        Serial.read();
        return;
    }

    if(first != '*'){
        return;
    }

    Serial.read();
    Serial.println(F("*1234"));
    if(read_password_from_serial(password, 30000UL)){
        save_password(password);
        lcd_clear();
        lcd_print_center(0, (char*)"Pass saved");
        delay(1000);
        lcd_clear();
    }
    else{
        Serial.println(F("Pass cancel"));
    }
}
