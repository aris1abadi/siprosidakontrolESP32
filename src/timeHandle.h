#ifndef TIMEHANDLE_H
#define TIMEHANDLE_H

void time_init();
void time_update();
void time_set(uint8_t second, uint8_t minute, uint8_t hour, uint8_t dayOfWeek, uint8_t dayOfMonth, uint8_t month, uint8_t year);
void ntp_init();
void ntp_sync();
void time_loop();
String getTimeNow();

#endif