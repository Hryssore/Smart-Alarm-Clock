#ifndef CONTEXT_H
#define CONTEXT_H

struct context {
    int nightlight_on;
    unsigned long nightlight_started;
    unsigned long nightlight_elapsed;
    int nightlight_level;
    int sleep_return_screen;
    int sleep_ready1;
    int sleep_ready2;
    int sleep_ready3;
    int sleep_ready4;
};

#endif
