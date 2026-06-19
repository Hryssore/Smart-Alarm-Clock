#ifndef HELPERS_H
#define HELPERS_H

#include <Arduino.h>

/**
 * @brief Prints formatted string (including floating point variables) to serial line.
 *
 * @tparam T
 * @param str format string
 * @param args list of arguments to print
 */
template <typename... T>
void print(const char *str, T... args) {
    int len = snprintf(NULL, 0, str, args...);

    if(len){
        char buff[len + 1];
        snprintf(buff, len + 1, str, args...);
        Serial.print(buff);
    }
}

#endif
