#include <Arduino.h>
#include <RtcDS1302.h>
#include <ThreeWire.h>

#include "config.h"
#include "rtc_wrapper.h"

// The reference lets this function configure the RTC object created by the caller.
void rtc_prepare(RtcDS1302<ThreeWire>& rtc) {
    RtcDateTime current;

    rtc.Begin();
    rtc.SetIsWriteProtected(false);

    if (!rtc.GetIsRunning()) {
        rtc.SetIsRunning(true);
    }

    current = rtc.GetDateTime();
    if (!current.IsValid() || current.Year() < 2026 || current.Year() > 2099) {
        rtc.SetDateTime(RtcDateTime(__DATE__, __TIME__));
    }
}

void rtc_init() {
    ThreeWire wire(RTC_DAT_PIN, RTC_CLK_PIN, RTC_RST_PIN);
    RtcDS1302<ThreeWire> rtc(wire);

    rtc_prepare(rtc);
}

void clock_init() {
    rtc_init();
}
void set_date(const byte day, const byte month, const int year) {
    struct dt current = now();

    set_datetime(day, month, year, current.hours, current.minutes, current.seconds);
}

void set_time(const byte hours, const byte minutes, const byte seconds) {
    struct dt current = now();

    set_datetime(current.day, current.month, current.year, hours, minutes, seconds);
}

void set_datetime(const byte day, const byte month, const int year, const byte hours, const byte minutes, const byte seconds) {
    ThreeWire wire(RTC_DAT_PIN, RTC_CLK_PIN, RTC_RST_PIN);
    RtcDS1302<ThreeWire> rtc(wire);

    rtc_prepare(rtc);
    rtc.SetDateTime(RtcDateTime(year, month, day, hours, minutes, seconds));
}

byte get_day() {
    return now().day;
}

byte get_month() {
    return now().month;
}

int get_year() {
    return now().year;
}

byte get_hours() {
    return now().hours;
}

byte get_minutes() {
    return now().minutes;
}

byte get_seconds() {
    return now().seconds;
}

struct dt now() {
    ThreeWire wire(RTC_DAT_PIN, RTC_CLK_PIN, RTC_RST_PIN);
    RtcDS1302<ThreeWire> rtc(wire);
    RtcDateTime current;
    struct dt value;

    rtc_prepare(rtc);
    current = rtc.GetDateTime();
    
    value.day = current.Day();
    value.month = current.Month();
    value.year = current.Year();
    value.hours = current.Hour();
    value.minutes = current.Minute();
    value.seconds = current.Second();

    return value;
}
