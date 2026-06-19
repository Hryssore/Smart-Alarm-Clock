#include <Arduino.h>
#include <math.h>
#include <DHT.h>

#include "config.h"
#include "sensors.h"

void sensors_init() {
    DHT dht(DHT_PIN, DHT11);

    dht.begin();
}

float get_temperature() {
    DHT dht(DHT_PIN, DHT11);

    dht.begin();
    return dht.readTemperature();
}

int get_humidity() {
    DHT dht(DHT_PIN, DHT11);
    float humidity;

    dht.begin();
    humidity = dht.readHumidity();

    if (isnan(humidity)) {
        return -1;
    }

    if (humidity < 0.0f) {
        return 0;
    }

    if (humidity > 100.0f) {
        return 100;
    }

    return (int)(humidity + 0.5f);
}
