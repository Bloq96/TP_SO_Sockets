#include <stdio.h>
#include <time.h>

void delayMilliseconds(int milliseconds) {
    struct timespec ts;
    ts.tv_sec = milliseconds/1000;
    ts.tv_nsec = (milliseconds%1000)*1000000;
    nanosleep(&ts,NULL);
    return;
}
