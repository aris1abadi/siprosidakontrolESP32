#ifndef siramHandle_h
#define siramHandle_h

void siram_start(uint8_t lahan,uint8_t dur);
void siram_stop(uint8_t lahan);
bool getSiram_status();
void setSiram_status(bool sts);
bool getSiramLahan1_status();
bool getSiramLahan2_status();
bool getSiramLahan3_status();
bool getSiramLahan4_status();
String getAllSiram_status();
void siram_loop();
void cekJadwalSiram();
void siramJadwal_start();
void siramKey_start();

#endif